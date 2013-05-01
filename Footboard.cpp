/*
 * ArduinoCommander.cpp
 *
 *  Created on: Mar 13, 2013
 *      Author: babele
 */
#include <stdio.h>
#include <assert.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
using namespace boost;

#include "Footboard.h"
#include "ProgramController.h"
#include "Commands.h"

Footboard::Footboard(shared_ptr<ProgramController> pc) :
		programController(pc)
{
	//states.resize(3);
	actuators.resize(2);

	positionConversionFactor = programController->parser->options.get<float>("conversion.positionFactor");
	forceConversionFactor = programController->parser->options.get<float>("conversion.forceFactor");

	memset(readBuffer, 0, sizeof(readBuffer));
	memset(states, 0, sizeof(ArduinoState) * 2);
}

Footboard::~Footboard()
{
}

#define MINFORCEVALUE 0
#define MINPOSITIONVALUE 0
#define MAXFORCEVALUE 4095
#define MAXPOSITIONVALUE 4095

bool Footboard::GetStateFromArduino(void)
{
	int len = serial.Read(readBuffer, 64);
	readBuffer[len] = 0;

	int channel = -1, force = -1, position = -1, pid = -1, ef = -1, epos = -1;
	// sscanf(readBuffer, "%d %d %d", &channel, &force, &position);
	sscanf(readBuffer, "%d %d %d %d %d %d", &channel, &force, &position, &pid, &ef, &epos);

	// cout << state.channel << state.force << state.position << endl;

	// vediamo se e' valida la lettura
	if (channel != 0 && channel != 1)
		return false;

	if (force < MINFORCEVALUE || force > MAXFORCEVALUE)
		return false;

	if (position < MINPOSITIONVALUE || position > MAXPOSITIONVALUE)
		return false;

	// salvo i dati solo per stamparli ogni tanto, il flusso vero e' quello che entra negli attuatori
	/*
	 ArduinoState state;
	 state.channel = channel;
	 state.force = force;
	 state.position = position;
	 state.pid = pid;
	 state.ef = ef;
	 state.epos = epos;
	 states.push_back(state);
	 */

	states[channel].channel = channel;
	states[channel].force = force;
	states[channel].position = position;
	states[channel].pid = pid;
	states[channel].ef = ef;
	states[channel].epos = epos;

	//actuators[channel].AddState(force, position);

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
char arduinoCommandBuffer[64];

bool Footboard::SendForceCommandToArduino(const int& channel, const int& force, const int& maxForce)
{
	if (force < -999 || force > 999 || maxForce < -99 || maxForce > 99)
	{
		cout << "warn: force command params out of bounds" << endl;
		return false;
	}

	// char buf[64];
	sprintf(arduinoCommandBuffer, "%1d %03d %02d\r\n", channel, force, maxForce);

	string command(arduinoCommandBuffer);
	trim_right(command);
	cout << "Footboard sending string to arduino: '" << command << "\\r\\n'" << endl;

	assert(arduinoCommandBuffer[10]==0);
	assert(strlen(arduinoCommandBuffer)==10);

	int bytesSent = serial.Write(arduinoCommandBuffer, strlen(arduinoCommandBuffer));
	// usleep(100000);
	// assert(bytesSent == command.length());
	// cout << bytesSent << endl;
	return (bytesSent != -1);
}

bool Footboard::SendPositionCommandToArduino(const int& channel_, const int& position_,
		const int& maxForceToGetToPosition_)
{
	int arduinoPosition = position_ * positionConversionFactor;
	int arduinoForce = maxForceToGetToPosition_ * forceConversionFactor;

	if (arduinoPosition < 0 || arduinoPosition > 999 || arduinoForce < -99 || arduinoForce > 99)
	{
		cout << "warn: position command params out of bounds after conversion: pos:" << arduinoPosition << " force:"
				<< arduinoForce << endl;
		cout << "will NOT send command to Arduino" << endl;
		return false;
	}

	sprintf(arduinoCommandBuffer, "P %1d %03d %02d\r\n", channel_, arduinoPosition, arduinoForce);

	// riformatto un pelo, evito eoln multipli
	string command(arduinoCommandBuffer);
	trim_right(command);
	cout << "Footboard sending string to arduino: '" << command << "\\r\\n'" << endl;

	assert(arduinoCommandBuffer[12]==0);
	assert(strlen(arduinoCommandBuffer)==12);
	int bytesSent = serial.Write(arduinoCommandBuffer, strlen(arduinoCommandBuffer));

	return (bytesSent != -1);

}

bool Footboard::Accept(shared_ptr<Command> c)
{
	int channel = c->GetChannel();
	if (channel != -1)
	{
		return actuators[channel].Accept(c);
	}

	// se arriviamo qui e' un comando per la footboard
	assert(channel == -1);

	// questo test viene fatto perche' i comandi per la footboard, ovvero i vari semafori,
	// sono considerati expired in funzione della expiration dei comandi che stanno girando
	// sui diversi canali. In verita' trae un po' in inganno, perche' i semafori non "fanno" nulla,
	// ma semplicemente bloccano fino a quando non sono expired. Questo implica che CI VUOLE SEMPRE
	// un S X alla fine del programma
	if (c->IsExpired())
	{
		cout << "\n----- command start -----\n" << "Footboard accepts command: " << c->AsString()
		<< " (line: " << c->lineNumber << ")" << endl;
		c->OnAccept();
		return true;
	}

	return false;
}

