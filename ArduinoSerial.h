/*
 * ArduinoSerial.h
 *
 *  Created on: Mar 13, 2013
 *      Author: babele
 */

#ifndef ARDUINOSERIAL_H_
#define ARDUINOSERIAL_H_

#include <string>
using namespace std;

class ArduinoSerial
{
public:
	ArduinoSerial(const string& serialName);
	virtual ~ArduinoSerial();

	int Write(const void* buf, size_t len);
	int Read(void* buf, size_t len);

	int fd;
};

#endif /* ARDUINOSERIAL_H_ */
