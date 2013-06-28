/*
 * Commands.cpp
 *
 *  Created on: Mar 24, 2013
 *      Author: babele
 */

#include "Commands.h"
#include "ProgramController.h"

void Command::OnAccept(void)
{
    acceptTime = microsec_clock::local_time();
    arduinoPositionOnAccept = programController->footboard->states[GetChannel()].position;
}

// bool PositionCommand::Execute(void)
void PositionCommand::OnAccept(void)
{
    Command::OnAccept();
    bool result = programController->footboard->SendPositionCommandToArduino(channel, position, 40);
}

bool PositionCommand::IsExpired(void)
{
//	if(expiredMemory)
//		return true;

	// cout << (microsec_clock::local_time() - acceptTime).total_seconds() << endl;
	if ((microsec_clock::local_time() - acceptTime).total_microseconds() > timeLimit * 1000000)
	{
//		expiredMemory = true;
		return true;
	}

	return false;
}

bool PositionCommand::Kill(void)
{
	// forzo l'expiration settando la durata di questo comando a 0
	timeLimit = 0;
	return true;
}

// bool PositionWithMaxForceCommand::Execute(void)
void PositionWithMaxForceCommand::OnAccept(void)
{
    Command::OnAccept();
	bool result = programController->footboard->SendPositionCommandToArduino(channel, position, optionalMaxForce);
}

// bool ForceCommand::Execute(void)
void ForceCommand::OnAccept(void)
{
    Command::OnAccept();
    // TODO: mettere a posto questo parametro 99
    bool res = programController->footboard->SendForceCommandToArduino(channel, force, 99);
}

bool ForceCommand::IsPositionReached(void)
{
    // io, in quanto comando di forza, ho anche specificato una posizione, che era questa:
    // int positionToReach = position;
    int positionToReach = position * programController->footboard->positionConversionFactor;
    // l'arduino e' arrivato qui:
    int positionReached = programController->footboard->states[GetChannel()].position;
    // le due posizioni sono in due scale diverse. Portiamole nella stessa scala
    // int positionReachedByArduinoInHostScale = positionReachedByArduino / programController->footboard->positionCommandPositionConversionFactor;

    // if(abs(positionToReach - positionReachedByArduinoInHostScale) < 0.1 * positiontoReach)
    int sourceDeltaPos = positionReached - arduinoPositionOnAccept;
    if (sourceDeltaPos >= 0) // devo sorpassare
    {
        if (positionReached >= positionToReach)
        {
//            expiredMemory = true;
            cout << "channel " << GetChannel() << ", delta = "<< sourceDeltaPos << ", positionReached = "
                    << positionReached << ", arduinoPositionOnAccept = " << arduinoPositionOnAccept << ", target pos reached, exiting" << endl;
            return true;
        }
    }
    else if (sourceDeltaPos <= 0)
    {
        if (positionReached <= positionToReach)
        {
//            expiredMemory = true;
            cout << "channel " << GetChannel() << ", delta = "<< sourceDeltaPos << ", positionReached = "
                    << positionReached << ", arduinoPositionOnAccept = " << arduinoPositionOnAccept << ", target pos reached, exiting" << endl;

            return true;
        }
    }

    // son sicuro che il papocchio qui sopra si puo' fare meglio assai...
    // int targetDeltaPos = positionReached - positionToReach;

    return false;
}

bool ForceCommand::IsExpired(void)
{
	if (PositionCommand::IsExpired())
	{
		return true;
	}

#ifdef SPOSTATO
	// io, in quanto comando di forza, ho anche specificato una posizione, che era questa:
	// int positionToReach = position;
	int positionToReach = position * programController->footboard->positionConversionFactor;
	// l'arduino e' arrivato qui:
	int positionReached = programController->footboard->states[GetChannel()].position;
	// le due posizioni sono in due scale diverse. Portiamole nella stessa scala
	// int positionReachedByArduinoInHostScale = positionReachedByArduino / programController->footboard->positionCommandPositionConversionFactor;

	// if(abs(positionToReach - positionReachedByArduinoInHostScale) < 0.1 * positiontoReach)
	int sourceDeltaPos = positionReached - arduinoPositionOnAccept;
	if (sourceDeltaPos >= 0) // devo sorpassare
	{
		if (positionReached >= positionToReach)
		{
			expiredMemory = true;
			cout << "channel " << GetChannel() << ", delta = "<< sourceDeltaPos << ", positionReached = "
					<< positionReached << ", arduinoPositionOnAccept = " << arduinoPositionOnAccept << ", target pos reached, exiting" << endl;
			return true;
		}
	}
	else if (sourceDeltaPos <= 0)
	{
		if (positionReached <= positionToReach)
		{
			expiredMemory = true;
			cout << "channel " << GetChannel() << ", delta = "<< sourceDeltaPos << ", positionReached = "
					<< positionReached << ", arduinoPositionOnAccept = " << arduinoPositionOnAccept << ", target pos reached, exiting" << endl;

			return true;
		}
	}

	// son sicuro che il papocchio qui sopra si puo' fare meglio assai...
	// int targetDeltaPos = positionReached - positionToReach;

	return false;
#endif
	return IsPositionReached();
}

bool ForceWithDeltaCommand::IsExpired(void)
{

    // vale sempre che se siamo arrivati alla posizione giusta
    // ci consideriamo soddisfatti e andiamo alla prossima istruzione
    if(IsPositionReached())
        return true;

    // diversamente dagli altri comandi, pero', qui il timeout quando scade
    // serve a far partire una seconda fase durante la quale alla pedana viene detto
    // di rendere via via sempre piu' facile il compito
    if (PositionCommand::IsExpired())
    {
        // qui dentro continuiamo a entrarci
        if ((microsec_clock::local_time() - lastLoopTime).total_microseconds() > 1000000)
        {
            cout << "sbadabam" << endl;
            lastLoopTime = microsec_clock::local_time();
        }
    }

    return false;
}

// bool ForceWithDeltaCommand::Execute(void)
void ForceWithDeltaCommand::OnAccept(void)
{
    Command::OnAccept();
    cout << "ForceWithDeltaCommand Unimplemented Arduino Command" << endl;
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

    // modifico in modo che si ricordi se un canale ha finito o meno:
    // porto queste due var locali a membri di classe
    /*
	bool isExpired0 = programController->footboard->actuators[0].IsCommandExpired();
	bool isExpired1 = programController->footboard->actuators[1].IsCommandExpired();
	*/

    if(!isExpired0)
        isExpired0 = programController->footboard->actuators[0].IsCommandExpired();

    if(!isExpired0)
        isExpired1 = programController->footboard->actuators[1].IsCommandExpired();

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

