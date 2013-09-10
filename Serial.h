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

#include <boost/thread/mutex.hpp>
using namespace boost;

class Serial
{
public:
	Serial(void);
	virtual ~Serial();

	bool Open(const string& serialName);
	void Close();
	int Write(const void* buf, size_t len);

	// int Read(void* buf, size_t len);
    string ReadLine(void);

	int fd;
	// boost::mutex mutex;

	char readBuffer[4096];
	string readString;
//    char writeBuffer[256];
};

#endif /* ARDUINOSERIAL_H_ */
