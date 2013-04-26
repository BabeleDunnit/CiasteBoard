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

//#include <boost/date_time/posix_time/posix_time.hpp>
//using namespace boost::posix_time;
#include <boost/property_tree/json_parser.hpp>
using namespace boost::property_tree;
#include <boost/shared_ptr.hpp>
// #include <boost/lexical_cast.hpp>
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

	int fdl;
	int fll;
	int pdf;

	shared_ptr<vector<shared_ptr<Command> > > commands;

	int parsedLineNum;

    ptree options;

	shared_ptr<ProgramController> programController;
};

#endif /* PROGRAMPARSER_H_ */
