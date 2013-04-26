/*
 * ArduinoCommander.h
 *
 *  Created on: Mar 13, 2013
 *      Author: babele
 */

#ifndef ARDUINOCOMMANDER_H_
#define ARDUINOCOMMANDER_H_

// #include <vector>
//using namespace std;
#include <boost/circular_buffer.hpp>

#include "Serial.h"
#include "Actuator.h"
// #include "ProgramController.h"


struct ArduinoState
{
	int channel; // 0 o 1
	int force;
	int position;
};


class Command;
class ProgramController;

class Footboard
{
public:
	Footboard(shared_ptr<ProgramController> pc);
	virtual ~Footboard();

	bool GetStateFromArduino(void);
	// string getLastStateString(const int& channel);

	bool SendForceCommandToArduino(const int& channel, const int& force, const int& maxForce);
	bool SendPositionCommandToArduino(const int& channel, const int& position, const int& maxForceToGetToPosition);

	bool Accept(shared_ptr<Command>);

	Serial serial;

	// only for debug purposes
	circular_buffer<ArduinoState> states;

	vector<Actuator> actuators;

	char readBuffer[64];

	// letti da json
    float positionCommandPositionConversionFactor;
    float positionCommandForceConversionFactor;

    shared_ptr<ProgramController> programController;
};

#endif /* ARDUINOCOMMANDER_H_ */
