/*
 * ProgramController.cpp
 *
 *  Created on: Mar 14, 2013
 *      Author: brancotti
 */

#include "ProgramController.h"
#include "ProgramParser.h"
#include "Commands.h"
#include "Footboard.h"

ProgramController::ProgramController() :
		running(true), loopCount(0)
{
	programCounter[0] = programCounter[1] = 0;
	loopTime = lastLogArduinoDataTime = boost::posix_time::microsec_clock::local_time();
	samplingLogFile.open("sampling.log");
	completeLogFile.open("complete.log");
	claxon.reset(new Sound("claxon.wav"));
}

ProgramController::~ProgramController()
{
	samplingLogFile.close();
	completeLogFile.close();
	cout << "ProgramController dtor" << endl;
}

bool ProgramController::Run(void)
{
//    boost::posix_time::ptime t1 = boost::posix_time::second_clock::local_time();
//    //boost::this_thread::sleep(boost::posix_time::millisec(500));
//    boost::posix_time::ptime t2 = boost::posix_time::second_clock::local_time();
//    boost::posix_time::time_duration diff = t2 - t1;
//    std::cout << diff.total_milliseconds() << std::endl;
//
//    boost::posix_time::ptime mst1 = boost::posix_time::microsec_clock::local_time();
//    // boost::this_thread::sleep(boost::posix_time::millisec(500));
//    boost::posix_time::ptime mst2 = boost::posix_time::microsec_clock::local_time();
//    boost::posix_time::time_duration msdiff = mst2 - mst1;
//    std::cout << msdiff.total_milliseconds() << std::endl;

    // cout.precision(4);
	startTime = boost::posix_time::microsec_clock::local_time();

	while (running)
	{

		ptime timeNow = boost::posix_time::microsec_clock::local_time();
		secondsFromStart = (timeNow - startTime).total_microseconds() / 1000000.0;
		deltaT = timeNow - loopTime;
		loopTime = timeNow;
		++loopCount;

		// leggo lo stato della pedana
		int readStatus = footboard->GetStateFromArduino();
		if(readStatus != 0)
			LogArduinoDataOnStreamNoLoops(completeLogFile);

		// double elapsedTime = (timeNow - startTime).total_microseconds() / 1000000.0;

		// una volta ogni tanto stampo gli ultimi dati letti cosi' per gradire
		if((timeNow - lastLogArduinoDataTime).total_microseconds() >= 1000000)
		{
			LogArduinoDataOnStream(cout);
			LogArduinoDataOnStream(samplingLogFile);
			if(readStatus == -1)
			{
			    // cout << "communication error detected - last buffer read: " << footboard->errorReadBuffer << endl;
                cout << "communication error detected - last buffer read: " << footboard->errorLines.back() << endl;
			}

			lastLogArduinoDataTime = timeNow;
			loopCount = 0;
		}

		// shared_ptr<Command> nextCommand = (*commands)[programCounter];
		int nextChan0CommandIdx = -1;
		int nextChan1CommandIdx = -1;
		shared_ptr<Command> nextCommand0 = FindNextCommand(0, nextChan0CommandIdx);
		shared_ptr<Command> nextCommand1 = FindNextCommand(1, nextChan1CommandIdx);

		int cmdCount = 0;
		if(nextCommand0) cmdCount++;
		if(nextCommand1) cmdCount++;

		if (footboard->Accept(nextCommand0))
		{
			cout << "time: " << secondsFromStart << " - Accepted command: " << acceptedCommand->AsString() /*<< " - programCounter: " << programCounter*/
			<< endl << "------ command end ------\n" << endl;

			// attenzione: nel caso dei semafori, aspetto che lo stesso semaforo venga raggiunto da entrambi i canali prima di
			// proseguire

			if(nextCommand0->GetChannel() == -1 && nextCommand0 != nextCommand1)
				goto fuori0; // il goto e' bello e chi non lo usa e' un debole. Dijkstra vaffanculo.

			programCounter[0] = nextChan0CommandIdx;
		}

		fuori0:

		if (footboard->Accept(nextCommand1))
		{
			cout << "time: " << secondsFromStart << " - Accepted command: " << acceptedCommand->AsString() /*<< " - programCounter: " << programCounter*/
			<< endl << "------ command end ------\n" << endl;

			if(nextCommand1->GetChannel() == -1 && nextCommand0 != nextCommand1)
				goto fuori1; // il goto e' bello e chi non lo usa e' un debole. Dijkstra vaffanculo.

			programCounter[1] = nextChan1CommandIdx;
		}

		fuori1:

		if(cmdCount == 0)
		{
			cout << "Exiting main loop" << endl;
			running = false;
		}
	}

	return true;
}

shared_ptr<Command> ProgramController::FindNextCommand(const int& requestedChannel, int& nextCommandIdx)
{
	//cout << "propongo comando " << programCounter << endl;
	//return (*commands)[programCounter];
	nextCommandIdx = programCounter[requestedChannel];
	shared_ptr<Command> toRet;

	if(nextCommandIdx >= commands->size())
		return toRet;

	do
	{
		toRet = (*commands)[nextCommandIdx++];
		if(toRet->GetChannel() != requestedChannel && toRet->GetChannel() != -1)
			toRet.reset();
	}
	while(nextCommandIdx < commands->size() && !toRet);

	// qui possiamo uscire con un comando nullo se non ce ne sono altri del canale richiesto,
	// oppure con un comando per la footboard (channel = -1)
	// oppure con un comando del canale specificato

	// me lo ricordo per la prossima volta
	// OCIO! va fatto fuori e solo quando viene accettato! :)
	/*
	if(toRet)
		programCounter[requestedChannel] = nextCommandIdx;
	*/



	return toRet;
}

bool ProgramController::Init(void)
{
	parser.reset(new ProgramParser(shared_from_this()));
	bool res = parser->ParseProgram("program.ftb");
	if(!res)
	{
		cout << "PARSE ERROR on program.ftb, line " << parser->parsedLineNum << endl;
		return false;
	}
    parser->ParseOptions("options.json");
	footboard.reset(new Footboard(shared_from_this()));
	commands = parser->commands;
	return true;
}

void ProgramController::LogArduinoDataOnStream(ostream& s)
{
	// double elapsedTime = (loopTime - startTime).total_microseconds() / 1000000.0;
    s << "loops: " << fixed << setw(3) << loopCount <<  " time: " << setprecision(3) << setw(7) << secondsFromStart;
	s << " read:";
	s << " ch:" << footboard->states[0].channel << " f:" << footboard->states[0].force << " p:" << footboard->states[0].position
							<< " pid:" << footboard->states[0].pid << " ef:" << footboard->states[0].ef << " epos:" << footboard->states[0].epos;

	s << " ch:" << footboard->states[1].channel << " f:" << footboard->states[1].force << " p:" << footboard->states[1].position
							<< " pid:" << footboard->states[1].pid << " ef:" << footboard->states[1].ef << " epos:" << footboard->states[1].epos;
	s << endl;
}


void ProgramController::LogArduinoDataOnStreamNoLoops(ostream& s)
{
	// double elapsedTime = (loopTime - startTime).total_microseconds() / 1000000.0;
    s << "time: " << fixed << setprecision(3) << setw(7) << secondsFromStart;
	s << " read:";
	s << " ch:" << footboard->states[0].channel << " f:" << footboard->states[0].force << " p:" << footboard->states[0].position
							<< " pid:" << footboard->states[0].pid << " ef:" << footboard->states[0].ef << " epos:" << footboard->states[0].epos;

	s << " ch:" << footboard->states[1].channel << " f:" << footboard->states[1].force << " p:" << footboard->states[1].position
							<< " pid:" << footboard->states[1].pid << " ef:" << footboard->states[1].ef << " epos:" << footboard->states[1].epos;
	s << endl;
}


