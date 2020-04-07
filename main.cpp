///////////////////////////////////////////////////////////////////////////////
/// \file		main.cpp
///
/// \author		Christoffer Wesselhoff
/// \author		Kasper Jensen
/// \date		27/03-2020
/// \version	1.0
///////////////////////////////////////////////////////////////////////////////

#include<stdlib.h>
#include<stdio.h>
#include "includes/startup.h"
#include "includes/states.h"

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
			cout << "Test started" << endl;
			exitvar = true;
			break;
		default:
			exitvar = true;
			break;
		}
	}
	return 0;
}


