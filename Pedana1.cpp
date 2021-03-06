//============================================================================
// Name        : Pedana1.cpp
// Author      : babele
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================


#include <iostream>
#include "ProgramController.h"
#include "ProgramParser.h"

#include "Sound.h"

/*
 * History
 * 20130430 vediamo i comandi di forza
 */


int main(void)
{

#ifdef ARDUINOTEST

	ArduinoSerial as("/dev/ttyACM0");
	ArduinoCommander ac(as);

	/*
	 int count = 0;
	 int val = 100;
	 while(1)
	 {
	 char buf[64];
	 int len = as.read(buf,64);
	 buf[len] = 0;
	 cout << buf;
	 count++;
	 if( count % 50 == 0)
	 {
	 string s = "0 " + lexical_cast<string>(val) + " 50" + lexical_cast<string>(13) + lexical_cast<string>(10);
	 cout << "mando comando " << s << endl;
	 as.write(s.c_str(), s.length());
	 val += 30;
	 }

	 }
	 */


	int count = 0;
	int which = 0;
	int val = 10;
	while (1)
	{
//		char buf[64];
//		int len = as.read(buf,64);
//		buf[len] = 0;
//		cout << buf;
		count++;
		bool good = ac.GetStateFromArduino();
		if (!good)
		{
			cout << "errore di lettura" << endl;
			cout << "arduino aveva detto: " << ac.readBuffer << endl;
		}
		else
		{
//			cout << "dx:" << ac.getLastStateString(0) << " rx:"
//					<< ac.getLastStateString(1) << endl;
		}

		if (count % 10 == 0)
		{
//			string s = "0 " + lexical_cast<string>(val) + " 50" + lexical_cast<string>(13) + lexical_cast<string>(10);
//			as.write(s.c_str(), s.length());

			val += 13;
			which++;
			if (which % 2 || which % 3)
			{
				cout << "mando comando forza" << endl;
				bool good = ac.SendForceCommandToArduino(0, val, 50);
				// assert(good);
			}
			//usleep(100000);
			else
			{
				cout << "mando comando posizione" << endl;
				good = ac.SendPositionCommandToArduino(0, val, 50);
			}
			// assert(good);

		}

	}
#endif


	shared_ptr<ProgramController> controller(new ProgramController);
	if(!controller->Init())
		exit(-1);

    string serialName = controller->parser->options.get<string>("serial");
    if(serialName.length())
    {
		cout << "Arduino expected on serial " << serialName << endl;
		controller->footboard->serial.Open(serialName);
		cout << "Serial opened" << endl;
	    usleep(1000000);
    }

    cout << "Now reading from serial" << endl;
    // leggiamo un po' di roba per sincronizzarci e svuotare il buffer
    for(int i = 0; i < 8; i++)
    	controller->footboard->GetStateFromArduino();

    cout << "Execution Start!" << endl;
    // Sound("claxon.wav").Play();
    controller->claxon->Play();
	controller->Run();

	// controller.reset(NULL);
	// controller->footboard->serial.Close();
	// }
	// XXX TODO per quale MINCHIA di ragione non vengono chiamati i distruttori???

	cout << "Program End" << endl;

	return 0;
}
