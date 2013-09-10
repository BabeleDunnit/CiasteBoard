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
#include "Sound.h"

#include <boost/date_time/posix_time/posix_time.hpp>
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

    // time all'inizio del loop
    ptime loopTime;

    // durata dell'ultimo loop
    time_duration deltaT;

    // ultima volta che abbiamo stampato le info arduino
    ptime lastLogArduinoDataTime;

    // inizio esecuzione programma
    ptime startTime;

    // secondi passati dall'inizio esecuzione
    double secondsFromStart;

    // quanti loop ha fatto dall'ultima stampa?
    int loopCount;

    ofstream samplingLogFile;
    ofstream completeLogFile;
    void LogArduinoDataOnStream(ostream&);
    void LogArduinoDataOnStreamNoLoops(ostream&);

    shared_ptr<Sound> claxon;
};

#endif /* PROGRAMCONTROLLER_H_ */
