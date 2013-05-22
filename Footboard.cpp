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

	// memset(readBuffer, 0, sizeof(readBuffer));
	//memset(states, 0, sizeof(ArduinoState) * 2);
	states.resize(2);

}

Footboard::~Footboard()
{
}

#ifdef VECCHIO_ERRORIDILETTURA
bool Footboard::GetStateFromArduino(void)
{
	int len = serial.Read(readBuffer, 64);
	readBuffer[len] = 0;

	if(len <= 1)
	{
	    // timeout
        strcpy(errorReadBuffer, "NOT a buffer dump! :) arduino->host serial timeout");
	    return false;
	}

	if(readBuffer[0] != '0' && readBuffer[0] != '1')
	{
	    cout << "message from arduino: <" << readBuffer << ">" << endl;
	    return true;
	}

    assert(readBuffer[0] == '0' || readBuffer[0] == '1' );

	int channel = -1, force = -1, position = -1, pid = -1, ef = -1, epos = -1;
	// sscanf(readBuffer, "%d %d %d", &channel, &force, &position);
	sscanf(readBuffer, "%d %d %d %d %d %d", &channel, &force, &position, &pid, &ef, &epos);

	// cout << state.channel << state.force << state.position << endl;

	// vediamo se e' valida la lettura
	/*
	if (channel != 0 && channel != 1)
		return false;

	if (force < MINFORCEVALUE || force > MAXFORCEVALUE)
		return false;

	if (position < MINPOSITIONVALUE || position > MAXPOSITIONVALUE)
		return false;
		*/

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

	// e' comunque successo qualche casino nella scanf? possibile??
	if(channel != 0 && channel != 1)
	{
	    // cout << "Error receiving Arduino data - abort read" << endl;

	    strncpy(errorReadBuffer, readBuffer, 64);
	    errorReadBuffer[63] = 0;

	    states[0].channel = -1;
        states[0].force = -1;
        states[0].position = -1;
        states[0].pid = -1;
        states[0].ef = -1;
        states[0].epos = -1;

        states[1].channel = -1;
        states[1].force = -1;
        states[1].position = -1;
        states[1].pid = -1;
        states[1].ef = -1;
        states[1].epos = -1;

        return false;
	}

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
#endif


int Footboard::GetStateFromArduino(void)
{
    string line = serial.ReadLine();

    if(line.empty())
        return 0;

    if(line[0] != '0' && line[0] != '1')
    {
        cout << "--- Message from arduino: <" << line << ">" << endl;
        return 2;
    }

    assert(line[0] == '0' || line[0] == '1' );

    int channel = -1, force = -1, position = -1, pid = -1, ef = -1, epos = -1;
    // sscanf(readBuffer, "%d %d %d", &channel, &force, &position);
    sscanf(line.c_str(), "%d %d %d %d %d %d", &channel, &force, &position, &pid, &ef, &epos);

    // e' comunque successo qualche casino nella scanf? possibile??
    if(channel != 0 && channel != 1)
    {
        // cout << "Error receiving Arduino data - abort read" << endl;

        //strncpy(errorReadBuffer, readBuffer, 64);
        errorLines.push_back(line);
        // errorReadBuffer[63] = 0;

        states[0].channel = -1;
        states[0].force = -1;
        states[0].position = -1;
        states[0].pid = -1;
        states[0].ef = -1;
        states[0].epos = -1;

        states[1].channel = -1;
        states[1].force = -1;
        states[1].position = -1;
        states[1].pid = -1;
        states[1].ef = -1;
        states[1].epos = -1;

        return -1;
    }

    states[channel].channel = channel;
    states[channel].force = force;
    states[channel].position = position;
    states[channel].pid = pid;
    states[channel].ef = ef;
    states[channel].epos = epos;

    return 1;
}



char arduinoCommandBuffer[64];

bool Footboard::SendForceCommandToArduino(const int& channel, const int& force, const int& maxForce)
{
	int arduinoForce = force * forceConversionFactor;
	int arduinoMaxForce = maxForce * forceConversionFactor;

	if (arduinoForce < -999 || arduinoForce > 999 || arduinoMaxForce < -99 || arduinoMaxForce > 99)
	{
		cout << "warn: force command params out of bounds" << endl;
		cout << "will NOT send command to Arduino" << endl;
		return false;
	}

	// char buf[64];
	sprintf(arduinoCommandBuffer, "%1d %03d %02d\r\n", channel, force, maxForce);

	string command(arduinoCommandBuffer);
	trim_right(command);
	cout << "Footboard sending string to arduino: '" << command << "\\r\\n'" << endl;
	programController->samplingLogFile << command << "\\r\\n" << endl;
	programController->completeLogFile << command << "\\r\\n" << endl;

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
	programController->samplingLogFile << command << "\\r\\n" << endl;
	programController->completeLogFile << command << "\\r\\n" << endl;

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

