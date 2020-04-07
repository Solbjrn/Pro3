///////////////////////////////////////////////////////////////////////////////
/// \file		states.cpp
///				Contains the functions defining the states for the state 
///				machine used in the PRO4 projekt at Electrical Engineering at 
///				AU
///				Herning.
/// \author		Christoffer Wesselhoff
/// \author		Kasper Jensen
/// \date		07/04-2020
/// \version	1.0
///////////////////////////////////////////////////////////////////////////////

#include"startup.h"
#include"states.h"
#include<thread>
#include<chrono>

int testConnection(void);
///< Runs the internet test script. 
///< \return exit value from internet test script.

int testConnection(void) {
	int ret = system(INTERNET_TEST);
	return WEXITSTATUS(ret);
}

enum state powerOnState(void){
	// Checks whether its an Pocketbeagle or Beaglebone black
	int machine = whichMachine();
	// If pocketbeagle set up P1-31 and P1-33
	if(machine == 1) {
		if(!configPin("P1-31","qep")) {
			return error_pin;
		}
		if(!configPin("P1-33","pwm")) {
			return error_pin;
		}
	}
	// If Beaglebone Black set up P1-31 and P1-33
	else if (machine == 0) {
		if(!configPin("P8-35","qep")) {
			return error_pin;
		}
		if(!configPin("P9-14","pwm")) {
			return error_pin;
		}
	}
	// On sucess return idle as next state
	if (testConnection() == 0) {
		return idle;
	}
	// On failure return error_connection as next state
	else {
		return error_connection;
	}
}

enum state idleState(void){
	while(1){
		// If test is recieved return start test as next state
		if (testConfig.isRecieved)
			return start_test;
		// Else sleep and test internet connection
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
