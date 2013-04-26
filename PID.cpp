/*
 * PID.cpp
 *
 *  Created on: Mar 13, 2013
 *      Author: babele
 */

#include "PID.h"

const int BUFLEN = 50;

PID::PID()
{
	// TODO Auto-generated constructor stub
	values.resize(BUFLEN);
	kp = ki = kd = 1.0;
	targetValue = 0;
	previousDelta = 0;
}

PID::~PID()
{
	// TODO Auto-generated destructor stub
}

int PID::AddValue(const int& v)
{

	int delta = v - targetValue;
	values.push_back(delta);
	int sum = 0;
	for(circular_buffer<int>::iterator i = values.begin(); i != values.end(); ++i)
		sum += *i;
	float meanError = sum / (float)BUFLEN;

	int delta2 = delta - previousDelta;
	int ret = kp * delta + ki * meanError + kd * delta2;

	previousDelta = delta;
	return ret;

}
