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

//#include <boost/date_time/posix_time/posix_time.hpp>
//#include <boost/thread.hpp>

ProgramController::ProgramController() :
		running(true), programCounter(0)
{
	loopTime = lastInfoTime = boost::posix_time::microsec_clock::local_time();
}

ProgramController::~ProgramController()
{
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
		deltaT = timeNow - loopTime;
		loopTime = timeNow;

		// leggo lo stato della pedana
		footboard->GetStateFromArduino();

		double elapsedTime = (timeNow - startTime).total_microseconds() / 1000000.0;

		// una volta ogni tanto stampo gli ultimi dati letti cosi' per gradire
		if((timeNow - lastInfoTime).total_microseconds() >= 1000000)
		{
			// cout << "time: " << (timeNow - startTime).total_microseconds() / 1000000.0;
            cout << "time: " << fixed << setprecision(2) << setw(7) << elapsedTime;
			cout << " info arduino:";

			/*
			// qui faccio un trusco per stampare sempre prima il canale 0 e poi 1
			int channelsPrinted = 0;
			for(circular_buffer<ArduinoState>::iterator i = footboard->states.begin(); i != footboard->states.end() && channelsPrinted < 2 ; ++i)
			{
				if(i->channel == 1) continue;
				cout << " ch:" << i->channel << " f:" << i->force << " p:" << i->position
						<< " pid:" << i->pid << " ef:" << i->ef << " epos:" << i->epos;
				++channelsPrinted;
			}
			*/
			cout << " ch:" << footboard->states[0].channel << " f:" << footboard->states[0].force << " p:" << footboard->states[0].position
									<< " pid:" << footboard->states[0].pid << " ef:" << footboard->states[0].ef << " epos:" << footboard->states[0].epos;

			cout << " ch:" << footboard->states[1].channel << " f:" << footboard->states[1].force << " p:" << footboard->states[1].position
									<< " pid:" << footboard->states[1].pid << " ef:" << footboard->states[1].ef << " epos:" << footboard->states[1].epos;


			cout << endl;
			lastInfoTime = timeNow;
		}

		shared_ptr<Command> nextCommand = (*commands)[programCounter];
		// cout << "exec command " << nextCommand->AsString() << endl;
		if (footboard->Accept(nextCommand))
		{
			cout << "time: " << elapsedTime << " - Executed command: " << nextCommand->AsString()
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


