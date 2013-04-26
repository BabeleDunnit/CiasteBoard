/*
 * ProgramController.cpp
 *
 *  Created on: Mar 14, 2013
 *      Author: brancotti
 */

#include "ProgramController.h"

//#include <boost/date_time/posix_time/posix_time.hpp>
//#include <boost/thread.hpp>

ProgramController::ProgramController() :
		running(true), commands(parser.commands), programCounter(0)
{
	// TODO Auto-generated constructor stub
	loopTime = boost::posix_time::microsec_clock::local_time();
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

	while (running)
	{

		ptime timeNow = boost::posix_time::microsec_clock::local_time();
		deltaT = timeNow - loopTime;
		loopTime = timeNow;

		// cout << deltaT.total_microseconds() << endl;

		// leggo lo stato della pedana
		footboard.GetStateFromArduino();

		shared_ptr<Command> nextCommand = commands[programCounter];
		if (footboard.Accept(nextCommand))
		{
			cout << "Accepted command: " << nextCommand->AsString() << endl;
			++programCounter;
			if (programCounter >= commands.size())
			{
				cout << "Exiting main loop" << endl;
				running = false;
			}
		}

	}

	return true;
}

