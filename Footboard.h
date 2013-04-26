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

/*
struct ArduinoState
{
	int channel; // 0 o 1
	int force;
	int position;
};
*/

class Command;

class Footboard
{
public:
	Footboard(void);
	virtual ~Footboard();

	bool GetStateFromArduino(void);
	// string getLastStateString(const int& channel);

	bool SendForceCommandToArduino(const int& channel, const int& force, const int& maxForce);
	bool SendPositionCommandToArduino(const int& channel, const int& position, const int& maxForceToGetToPosition);

	bool Accept(shared_ptr<Command>);

	Serial serial;

	// circular_buffer<ArduinoState> states[2];
	// Actuator actuators[2];
	vector<Actuator> actuators;

	char readBuffer[64];
};

#endif /* ARDUINOCOMMANDER_H_ */
