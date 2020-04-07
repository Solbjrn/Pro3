/*
 * states.h
 *
 *  Created on: 1 Apr 2020
 *      Author: Christoffer
 */

#ifndef STATES_H_
#define STATES_H_

enum state{power_on, start_test, idle, error_pin, error_connection};
// When a config has been recieved, a class should be filled with parameters
static class config{
public:
	bool isRecieved = 1;
	bool isPID;
	float rpm;
	float voltage;
} testConfig;

enum state powerOnState(void);
enum state idleState(void);
void ErrorConnectionState(void);
void ErrorPinState(void);



#endif /* STATES_H_ */
