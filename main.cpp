///////////////////////////////////////////////////////////////////////////////
/// \file		main.cpp
///
/// \author		Christoffer Wesselhoff
/// \author		Kasper Jensen
/// \date		27/03-2020
/// \version	1.0
///////////////////////////////////////////////////////////////////////////////

#include"includes/startup.h"
#include<stdlib.h>
#include<stdio.h>

using namespace std;

int main(void) {
	int machine = whichMachine();
	if(machine == 1) {
		if(!configPin("P1-31","qep")) {
			return 1;
		}
		if(!configPin("P1-33","pwm")) {
			return 1;
		}
	}
	else if (machine == 0) {
		if(!configPin("P8-35","qep")) {
			return 1;
		}
		if(!configPin("P9-14","pwm")) {
			return 1;
		}
	}
	int i, ret = system("/home/blob/internettest.sh");
	i = WEXITSTATUS(ret);
	std::cout << endl << i << endl;

	return 0;
}


