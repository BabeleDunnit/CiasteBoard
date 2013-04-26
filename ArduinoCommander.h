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

#include "ArduinoSerial.h"
#include "Actuator.h"

/*
struct ArduinoState
{
	int channel; // 0 o 1
	int force;
	int position;
};
*/

class ArduinoCommander
{
public:
	ArduinoCommander(const ArduinoSerial&);
	virtual ~ArduinoCommander();

	bool GetStateFromArduino(void);
	// string getLastStateString(const int& channel);

	bool SendForceCommandToArduino(const int& channel, const int& force, const int& maxForce);
	bool SendPositionCommandToArduino(const int& channel, const int& position, const int& maxForceToGetToPosition);

	ArduinoSerial arduinoSerial;

	// circular_buffer<ArduinoState> states[2];
	Actuator actuators[2];

	char readBuffer[64];
};

#endif /* ARDUINOCOMMANDER_H_ */
