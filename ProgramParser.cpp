/*
 * ProgramParser.cpp
 *
 *  Created on: Mar 14, 2013
 *      Author: brancotti
 */

#include "ProgramParser.h"

#include<boost/tokenizer.hpp>
#include<boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
// using namespace boost::;

#include <iostream>
#include <fstream>

using namespace std;

ProgramParser::ProgramParser()
{
	fdl = fll = pdf = -1;
}

ProgramParser::~ProgramParser()
{
}

bool ProgramParser::ParseProgram(const string& path)
{
	ifstream program(path.c_str());
	parsedLineNum = 0;
	while (program)
	{
		char linebuf[512];
		program.getline(linebuf, 512);
		++parsedLineNum;
		string line(linebuf);
		trim(line);
		cout << "line read: " << parsedLineNum << " " << line << endl;

		// io AMO boost.
		// boost::char_separator<char> sep;
		// boost::char_separator<char> sep("-;", "|", boost::keep_empty_tokens);
		boost::char_separator<char> sep(" ");
		tokenizer<boost::char_separator<char> > tok(line, sep);

		vector<string> tokens;
		for (tokenizer<boost::char_separator<char> >::iterator i = tok.begin();
				i != tok.end(); ++i)
		{
			tokens.push_back(to_lower_copy(*i));
			// cout << "<" << *i << ">" << endl;
		}



		if (tokens.empty() || tokens[0][0] == '#')
		{
			// commento o linea vuota
		}
		else if (tokens[0] == "f" && tokens[1] == "dl")
		{
			if (fdl != -1)
			{
				cout << "ERROR: F DL set again at line " << parsedLineNum << endl;
				return false;
			}

			if (tokens.size() < 3)
			{
				cout << "ERROR: F DL value not found at line " << parsedLineNum
						<< endl;
				return false;
			}

			fdl = lexical_cast<int>(tokens[2]);
		}
		else if (tokens[0] == "f" && tokens[1] == "ll")
		{
			if (fll != -1)
			{
				cout << "ERROR: F LL set again at line " << parsedLineNum << endl;
				return false;
			}

			if (tokens.size() < 3)
			{
				cout << "ERROR: F LL value not found at line " << parsedLineNum
						<< endl;
				return false;
			}

			fll = lexical_cast<int>(tokens[2]);
		}
		else if (tokens[0] == "p" && tokens[1] == "df")
		{
			if (pdf != -1)
			{
				cout << "ERROR: P DF set again at line " << parsedLineNum << endl;
				return false;
			}

			if (tokens.size() < 3)
			{
				cout << "ERROR: P DF value not found at line " << parsedLineNum
						<< endl;
				return false;
			}

			pdf = lexical_cast<int>(tokens[2]);

		}
		else if (tokens[1] == "f")
		{
			if(!ParseForceCommand(tokens))
				return false;
		}
		else if (tokens[1] == "p")
		{
			if(!ParsePositionCommand(tokens))
				return false;
		}
		else if (tokens[0] == "s")
		{
			if(!ParseSemaphoreCommand(tokens))
				return false;
		}
		else
		{
			cout << "ERROR: unrecognized command at line " << parsedLineNum << endl;
			return false;
		}

	}

	commands.push_back(shared_ptr<Command>(new Command()));

	cout << "Parsed " << commands.size() << " commands" << endl;
	for(int i = 0; i < commands.size(); ++i)
		cout << commands[i]->AsString() << endl;

	return true;
}

bool ProgramParser::ParseOptions(const string& path)
{
    read_json("options.json", options);
	return true;
}

bool ProgramParser::ParseForceCommand(const vector<string>& tokens)
{
	assert(tokens[1] == "f");
	if (tokens.size() != 5 && tokens.size() != 6)
	{
		cout << "ERROR: malformed F command at line " << parsedLineNum << endl;
		return false;
	}

	int channel = lexical_cast<int>(tokens[0]);
	int force = lexical_cast<int>(tokens[2]);
	int position = lexical_cast<int>(tokens[3]);
	int timelimit = lexical_cast<int>(tokens[4]);

	Command* cmd = NULL;
	if (tokens.size() > 5)
		cmd = new ForceWithDeltaCommand(channel, force, position, timelimit,
				lexical_cast<int>(tokens[5]));
	else
		cmd = new ForceCommand(channel, force, position, timelimit);

	shared_ptr<Command> fc(cmd);
	commands.push_back(fc);

	return true;
}

bool ProgramParser::ParsePositionCommand(const vector<string>& tokens)
{
	assert(tokens[1] == "p");
	if (tokens.size() != 4 && tokens.size() != 5)
	{
		cout << "ERROR: malformed P command at line " << parsedLineNum << endl;
		return false;
	}

	int channel = lexical_cast<int>(tokens[0]);
	int position = lexical_cast<int>(tokens[2]);
	int timelimit = lexical_cast<int>(tokens[3]);

	Command* cmd = NULL;
		if (tokens.size() > 4)
			cmd = new PositionWithDeltaCommand(channel, position, timelimit, lexical_cast<int>(tokens[4]));
		else
			cmd = new PositionCommand(channel, position, timelimit);

	shared_ptr<Command> pc(cmd);
	commands.push_back(pc);

	return true;
}

bool ProgramParser::ParseSemaphoreCommand(const vector<string>& tokens)
{
	assert(tokens[0] == "s");
	shared_ptr<Command> sc(new SemaphoreCommand(tokens[1]));
	commands.push_back(sc);

	return true;
}

