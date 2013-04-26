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

#include <boost/enable_shared_from_this.hpp>

class ProgramController : public enable_shared_from_this<ProgramController>
{
public:
    ProgramController();
    virtual ~ProgramController();

    bool Run(void);
    void Stop(void) { running = false; }
    bool running;

    bool Init(void);

    shared_ptr<ProgramParser> parser;
    shared_ptr<Footboard> footboard;

    // bool ExecuteNextCommand(void);
    shared_ptr<vector<shared_ptr<Command> > > commands;
    int programCounter;

    ptime loopTime;
    time_duration deltaT;
    ptime lastInfoTime;
    ptime startTime;

    // float positionCommandPositionConversionFactor;
    // float positionCommandForceConversionFactor;


};

#endif /* PROGRAMCONTROLLER_H_ */
