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

#include "ArduinoCommander.h"

ArduinoCommander::ArduinoCommander(const ArduinoSerial& as) :
		arduinoSerial(as)
{
	// TODO Auto-generated constructor stub
//	states[0].resize(50);
//	states[1].resize(50);
}

ArduinoCommander::~ArduinoCommander()
{
	// TODO Auto-generated destructor stub
}

#define MINFORCEVALUE 0
#define MINPOSITIONVALUE 0
#define MAXFORCEVALUE 4095
#define MAXPOSITIONVALUE 4095

bool ArduinoCommander::GetStateFromArduino(void)
{
	// leggo robba
	// char buf[64];
	int len = arduinoSerial.Read(readBuffer, 64);
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

	// states[state.channel].push_back(state);
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

bool ArduinoCommander::SendForceCommandToArduino(const int& channel,
		const int& force, const int& maxForce)
{
//	string command = lexical_cast<string>(channel) + " " + lexical_cast<string>(force)
//			+ " " + lexical_cast<string>(maxForce) + lexical_cast<string>(13) + lexical_cast<string>(10);
	if(force < -999 || force > 999 ||
			maxForce < -99 || maxForce > 99 )
	{
		cout << "warn: force command params out of bounds" << endl;
		return false;
	}

	char buf[64];
	sprintf(buf, "%1d %03d %02d\r\n", channel, force, maxForce);
	// cout << buf << endl;

	int bytesSent = arduinoSerial.Write(buf, strlen(buf));
	// usleep(100000);
	// assert(bytesSent == command.length());
	// cout << bytesSent << endl;
	return (bytesSent != -1);
}

bool ArduinoCommander::SendPositionCommandToArduino(const int& channel,
		const int& position, const int& maxForceToGetToPosition)
{

	if(position < 0 || position > 999 ||
			maxForceToGetToPosition < -99 || maxForceToGetToPosition > 99 )
	{
		cout << "warn: position command params out of bounds" << endl;
		return false;
	}


//	bool shooted = actuators[channel].positionPID.setTargetValue(position);
//	if(shooted)
//
//	    fare oggetto alto che parsa e fa girare il prog e parla con pid, pid chiama questo


	/*
	 string command = "P" + lexical_cast<string>(channel) + " " + lexical_cast<string>(position)
	 + " " + lexical_cast<string>(maxForceToGetToPosition) + lexical_cast<string>(13) + lexical_cast<string>(10);
	 // cout << "sending " << command << endl;
	 int bytesSent = arduinoSerial.write(command.c_str(), command.length());
	 //usleep(100000);
	 return (bytesSent != -1);
	 */

	char buf[64];
	sprintf(buf, "P %1d %03d %02d\r\n", channel, position,
			maxForceToGetToPosition);
	// cout << buf << endl;

	int bytesSent = arduinoSerial.Write(buf, strlen(buf));
	// usleep(100000);
	// assert(bytesSent == command.length());
	// cout << bytesSent << endl;
	return (bytesSent != -1);

}
