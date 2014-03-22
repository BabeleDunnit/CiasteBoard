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

//#define BAUDRATE B38400
#define BAUDRATE B57600

#include "Serial.h"

Serial::Serial(void)
{
	fd = -1;
}

bool Serial::Open(const string& serialName)
{

	struct termios toptions;
	struct termios opts_read;
	
	/* open serial port */								// O_RDONLY read only
	fd = open(serialName.c_str(), O_RDWR | O_NOCTTY | O_LARGEFILE |O_NONBLOCK );  	//RDWR per leggi-e-scrivi
	 	// fd = open(serialName.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	 	// fd = open(serialName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	 	/*	long flag = fcntl(fd, F_GETFL, 0 );
	 	fcntl(fd,F_SETFL,O_NONBLOCK);*/
	if(fd < 0) 	{
		cout << "ERROR opening Connection port:" << serialName << endl;
		exit(-1);
	}

	printf("fd opened as %i\n", fd);

	/* wait for the Arduino to reboot here?*/

	if (tcflush( fd, TCIOFLUSH) != 0)  perror("tcflush error");
	
	toptions.c_iflag = 0;		// CS8 8 bit
	toptions.c_oflag = 0;		// nothing set for output
	toptions.c_oflag &= ~OPOST;	//ecept for raw output
	toptions.c_cflag &= ~PARENB;
	toptions.c_cflag &= ~CSTOPB;
	toptions.c_cflag &= ~CSIZE;
 	toptions.c_cflag = CS8| CREAD| CLOCAL;  //CLOCAL ignore status lines //CREAD enable receiver
	//toptions.c_lflag |= ICANON;    		//under investigation
	toptions.c_lflag &= ~( ICANON |ECHO|ISIG); // Raw input no echo
	
	cfsetispeed(&toptions, BAUDRATE);
	cfsetospeed(&toptions, BAUDRATE);

	toptions.c_cc[VMIN]  = 8; // assuming 6 numbers each 1 byte plus CR LF worst case
	toptions.c_cc[VTIME] = 1; //  might be too big 1=0.1 seconds!	
	
   	if (tcsetattr(fd, TCSANOW, &toptions) <0) perror("tcsetaddr failed");
	usleep(100000);
	if (tcflush( fd, TCIOFLUSH) != 0) 	 perror("tcflush error");
	
	// no flow control
	//toptions.c_cflag &= ~CRTSCTS;  //flow ctl

    //toptions.c_cflag &= ~HUPCL; // disable hang-up-on-close to avoid reset
    //toptions.c_cflag |= CREAD | CLOCAL ;  // turn on READ & ignore ctrl lines
    //toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl
    // No line processing:
    // echo off, echo newline off, canonical mode off, 
    // extended input processing off, signal chars off
    //toptions.c_lflag &= ~(ICANON | ECHO | ECHOE |  ISIG); // make raw
    //toptions.c_oflag &= ~OPOST; // make raw
	/* Canonical mode */
	//toptions.c_lflag |= ICANON;

    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    //toptions.c_cc[VMIN]  = 14; // assuming 6 numbers each 1 byte plus CR LF
    //toptions.c_cc[VTIME] = 1; // i might be too big 1=0.1 seconds!
    //if (tcflush( fd, TCIOFLUSH) != 0)  perror("tcflush error");

    /* commit the serial port settings */
	//tcsetattr(fd, TCSANOW, &toptions);

	//if (tcflush( fd, TCIOFLUSH) != 0)  perror("tcflush error");
	/* Send byte to trigger Arduino to send string back */
	// write(fd, "0", 1);
	/* Receive string from Arduino */
	// n = read(fd, buf, 64);
	/* insert terminating zero in the string */
	// buf[n] = 0;

	/*
	printf("%i bytes read, buffer contains: %s\n", n, buf);

	if (DEBUG)	{
		printf("Printing individual characters in buf as integers...\n\n");
		for (i = 0; i < n; i++)	{
			printf("Byte %i:%i, ", i + 1, (int) buf[i]);
		}
		printf("\n");
	}
*/

//  is it set as we asked for?
//	struct termios toptionVerif;
//	tcgetattr(fd, &toptionVerif);
//	if ( StructComparator( toptions, toptionVerif) == 0)
//		cout << "Damn! i told you attributes where messed up!" << endl;
//  assert(memcmp(&toptions, &toptionVerif, sizeof(toptions)) == 0);

//	if (tcdrain( fd) != 0)
//		 perror("tcdrain error");

	return true;
}

Serial::~Serial()
{
	// TODO Auto-generated destructor stub
	if (tcflush( fd, TCIOFLUSH) != 0)
		 perror("tcflush error");
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
	// boost::unique_lock<boost::mutex> scoped_lock(m);
	// int written = ::write(fd, writeBuffer, len);
    int written = ::write(fd, buf, len);
	// senza queste sleep si incarta tutto
	// usleep(240000);
    return (written == len);
}


string Serial::ReadLine(void)
{
	// i assume this is a blocking read, i.e. it waits until good data available
    if(fd == -1)
    {
        // cout << "(no arduino, skipping read)" << endl;
        // sleppo per simulare comunque un timeout
        usleep(20000);
        return "";   // strcpy(readBuffer, "Bad tty plutopaperino\r\nilrestod");
    }

    int nread = ::read(fd, readBuffer, 80);
    string newRead;
    if(nread == -1)   {
    	// cout << "Reading error: " << strerror( errno ) << endl;  // something went wrong....
    	return "";
    } else if (nread == 0) {
    	return "";
    } else if (nread > 0 ) // luca -se ne becco 0  salto
    {
        // lo sparo in newRead
        newRead = string(readBuffer, nread);
        readString = readString + newRead;
    }

    // c'e' un cippa di terminatore? cerco \r\n, ovvero 13 10
    std::string::size_type found = readString.find("\r\n");
    //    cout <<  found << readString.size() << endl;

    string toReturn;
    if (found != std::string::npos)  // i have a good line available
    {
        // ho trovato il terminatore. Splitto il bufferazzo in due, mi tengo la coda pronta per beccarsi la prossima
        // lettura in coda e restituisco la testa. Nota che elimino da entrambe le stringhe il \r\n.
        toReturn = readString.substr(0, found );
        readString = readString.substr(found+2);
    }

    if(readString.length() > 2048)
    {
    	cout << "buffer too long, resetting.." << endl;
    	toReturn = "buffer too long, resetting..";
    	readString.clear();
    }

    return toReturn;
}


std::string string_to_hex(const std::string& input)
{
    static const char* const lut = "0123456789ABCDEF";
    static const char* spc = " ";
    size_t len = input.length();

    std::string output;
    output.reserve(2 * len + len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
        output.push_back(spc[0]);
    }
    return output;
}


/*
bool StructComparator(const termios& lhs, const termios& rhs)
{
// beware of this michevious function! If structs contains pointers (to pointers) it will fuck you!
	return lhs.to_string() == rhs.to_string();
}

float MapArduPress(int channel, int SampleVal)
{
// accessing static conversion table we can convert it
	float result;
	result = ConversionTable[channel][MAXOUT_POS] * 
	         (SampleVal - ConversionTable[channel][ZERO_POS])/ ConversionTable[channel][RANGE_POS];
	return result;	
}


int CalcWeight( int Pres0, int Pos0, int Pres1, int Pos1)
{
// make sure to call this function several times, and average non-zero values
// returns instant force
	int ret = 0;
	if ( Pos0 > ConversionTable[0][ZERO_POS] && Pos1 > ConversionTable[1][ZERO_POS] )
		ret = (Pres0 + Pres1)/2;
	return ret;
}
*/

#ifdef NO_LEGGEALLACACCHIO
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
#endif



