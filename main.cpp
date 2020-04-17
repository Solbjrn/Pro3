///////////////////////////////////////////////////////////////////////////////
/// \file		main.cpp
///
/// \author		Christoffer Wesselhoff
/// \author		Kasper Jensen
/// \date		27/03-2020
/// \version	1.0
///////////////////////////////////////////////////////////////////////////////

#include<stdlib.h>
#include<iostream>
#include "Includes/states.h"

using namespace std;

int main(void) {
	bool exitvar = false;
	state current_state, next_state;
	current_state = power_on;
	while(!exitvar) {
		current_state = next_state;
		switch(current_state){
		case power_on:
			cout << "poweron" << endl;
			next_state = powerOnState();
			break;
		case idle:
			next_state = idleState();
			break;
		case error_connection:
			cout << "errorstate" << endl;
			ErrorConnectionState();
			//debug
			exitvar = true;
			break;
		case error_pin:
			ErrorPinState();
			//debug
			exitvar = true;
			break;
		case start_test:
			//debug
			cout << "testStart" << endl;
			exitvar = true;
			break;
		case voltage_test_init:
			//debug
			cout << "voltage test start" << endl;
			exitvar = true;
			break;
		case pid_test_init:
			//debug
			cout << "pid test start" << endl;
			exitvar = true;
			break;
		case test:
			//debug
			cout << "Test state" << endl;
			next_state = TestState();
			break;
		case return_results:
			//debug
			cout << "Return result" << endl;
			next_state = ReturnResultsState();
			exitvar = true;
			break;
		default:
			exitvar = true;
			break;
		}
	}
	return 0;
}


