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

#define TOT_CH 2

Footboard::Footboard(shared_ptr<ProgramController> pc) :
		programController(pc)
{
	//states.resize(3);
	actuators.resize(2);

	positionConversionFactor = programController->parser->options.get<float>("conversion.positionFactor");
	forceConversionFactor = programController->parser->options.get<float>("conversion.forceFactor");

	ForceIn2Phys_K[0] = (
		programController->parser->options.get<float>("conversion.0PhysForceMax") -
		programController->parser->options.get<float>("conversion.0PhysForceMin")) /
		(programController->parser->options.get<float>("conversion.0InputForceMax") -
		 programController->parser->options.get<float>("conversion.0InputForceMin"));
	ForceIn2Phys_K[1] = (
		programController->parser->options.get<float>("conversion.1PhysForceMax") -
		programController->parser->options.get<float>("conversion.1PhysForceMin")) /
		(programController->parser->options.get<float>("conversion.1InputForceMax") -
		 programController->parser->options.get<float>("conversion.1InputForceMin"));

	ForceIn2Phys_Offset[0] = programController->parser->options.get<float>("conversion.0PhysForceMin")-
			programController->parser->options.get<float>("conversion.0InputForceMin") * ForceIn2Phys_K[0];
	ForceIn2Phys_Offset[1] = programController->parser->options.get<float>("conversion.1PhysForceMin")-
			programController->parser->options.get<float>("conversion.1InputForceMin") * ForceIn2Phys_K[1];

	ForcePhys2Out_K[0] = (
		programController->parser->options.get<float>("conversion.0OutForceMax") -
		programController->parser->options.get<float>("conversion.0OutForceMin")) /
		(programController->parser->options.get<float>("conversion.0PhysForceMax") -
		 programController->parser->options.get<float>("conversion.0PhysForceMin"));
    ForcePhys2Out_K[1] = (
		programController->parser->options.get<float>("conversion.1OutForceMax") -
		programController->parser->options.get<float>("conversion.1OutForceMin")) /
		(programController->parser->options.get<float>("conversion.1PhysForceMax") -
		 programController->parser->options.get<float>("conversion.1PhysForceMin"));

	ForcePhys2Out_Offset[0] = programController->parser->options.get<float>("conversion.0OutForceMin") -
		programController->parser->options.get<float>("conversion.0PhysForceMin") * ForcePhys2Out_K[0];
	ForcePhys2Out_Offset[1] = programController->parser->options.get<float>("conversion.1OutForceMin")-
		programController->parser->options.get<float>("conversion.1PhysForceMin") * ForcePhys2Out_K[1];

// and now position calib
	PosIn2Phys_K[0] = (
		programController->parser->options.get<float>("conversion.0PhysPosMax") -
		programController->parser->options.get<float>("conversion.0PhysPosMin")) /
		(programController->parser->options.get<float>("conversion.0InputPosMax") -
		 programController->parser->options.get<float>("conversion.0InputPosMin"));
	PosIn2Phys_K[1] = (
		programController->parser->options.get<float>("conversion.1PhysPosMax") -
		programController->parser->options.get<float>("conversion.1PhysPosMin")) /
		(programController->parser->options.get<float>("conversion.1InputPosMax") -
		 programController->parser->options.get<float>("conversion.1InputPosMin"));

	PosIn2Phys_Offset[0] = programController->parser->options.get<float>("conversion.0PhysPosMin")-
			programController->parser->options.get<float>("conversion.0InputPosMin") * PosIn2Phys_K[0];
	PosIn2Phys_Offset[1] = programController->parser->options.get<float>("conversion.1PhysPosMin")-
			programController->parser->options.get<float>("conversion.1InputPosMin") * PosIn2Phys_K[1];

	PosPhys2Out_K[0] = (
		programController->parser->options.get<float>("conversion.0OutPosMax") -
		programController->parser->options.get<float>("conversion.0OutPosMin")) /
		(programController->parser->options.get<float>("conversion.0PhysPosMax") -
		 programController->parser->options.get<float>("conversion.0PhysPosMin"));
    PosPhys2Out_K[1] = (
		programController->parser->options.get<float>("conversion.1OutPosMax") -
		programController->parser->options.get<float>("conversion.1OutPosMin")) /
		(programController->parser->options.get<float>("conversion.1PhysPosMax") -
		 programController->parser->options.get<float>("conversion.1PhysPosMin"));

	PosPhys2Out_Offset[0] = programController->parser->options.get<float>("conversion.0OutPosMin") -
		programController->parser->options.get<float>("conversion.0PhysPosMin") * PosPhys2Out_K[0];
	PosPhys2Out_Offset[1] = programController->parser->options.get<float>("conversion.1OutPosMin")-
		programController->parser->options.get<float>("conversion.1PhysPosMin") * PosPhys2Out_K[1];


	// memset(readBuffer, 0, sizeof(readBuffer));
	//memset(states, 0, sizeof(ArduinoState) * 2);
	states.resize(TOT_CH);

	for (int i = 0; i < TOT_CH; ++i) {
		char buffer[200];
		programController->completeLogFile << "Conversion table for channel: " <<  i << endl;
		sprintf(buffer, "F In K: %2.3f   F In Offset: %2.3f   F Out K: %2.3f   F Out Offset: %2.3f",
				ForceIn2Phys_K[i], ForceIn2Phys_Offset[i], ForcePhys2Out_K[i], ForcePhys2Out_Offset[i]  );

		programController->completeLogFile << string(buffer) << "\\r\\n" << endl;
		sprintf(buffer, "P In K: %2.3f   P In Offset: %2.3f   P Out K: %2.3f   P Out Offset: %2.3f",
				PosIn2Phys_K[i], PosIn2Phys_Offset[i], PosPhys2Out_K[i], PosPhys2Out_Offset[i]  );

		programController->completeLogFile << string(buffer) << "\\r\\n" << endl<<endl;

/*perchecacchio non gli piace? puzzone!
		string FConst = "F In K: " 	+  boost::lexical_cast<std::string>(ForceIn2Phys_K[i]) ;
				" F In Offset: " 	+ lexical_cast<string>(ForceIn2Phys_Offset[i]) +
				" F Out K: " 		+ lexical_cast<string>(ForcePhys2Out_K[i]) +
				" F Out Offset: " 	+ lexical_cast<string>(ForcePhys2Out_Offset[i]) +
				" P In K: "  		+ lexical_cast<string>(PosIn2Phys_K[i])    +
				" P In Offset: "  	+ lexical_cast<string>(PosIn2Phys_Offset[i]) +
				" P Out K: " 		+ lexical_cast<string>(PosPhys2Out_K[i])   +
				" P Out K: " 		+ lexical_cast<string>(PosPhys2Out_Offset[i]);
*/
	}

}

Footboard::~Footboard()
{
}




int Footboard::GetStateFromArduino(void)
{
// AAA Attenzione! per qualche motivo un baudrate erroneo mi ritorna tutti zero senza dare errore!
    string line = serial.ReadLine();

    if(line.empty())
        return 0;

    if(line[0] != '0' && line[0] != '1')
    {
        cout << "--- Debug Msg: <" << line << ">" << endl;
    	programController->samplingLogFile << line << "\\r\\n" << endl;
    	programController->completeLogFile << line << "\\r\\n" << endl;
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
    states[channel].force = (int) force * ForceIn2Phys_K[channel]+ ForceIn2Phys_Offset[0];
    states[channel].position = (int) position * PosIn2Phys_K[channel] + PosIn2Phys_Offset[channel];
    states[channel].pid = pid;
    states[channel].ef = ef;
    states[channel].epos = epos;

    return 1;
}



char arduinoCommandBuffer[64];

bool Footboard::SendForceCommandToArduino(const int& channel, const int& force, const int& maxForce)
{
	int arduinoForce = int( 0.5 + forceConversionFactor * ( force * ForcePhys2Out_K[channel] + ForcePhys2Out_Offset[channel] ));
	int arduinoMaxForce = maxForce;
	//	int arduinoMaxForce = int (0.5 + forceConversionFactor * (maxForce * ForcePhys2Out_K[channel] + ForcePhys2Out_Offset[channel] ));

	if (arduinoForce < 1 || arduinoForce > 999 || arduinoMaxForce < 0 || arduinoMaxForce > 99)
	{
		cout << "warn: force command params out of bounds will NOT send command to Arduino" << endl;
		cout << "force received = " << force    << "  force converted = " << arduinoForce << endl;
		cout << "Max f received = " << maxForce << "  Max f converted = " << arduinoMaxForce << endl;
		return false;
	}

	// char buf[64];
	sprintf(arduinoCommandBuffer, "%1d %03d %02d\r\n", channel, (int) arduinoForce, (int) arduinoMaxForce);

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
	int arduinoPosition = int (0.5 + positionConversionFactor *
			(position_ * PosPhys2Out_K[channel_] + PosPhys2Out_Offset[channel_]));
	//int arduinoForce =  int (0.5 + forceConversionFactor *
	//		( maxForceToGetToPosition_ * ForcePhys2Out_K[channel_] + ForcePhys2Out_Offset[channel_]));

	int arduinoForce = maxForceToGetToPosition_;

	if (arduinoPosition < 0 || arduinoPosition > 999 || arduinoForce < 0 || arduinoForce > 99)
	{
			cout << "warn: position command params out of bounds: will NOT send command to Arduino" << endl;
			cout << "position received = " << position_    <<   		"   position converted = " << arduinoPosition << endl;
			cout << "Max f received = "   << maxForceToGetToPosition_ <<	"  Max f converted = " << arduinoForce << endl;
			return false;
	}

	sprintf(arduinoCommandBuffer, "P %1d %03d %02d\r\n", channel_, (int) arduinoPosition, (int) arduinoForce);

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

//	cout << "Evaluating cmd: " << c->AsString() << " Channel: " << channel << endl;

	if (channel != -1)
	{
		actuators[(channel+1)%2].IsCommandExpired();  //prova di luca
		bool ret = actuators[channel].Accept(c);
		if (ret == true)
			programController->completeLogFile << "\n----- command start -----\n" << "Actuator is accepting command: "
					<< c->AsString() << " (line: " << c->lineNumber << ")" << endl;
		return ret;
	}

	// se arriviamo qui e' un comando per la footboard
	assert(channel == -1);

	// questo test viene fatto perche' i comandi per la footboard, ovvero i vari semafori,
	// sono considerati expired in funzione della expiration dei comandi che stanno girando
	// sui diversi canali. In verita' trae un po' in inganno, perche' i semafori non "fanno" nulla,
	// ma semplicemente bloccano fino a quando non sono expired. Questo implica che CI VUOLE SEMPRE
	// un S X alla fine del programma
//	cout << "Command: Expired status is: " << c-> IsExpired() << endl;
 	if (c->IsExpired())
	{
		cout << "\n----- command start -----\n" << "Footboard is accepting command: " << c->AsString()
		<< " (line: " << c->lineNumber << ")" << endl;
		c->OnAccept();
		return true;
	}

	return false;
}

