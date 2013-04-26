/*
 * Commands.h
 *
 *  Created on: Mar 24, 2013
 *      Author: babele
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
#include <boost/shared_ptr.hpp>
// #include <boost/lexical_cast.hpp>
using namespace boost;

#include <string>
using namespace std;

class ProgramController;

struct Command
{
	virtual int GetChannel(void)
	{
		return -1;
	}

	virtual string AsString(void) { return "null command"; }

	// per ora, per farlo uscire, accetto sempre
	virtual bool IsExpired(void) { return true; }
	virtual bool Kill(void) { return true; }

	virtual bool Execute(void) { return true; }

	ptime acceptTime;
	shared_ptr<ProgramController> programController;
};

struct PositionCommand: public Command
{
	PositionCommand(const int& c, const int& p, const int& tl) :
			channel(c), position(p), timeLimit(tl)
	{
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

	virtual bool IsExpired(void);
	virtual bool Kill(void);

	virtual bool Execute(void);

	int channel;
	int position;
	int timeLimit;

};

struct PositionWithMaxForceCommand: PositionCommand
{
	PositionWithMaxForceCommand(const int& c, const int&p, const int& tl,
			const int& mf) :
			PositionCommand(c, p, tl), optionalMaxForce(mf)
	{
	}

	virtual string AsString(void)
	{
		return PositionCommand::AsString() + " "
				+ lexical_cast<string>(optionalMaxForce);
	}

	virtual bool Execute(void);

	int optionalMaxForce;
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

	virtual bool IsExpired(void);

	string mode;
};


#endif /* COMMANDS_H_ */
