/*
 * Actuator.h
 *
 *  Created on: Mar 13, 2013
 *      Author: babele
 */

#ifndef ACTUATOR_H_
#define ACTUATOR_H_

// #include <boost/circular_buffer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
#include <boost/shared_ptr.hpp>
using namespace boost;


// #include "ArduinoSerial.h"
// #include "Actuator.h"

//struct ArduinoState
//{
//	// int channel; // 0 o 1
//	int force;
//	int position;
//};

#include "PID.h"
// #include "ProgramParser.h"


class Command;

class Actuator
{
public:
	Actuator();
	virtual ~Actuator();

	bool Accept(shared_ptr<Command> c);

	bool AddState(const int& force, const int& position);

	bool IsCommandExpired(void);
	bool KillCommand(void); //  { if(actualCommand) actualCommand->Kill(); }

	PID positionPID;

	int previousPIDValue;
	int PIDValue;

private:
	// il comando puo' essere nullo, e voglio astrarre che un comando nullo e' da considerarsi "expired"
	// nel senso che possiamo immediatamente caricare il comando successivo. Quindi lo metto privato
	// e sposto i check nell'interfaccia dell'actuator
	shared_ptr<Command> actualCommand;

};

#endif /* ACTUATOR_H_ */
