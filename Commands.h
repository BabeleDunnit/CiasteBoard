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
using namespace boost;

#include <string>
using namespace std;

class ProgramController;

struct Command
{
	// Command(void) : expiredMemory(false) {}

	virtual int GetChannel(void) { return -1; }

	virtual string AsString(void) { return "null command"; }

	// per ora, per farlo uscire, accetto sempre
	virtual bool IsExpired(void) { return true; }
	virtual bool Kill(void) { return true; }

	// virtual bool Execute(void) { return true; }
	virtual void OnAccept(void);

	ptime acceptTime;

	// abbiamo bisogno di sapere dove era Arduino quando questo questo comando e' stato accettato
	int arduinoPositionOnAccept;

	// il numero di linea di questo comando nel programma sorgente
	int lineNumber;

	shared_ptr<ProgramController> programController;

	// in verita' avevo capito male il comando S X: non deve controllare in "real time" se entrambi i comandi
	// sono expired, ma fermare l'esecuzione del comando expired fino a quando anche l'altro non e' finito.
	// prima dell'introduzione di questo flag, io potevo avere che CH0 era expired ma CH1 no, e se poi switchavano
	// non si passava alla prossima istruzione. Vogliamo invece che si passi alla prossima istruzione.
	// pero' e' meglio inscatolare tutto nel SemaphoreCommand
	// bool expiredMemory;
};

struct PositionCommand: public Command
{
	PositionCommand(const int& c, const int& p, const int& tl) :
			channel(c), position(p), timeLimit(tl)
	{
	}

	virtual int GetChannel(void) { return channel; }

	virtual string AsString(void)
	{
		return "ch:" +lexical_cast<string>(channel) + " P "
				+ "pos:" + lexical_cast<string>(position) + " timeout:"
				+ lexical_cast<string>(timeLimit);
	}

	virtual bool IsExpired(void);
	virtual bool Kill(void);

	// virtual bool Execute(void);
    virtual void OnAccept(void);

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
		return PositionCommand::AsString() + " optMaxF:"
				+ lexical_cast<string>(optionalMaxForce);
	}

	// virtual bool Execute(void);
    virtual void OnAccept(void);

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
		return "ch:" + lexical_cast<string>(channel) + " F "
				+ "f:" +lexical_cast<string>(force) + " "
				+ "pos:" + lexical_cast<string>(position) + " "
				+ "timeout:" + lexical_cast<string>(timeLimit);
	}

	virtual bool IsExpired(void);
	// virtual bool Execute(void);
    virtual void OnAccept(void);

	bool IsPositionReached(void);

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
				+ "optDelta:" + lexical_cast<string>(optionalDelta);
	}

    virtual bool IsExpired(void);
	// virtual bool Execute(void);
    virtual void OnAccept(void);

	int optionalDelta;
    ptime lastLoopTime;

};

struct SemaphoreCommand: public Command
{
	SemaphoreCommand(const string& m) :
			mode(m), isExpired0(false), isExpired1(false)
	{
	}

	virtual string AsString(void) { return "s " + mode; }

	virtual bool IsExpired(void);

    bool isExpired0;
    bool isExpired1;

	string mode;
};


#endif /* COMMANDS_H_ */
