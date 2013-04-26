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

	// TODO Auto-generated constructor stub
	loopTime = lastInfoTime = boost::posix_time::microsec_clock::local_time();
}

ProgramController::~ProgramController()
{
	// TODO Auto-generated destructor stub
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

	startTime = boost::posix_time::microsec_clock::local_time();

	while (running)
	{

		ptime timeNow = boost::posix_time::microsec_clock::local_time();
		deltaT = timeNow - loopTime;
		loopTime = timeNow;

		// cout << deltaT.total_microseconds() << endl;

		// leggo lo stato della pedana
		footboard->GetStateFromArduino();

		// una volta ogni tanto stampo gli ultimi dati letti cosi' per gradire
		if((timeNow - lastInfoTime).total_microseconds() >= 1000000)
		{
			cout << "time: " << (timeNow - startTime).total_microseconds() / 1000000.0;
			cout << " info arduino:";
			for(circular_buffer<ArduinoState>::iterator i = footboard->states.begin(); i != footboard->states.end(); ++i)
			{
				cout << " ch:" << i->channel << " f:" << i->force << " p:" << i->position;
			}
			cout << endl;
			lastInfoTime = timeNow;
		}

		shared_ptr<Command> nextCommand = (*commands)[programCounter];
		// cout << "exec command " << nextCommand->AsString() << endl;
		if (footboard->Accept(nextCommand))
		{
			cout << "Accepted command: " << nextCommand->AsString() << endl;
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


