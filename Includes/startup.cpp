///////////////////////////////////////////////////////////////////////////////
/// \file		startup.cpp
///				Source file containing the implematations of the functions 
///				configPin and whichMachine used in the PRO4 projekt at 
///				Electrical Engineering at AU Herning.
/// \author		Christoffer Wesselhoff
/// \author		Kasper Jensen
/// \date		27/03-2020
/// \version	1.0
///////////////////////////////////////////////////////////////////////////////
#include"startup.h"

bool configPin(string pin, string mode){
	stringstream commnd;			    // Stringsteam containing shell command
	commnd << "config-pin " << pin << " " << mode;	// Assemble the stringsteam
	if(system(commnd.str().c_str()) != 0){ 	   // Execute the assembled command
		// Report error this should be changed to fit the error handling
		cout << "Error during setup of " << pin << endl;
		return false;
	}
	commnd.str(""); 									   // Clear stringsteam
	commnd << "config-pin -q" << pin;			  // Assemble new shell command
	if(system(commnd.str().c_str()) != 0) {    // Execute the assembled command
		// Report error this should be changed to fit the error handling
		cout << "Error during setup of " << pin << endl;
		return false;
	}
	return true;
}

int whichMachine(){
	string val; 			 // String variable for easy compare
	val = getenv("MACHINE"); // Get value of environment variable machine
	if(!val.compare("Black"))//	If Black return 0
		return 0;
	else if(!val.compare("PocketBeagle")) // If PocketBeagle return 1
		return 1;
	else					 // If not, return -1
		return -1;
}
