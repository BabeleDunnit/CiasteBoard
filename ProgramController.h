/*
 * ProgramController.h
 *
 *  Created on: Mar 14, 2013
 *      Author: brancotti
 */

#ifndef PROGRAMCONTROLLER_H_
#define PROGRAMCONTROLLER_H_


#include "ProgramParser.h"
#include "Footboard.h"

#include <boost/date_time/posix_time/posix_time.hpp>
// using namespace boost::date_time;
using namespace boost::posix_time;


class ProgramController
{
public:
    ProgramController();
    virtual ~ProgramController();

    bool Run(void);
    void Stop(void) { running = false; }
    bool running;

    ProgramParser parser;
    Footboard footboard;

    // bool ExecuteNextCommand(void);
    vector<shared_ptr<Command> >& commands;
    int programCounter;

    ptime loopTime;
    time_duration deltaT;


};

#endif /* PROGRAMCONTROLLER_H_ */
