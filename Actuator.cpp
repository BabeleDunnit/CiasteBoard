/*
 * Actuator.cpp
 *
 *  Created on: Mar 13, 2013
 *      Author: babele
 */

#include "Actuator.h"
#include "ProgramParser.h"
#include "Commands.h"

#include <iostream>
using namespace std;

Actuator::Actuator()
{
}

Actuator::~Actuator()
{
}

const float THRESHOLD = 0.05;

bool Actuator::Accept(shared_ptr<Command> c)
{
	// per vedere se posso accettare un nuovo comando devo vedere se ho raggiunto
	// i parametri specificati dal parametro attuale o se e' scaduto il timeout
	// bool targetReached;

	if (!actualCommand || actualCommand->IsExpired())
// 		|| (microsec_clock::local_time() - actualCommand->acceptTime).total_seconds() > 5)
	{
		cout << "\n----- command start -----\n" << "Actuator is accepting command: "
				<< c->AsString() << " (line: " << c->lineNumber << ")" << endl;
		actualCommand = c;
		// actualCommand->acceptTime = microsec_clock::local_time();
		actualCommand->OnAccept();
		// actualCommand->Execute();
		return true;
	}

	return false;
}

/*
bool Actuator::AddState(const int& force, const int& position)
{
	PIDValue = positionPID.AddValue(position);
	bool shooting = false;
	if (abs(PIDValue - previousPIDValue) > PIDValue * THRESHOLD)
	{
		// cout << "sparo" << endl;
		shooting = true;
	}

	previousPIDValue = PIDValue;
	return shooting;
}
*/

bool Actuator::IsCommandExpired(void)
{
	return !actualCommand || actualCommand->IsExpired();
}

bool Actuator::KillCommand(void)
{
	if(actualCommand)
		return actualCommand->Kill();

	return true;
}


