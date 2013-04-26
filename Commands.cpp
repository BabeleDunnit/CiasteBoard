/*
 * Commands.cpp
 *
 *  Created on: Mar 24, 2013
 *      Author: babele
 */

#include "Commands.h"
#include "ProgramController.h"

bool PositionCommand::IsExpired(void)
{
	// cout << (microsec_clock::local_time() - acceptTime).total_seconds() << endl;
	if ((microsec_clock::local_time() - acceptTime).total_microseconds()
			> timeLimit * 1000000)
		return true;

	return false;
}

bool PositionCommand::Kill(void)
{
	// forzo l'expiration settando la durata di questo comando a 0
	timeLimit = 0;
	return true;
}

bool PositionCommand::Execute(void)
{
	return programController->footboard->SendPositionCommandToArduino(channel,
			position, 40);
}

bool PositionWithMaxForceCommand::Execute(void)
{
	return programController->footboard->SendPositionCommandToArduino(channel,
			position, optionalMaxForce);
}


bool SemaphoreCommand::IsExpired(void)
{
	// qui un giorno eventualmente vettorizzeremo tutto quando avremo 20 attuatori. per ora facciamo cosi' alla brutta

	//shared_ptr<Command> command0 = programController->footboard->actuators[0].actualCommand;
	//shared_ptr<Command> command1 = programController->footboard->actuators[0].actualCommand;

	// se un comando e' nullo, vuol dire che siamo all'inizio del programma, l'attuatore non ha ancora iniziato ad eseguire nulla.
	// quindi possiamo dire che un comando nullo e' sicuramente expired.
	//bool isExpired0 = (!programController->footboard->actuators[0].actualCommand) || programController->footboard->actuators[0].actualCommand->IsExpired();
	// bool isExpired1 = (!programController->footboard->actuators[1].actualCommand) || programController->footboard->actuators[1].actualCommand->IsExpired();

	bool isExpired0 =
			programController->footboard->actuators[0].IsCommandExpired();
	bool isExpired1 =
			programController->footboard->actuators[1].IsCommandExpired();

	if (mode == "x")
	{

		return (isExpired0 && isExpired1);
	}
	else if (mode == "0")
	{
		return isExpired0;
	}
	else if (mode == "1")
	{
		return isExpired1;
	}
	else if (mode == "n")
	{
		if (isExpired0)
		{
			programController->footboard->actuators[1].KillCommand();
			return true;
		}
		else if (isExpired1)
		{
			programController->footboard->actuators[0].KillCommand();
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		cout << "command " << AsString() << " not implemented" << endl;
	}

	return true;
}

