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
		running(true), programCounter(0), loopCount(0)
{
	loopTime = lastLogArduinoDataTime = boost::posix_time::microsec_clock::local_time();
	samplingLogFile.open("sampling.log");
	completeLogFile.open("complete.log");
}

ProgramController::~ProgramController()
{
	samplingLogFile.close();
	completeLogFile.close();
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
			/*
            cout << "time: " << fixed << setprecision(2) << setw(7) << elapsedTime;
			cout << " info arduino:";
			cout << " ch:" << footboard->states[0].channel << " f:" << footboard->states[0].force << " p:" << footboard->states[0].position
									<< " pid:" << footboard->states[0].pid << " ef:" << footboard->states[0].ef << " epos:" << footboard->states[0].epos;

			cout << " ch:" << footboard->states[1].channel << " f:" << footboard->states[1].force << " p:" << footboard->states[1].position
									<< " pid:" << footboard->states[1].pid << " ef:" << footboard->states[1].ef << " epos:" << footboard->states[1].epos;
			cout << endl;
			*/

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

		shared_ptr<Command> nextCommand = (*commands)[programCounter];
		// cout << "exec command " << nextCommand->AsString() << endl;
		if (footboard->Accept(nextCommand))
		{
			cout << "time: " << secondsFromStart << " - Accepted command: " << nextCommand->AsString()
			<< endl << "------ command end ------\n" << endl;
			++programCounter;
			if (programCounter >= commands->size())
			{
				cout << "Exiting main loop" << endl;
				running = false;
			}
		}
	}

	return true;
}

bool ProgramController::Init(void)
{
	parser.reset(new ProgramParser(shared_from_this()));
	parser->ParseProgram("program.ftb");
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


