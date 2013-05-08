/*
 * ArduinoSerial.cpp
 *
 *  Created on: Mar 13, 2013
 *      Author: babele
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <iostream>


#define DEBUG 1

#define BAUDRATE B38400

#include "Serial.h"

Serial::Serial(void)
{
	fd = -1;
}

bool Serial::Open(const string& serialName)
{
	// int fd, n, i;
	// char buf[64] = "temp text";
	struct termios toptions;

	/* open serial port */
	fd = open(serialName.c_str(), O_RDWR | O_NOCTTY);
	if(fd < 0)
	{
		cout << "ERROR opening Arduino port:" << serialName << endl;
		exit(-1);
	}

	printf("fd opened as %i\n", fd);

	/* wait for the Arduino to reboot */
	usleep(2000000);

	/* get current serial port settings */
	tcgetattr(fd, &toptions);
	/* set 9600 baud both ways */
	cfsetispeed(&toptions, BAUDRATE);
	cfsetospeed(&toptions, BAUDRATE);
	/* 8 bits, no parity, no stop bits */
	toptions.c_cflag &= ~PARENB;
	toptions.c_cflag &= ~CSTOPB;
	toptions.c_cflag &= ~CSIZE;
	toptions.c_cflag |= CS8;
	/* Canonical mode */
	toptions.c_lflag |= ICANON;
	/* commit the serial port settings */
	tcsetattr(fd, TCSANOW, &toptions);

	/* Send byte to trigger Arduino to send string back */
	// write(fd, "0", 1);
	/* Receive string from Arduino */
	// n = read(fd, buf, 64);
	/* insert terminating zero in the string */
	// buf[n] = 0;

	/*
	printf("%i bytes read, buffer contains: %s\n", n, buf);

	if (DEBUG)
	{
		printf("Printing individual characters in buf as integers...\n\n");
		for (i = 0; i < n; i++)
		{
			printf("Byte %i:%i, ", i + 1, (int) buf[i]);
		}
		printf("\n");
	}
*/

	// return 0;
	return true;
}

Serial::~Serial()
{
	// TODO Auto-generated destructor stub
	close(fd);
}

int Serial::Write(const void* buf, size_t len)
{
    if(fd == -1)
    {
        // cout << "(no arduino, skipping write)" << endl;
        return 1;
    }

    // memcpy(writeBuffer, buf, len);
    // writeBuffer[len] = 0;

	boost::unique_lock<boost::mutex> scoped_lock(mutex);
	// int written = ::write(fd, writeBuffer, len);
    int written = ::write(fd, buf, len);
	// senza queste sleep si incarta tutto
	// usleep(240000);
	return (written == len);
}

int Serial::Read(void* buf, size_t len)
{
    if(fd == -1)
    {
        // cout << "(no arduino, skipping read)" << endl;

        // sleppo per simulare comunque un timeout
        usleep(20000);
        return 1;
    }

    // readBuffer[0] = 0;

    boost::unique_lock<boost::mutex> scoped_lock(mutex);
	// int nread = ::read(fd, readBuffer, len);
    int nread = ::read(fd, buf, len);

	// senza queste sleep si incarta tutto
	// usleep(2000);

//	readBuffer[len] = 0;
//	memcpy(buf, readBuffer, len);

	return nread;
}
