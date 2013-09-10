/*
 * Sound.cpp
 *
 *  Created on: Sep 10, 2013
 *      Author: babele
 */

//#include <stdio.h>
#include <stdlib.h>

#include "Sound.h"

Sound::Sound(const string& soundFile) :
		fileName(soundFile)
{
	lastPlayTime = boost::posix_time::microsec_clock::local_time() - boost::posix_time::seconds(3);
}

Sound::~Sound()
{
	// TODO Auto-generated destructor stub
}

void Sound::ImmediatePlay(const bool& async)
{
	if (async)
		system(("aplay " + fileName + " & > /dev/null").c_str());
	else
		system(("aplay " + fileName).c_str());
}

void Sound::Play(void)
{

	ptime timeNow = boost::posix_time::microsec_clock::local_time();
	if ((timeNow - lastPlayTime).total_microseconds() >= 2000000)
	{
		ImmediatePlay();
		lastPlayTime = timeNow;
	}
}

