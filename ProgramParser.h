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

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost;

struct Command
{
	virtual int GetChannel(void)
	{
		return -1;
	}

	virtual string AsString(void) { return "null command"; }

	ptime acceptTime;
};

//struct PFCommand : public Command
//{
//	int channel;
//	int position;
//	int timeLimit;
//};

struct PositionCommand: public Command
{
	PositionCommand(const int& c, const int& p, const int& tl) :
			channel(c), position(p), timeLimit(tl)
	{
//		channel = c;
//		position = p;
//		timeLimit = tl;
	}

	virtual int GetChannel(void)
	{
		return channel;
	}

	virtual string AsString(void)
	{
		return lexical_cast<string>(channel) + " p "
				+ lexical_cast<string>(position) + " "
				+ lexical_cast<string>(timeLimit);
	}

	int channel;
	int position;
	int timeLimit;

};

struct PositionWithDeltaCommand: PositionCommand
{
	PositionWithDeltaCommand(const int& c, const int&p, const int& tl,
			const int& d) :
			PositionCommand(c, p, tl), optionalDelta(d)
	{
	}

	virtual string AsString(void)
	{
		return PositionCommand::AsString() + " "
				+ lexical_cast<string>(optionalDelta);
	}

	int optionalDelta;
};

struct ForceCommand: public PositionCommand
{
	ForceCommand(const int& c, const int& f, const int&p, const int& tl) :
			PositionCommand(c, p, tl), force(f)
	{
	}

	virtual string AsString(void)
	{
		return lexical_cast<string>(channel) + " f "
				+ lexical_cast<string>(force) + " "
				+ lexical_cast<string>(position) + " "
				+ lexical_cast<string>(timeLimit);
	}

	int force;
};

struct ForceWithDeltaCommand: public ForceCommand
{
	ForceWithDeltaCommand(const int& c, const int& f, const int&p,
			const int& tl, const int& d) :
			ForceCommand(c, f, p, tl), optionalDelta(d)
	{
	}

	virtual string AsString(void)
	{
		return ForceCommand::AsString() + " "
				+ lexical_cast<string>(optionalDelta);
	}

	int optionalDelta;
};

struct SemaphoreCommand: public Command
{
	SemaphoreCommand(const string& m) :
			mode(m)
	{
	}

	virtual string AsString(void)
	{
		return "s " + mode;
	}

	string mode;
};

class ProgramParser
{
public:
	ProgramParser();
	virtual ~ProgramParser();

	bool ParseProgram(const string& path);
	bool ParseOptions(const string& path);

	bool ParseForceCommand(const vector<string>& tokens);
	bool ParsePositionCommand(const vector<string>& tokens);
	bool ParseSemaphoreCommand(const vector<string>& tokens);

	int fdl;
	int fll;
	int pdf;

	vector<shared_ptr<Command> > commands;

	int parsedLineNum;
};

#endif /* PROGRAMPARSER_H_ */
