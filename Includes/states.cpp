/*
 * states.cpp
 *
 *  Created on: 1 Apr 2020
 *      Author: Christoffer
 */

#include"startup.h"
#include"states.h"
#include<thread>
#include<chrono>

#define IDLE_WAIT 5
#define INTERNET_TEST "/home/blob/internettest.sh"

int testConnection(void) {
	int ret = system(INTERNET_TEST);
	return WEXITSTATUS(ret);
}

enum state powerOnState(void){
	int machine = whichMachine();
	if(machine == 1) {
		if(!configPin("P1-31","qep")) {
			return error_pin;
		}
		if(!configPin("P1-33","pwm")) {
			return error_pin;
		}
	}
	else if (machine == 0) {
		if(!configPin("P8-35","qep")) {
			return error_pin;
		}
		if(!configPin("P9-14","pwm")) {
			return error_pin;
		}
	}
	if (testConnection() == 0) {
		return idle;
	}
	else {
		return error_connection;
	}
}

enum state idleState(void){
	while(1){
		if (testConfig.isRecieved)
			return start_test;
		else{
			std::this_thread::sleep_for(std::chrono::seconds(IDLE_WAIT));
			if (testConnection() != 0)
				return error_connection;
			// debug
			std::cout << "no test recieved" << std::endl;
		}
	}
}

void ErrorConnectionState(void) {
	//debug
	std::cout << "Connection error" << std::endl;
}

void ErrorPinState(void) {
	//debug
	std::cout << "Pin error" << std::endl;
}
