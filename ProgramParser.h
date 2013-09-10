/*
 * ProgramParser.h
 *
 *  Created on: Mar 14, 2013
 *      Author: brancotti
 */

#ifndef PROGRAMPARSER_H_
#define PROGRAMPARSER_H_

#include <string>
#include <vector>
using namespace std;

#include <boost/property_tree/json_parser.hpp>
using namespace boost::property_tree;
#include <boost/shared_ptr.hpp>
using namespace boost;

class ProgramController;
class Command;

class ProgramParser
{
public:
	ProgramParser(shared_ptr<ProgramController> pc);
	virtual ~ProgramParser();

	bool ParseProgram(const string& path);
	bool ParseOptions(const string& path);

	bool ParseForceCommand(const vector<string>& tokens);
	bool ParsePositionCommand(const vector<string>& tokens);
	bool ParseSemaphoreCommand(const vector<string>& tokens);

	bool ParseThresholdCommand(const vector<string>& tokens);

	// qui l'idea sarebbe parsare una cosa del genere
	// trigger <nome> <stato> <canale> <condizione> <azione>
	// trigger 1 on chan0 position high 300 playsound pippo.wav
	// trigger 1 off
	// bool ParseTriggerCommand(const vector<string>& tokens);
	// bool ParseTriggerCondition()

	int fdl;
	int fll;
	int pdf;

	// int upperThreshold[2];
	// int lowerThreshold[2];

	// primo indice: canale
	// secondo indice: 0 = down(giu), 1 = up(su)
	// quando non vogliamo tenerne conto ci schiaffiamo dentro -1
	int thresholds[2][2];

	shared_ptr<vector<shared_ptr<Command> > > commands;

	int parsedLineNum;

    ptree options;

	shared_ptr<ProgramController> programController;
};

#endif /* PROGRAMPARSER_H_ */
