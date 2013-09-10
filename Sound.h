/*
 * Sound.h
 *
 *  Created on: Sep 10, 2013
 *      Author: babele
 */

#ifndef SOUND_H_
#define SOUND_H_

#include <string>
using namespace std;

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

class Sound
{
public:
	Sound(const string& fileName);
	virtual ~Sound();
	void ImmediatePlay(const bool& async = true);

	// usare QUESTO metodo, che ha un controllo con timer etc
	void Play(void);

private:
	string fileName;
	ptime lastPlayTime;
};

#endif /* SOUND_H_ */
