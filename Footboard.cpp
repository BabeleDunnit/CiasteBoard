/*
 * ArduinoCommander.cpp
 *
 *  Created on: Mar 13, 2013
 *      Author: babele
 */
#include <stdio.h>
// #include <numeric>
#include <assert.h>
#include <boost/lexical_cast.hpp>
using namespace boost;

#include "Footboard.h"
#include "ProgramController.h"
#include "Commands.h"

Footboard::Footboard(shared_ptr<ProgramController> pc) :
		programController(pc)
{
	// TODO Auto-generated constructor stub
	states.resize(2);
	actuators.resize(2);


	positionCommandPositionConversionFactor =
			programController->parser->options.get<float>("conversion.positionCommand.positionFactor");
	positionCommandForceConversionFactor =
			programController->parser->options.get<float>("conversion.positionCommand.forceFactor");

	// cout << programController->parser->options.get<float>("c1");

}

Footboard::~Footboard()
{
	// TODO Auto-generated destructor stub
}

#define MINFORCEVALUE 0
#define MINPOSITIONVALUE 0
#define MAXFORCEVALUE 4095
#define MAXPOSITIONVALUE 4095

bool Footboard::GetStateFromArduino(void)
{
	// leggo robba
	// char buf[64];
	int len = serial.Read(readBuffer, 64);
	readBuffer[len] = 0;
	// cout << buf << endl;
	// provo a parsare
	// ArduinoState state;
	int channel = -1, force = -1, position = -1;
//	sscanf(readBuffer, "%d %d %d", &state.channel, &state.force,
//			&state.position);
	sscanf(readBuffer, "%d %d %d", &channel, &force, &position);

	// cout << state.channel << state.force << state.position << endl;

	// vediamo se e' valida la lettura
	if (channel != 0 && channel != 1)
		return false;

	if (force < MINFORCEVALUE || force > MAXFORCEVALUE)
		return false;

	if (position < MINPOSITIONVALUE || position > MAXPOSITIONVALUE)
		return false;

	// salvo i dati solo per stamparli ogni tanto, il flusso vero e' quello che entra negli attuatori
	ArduinoState state;
	state.channel = channel;
	state.force = force;
	state.position = position;
	states.push_back(state);

	actuators[channel].AddState(force, position);

//	int sum = 0;
//	assert(states.size() == 128);
//	for(circular_buffer<ArduinoState>::iterator i = states.begin(); i != states.end(); ++i)
//	{
//		sum += i->force;
//	}
//
//	cout << "media1 " << sum / 128.0 << endl;

	return true;
}

/*
 string ArduinoCommander::getLastStateString(const int& channel)
 {
 ArduinoState& ls = states[channel].back();
 return lexical_cast<string>(ls.channel) + " "
 + lexical_cast<string>(ls.force) + " "
 + lexical_cast<string>(ls.position);
 }
 */

bool Footboard::SendForceCommandToArduino(const int& channel, const int& force,
		const int& maxForce)
{
//	string command = lexical_cast<string>(channel) + " " + lexical_cast<string>(force)
//			+ " " + lexical_cast<string>(maxForce) + lexical_cast<string>(13) + lexical_cast<string>(10);
	if (force < -999 || force > 999 || maxForce < -99 || maxForce > 99)
	{
		cout << "warn: force command params out of bounds" << endl;
		return false;
	}

	char buf[64];
	sprintf(buf, "%1d %03d %02d\r\n", channel, force, maxForce);
	// cout << buf << endl;

	int bytesSent = serial.Write(buf, strlen(buf));
	// usleep(100000);
	// assert(bytesSent == command.length());
	// cout << bytesSent << endl;
	return (bytesSent != -1);
}

bool Footboard::SendPositionCommandToArduino(const int& channel_,
		const int& position_, const int& maxForceToGetToPosition_)
{
	int arduinoPosition = position_ * positionCommandPositionConversionFactor;
	int arduinoForce = maxForceToGetToPosition_ * positionCommandForceConversionFactor;

	if (arduinoPosition < 0 || arduinoPosition > 999 || arduinoForce < -99
			|| arduinoForce > 99)
	{
		cout << "warn: position command params out of bounds after conversion: pos:" << arduinoPosition << " force:" << arduinoForce << endl;
		return false;
	}

	char buf[64];
	sprintf(buf, "P %1d %03d %02d\r\n", channel_, arduinoPosition,
			arduinoForce);

	assert(buf[12]==0);
	assert(strlen(buf)==12);

	cout << "sending command to arduino: " << buf << endl;

	int bytesSent = serial.Write(buf, strlen(buf));

	return (bytesSent != -1);

}

bool Footboard::Accept(shared_ptr<Command> c)
{
	int channel = c->GetChannel();
	if (channel != -1)
	{
		return actuators[channel].Accept(c);
	}
//	else
//	{
//		cout << "Footboard accepts command: " << c->AsString() << endl;
//		return true;
//	}

	// per ora abbiamo solo il comando S X
	if (c->IsExpired())
	{
		cout << "Footboard accepts command: " << c->AsString() << endl;
		return true;
	}

	return false;
}

