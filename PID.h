/*
 * PID.h
 *
 *  Created on: Mar 13, 2013
 *      Author: babele
 */

#ifndef PID_H_
#define PID_H_

#include <boost/circular_buffer.hpp>
using namespace boost;

class PID
{
public:
	PID();
	virtual ~PID();

	int AddValue(const int& v);
	void setTargetValue(const int& v) { targetValue = v; }
	void setK(const float& p, const float& i, const float& d) { kp = p; ki = i; kd = d; }

	circular_buffer<int> values;
	float kp, ki, kd;
	int targetValue;

	int previousDelta;
};

#endif /* PID_H_ */
