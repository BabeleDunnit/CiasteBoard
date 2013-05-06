/*
 * ArduinoCommander.h
 *
 *  Created on: Mar 13, 2013
 *      Author: babele
 */

#ifndef ARDUINOCOMMANDER_H_
#define ARDUINOCOMMANDER_H_

#include <boost/circular_buffer.hpp>

#include "Serial.h"
#include "Actuator.h"

struct ArduinoState
{
	int channel; // 0 o 1
	int force;
	int position;
	int pid;
	int ef;
	int epos;
};

class Command;
class ProgramController;

class Footboard
{
public:
	Footboard(shared_ptr<ProgramController> pc);
	virtual ~Footboard();

	bool GetStateFromArduino(void);

	bool SendForceCommandToArduino(const int& channel, const int& force, const int& maxForce);
	bool SendPositionCommandToArduino(const int& channel, const int& position, const int& maxForceToGetToPosition);

	bool Accept(shared_ptr<Command>);

	Serial serial;

	// only for debug purposes
	// circular_buffer<ArduinoState> states;
	// li tengo gia' separati per canale
	// ArduinoState states[2];
    vector<ArduinoState> states;

	vector<Actuator> actuators;

	char readBuffer[64];
    char errorReadBuffer[64];

	// letti da json
	// int arduinoPosition = position_ * positionCommandPositionConversionFactor;
	// int arduinoForce = maxForceToGetToPosition_ * positionCommandForceConversionFactor;
    float positionConversionFactor;
    float forceConversionFactor;

    shared_ptr<ProgramController> programController;
};

#endif /* ARDUINOCOMMANDER_H_ */
