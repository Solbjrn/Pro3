///////////////////////////////////////////////////////////////////////////////
/// \file		states.cpp
///				Header file containing the functions configPin and whichMachine
///				used in the PRO4 projekt at Electrical Engineering at AU
///				Herning.
/// \author		Christoffer Wesselhoff
/// \author		Kasper Jensen
/// \date		10/04-2020
/// \version	1.0
///////////////////////////////////////////////////////////////////////////////

#include"states.h"
#include"io.h"
#include<thread>
#include<chrono>
#include<iostream>
#include<string>
#include<string.h>
#include<sstream>
#include<cstdlib>
#include<sys/time.h>
#include<signal.h>

#define LIGHT 46			///< Kernel number for P8-16

using namespace std;

Gpio Light(LIGHT,"out");	///< Object for controlling internal lights.

int whichMachine();
///< Looks up the environment variable MACHINE created by the startup script
///< startup.sh and returns an integer representing the machine type
///< \return 0 If machine is Beaglebone Black.
///< \return 1 If machine is PocketBeagle.
///< \return -1 If there is an error.
bool configPin(string pin, string mode);
///< Configures pin to a specific mode of operation, by calling the
///< shell commands:
///
///  \code{.unparsed} configure-pin <pin> <mode>\endcode
///  \code{.unparsed} configure-pin -q <pin>\endcode
///
///< To see which modes a pin can be configured to, use the command:
///
///  \code{.unparsed} configure-pin -i <pin>\endcode
///
///< \param pin - Location of pin to configure.
///< \param mode - Mode which pin should be configured to.
///< \return true on success.
///< \return false on failure.
int testConnection(void);
///< Runs the internettest.sh script and returns the exit value.
///< \return 0 On sucess
///< \return -1 If there is an error.
void Sleep(int seconds);
///< Pauses the execution of the current thread for an amount of seconds.
///< \param seconds - Amount of seconds to sleep.
void readJSON(void);
///< Not implemented yet
///<
void Lights(onOff state);
///< Turns internal light on or off
///< \param state - on or off.
void Camera(onOff state);
///< Not implemented yet
///< \param state -
void CoolingFan(onOff state);
///< Not implemented yet
///< \param state -
void InitTimerInterrupt(int sec, int usec);
///< Initiates a timer which calls timerHandler with the interval specified.
///< \param sec - Amount of seconds to set the timer interval.
///< \param usec - Amount of microseconds to set the timer interval.
void timerHandler (int signum);
///< Timer handler, which which is executed every timer expires.
///< Should be changed to take measurements under test.
int CalculateSamples(int time);
///< Not implemented yet
///< \return nothing yet
void SetLoad(load testload);
///< Not implemented yet
///< \param testload -
void setupPID(void);
///< Not implemented yet
///<
void CalculateDutycycle(float voltage);
///< Not implemented yet
///< \param voltage -
void Test(void);
///< Not implemented yet
///<
void WriteJSON(void);
///< Not implemented yet
///<
void BlinkLed(led indication);
///< Not implemented yet
///< \param indication -

state powerOnState(void){
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
	//debug
	InitTimerInterrupt(0, 10000);
	if (testConnection() == 0) {
		return idle;
	}
	else {
		return error_connection;
	}
}

state idleState(void){
	while(1){

		if (testConfig.isRecieved)
			return start_test;
		else{
			Sleep(IDLE_WAIT);
			if (testConnection() != 0)
				return error_connection;
			// debug
			cout << "no test recieved" << endl;
		}
	}
}

state VoltageTestInit(void) {
	// Maybe input checks should be in the web interface
	if(testConfig.voltage >= 0 && testConfig.voltage <= 24) {
		CalculateDutycycle(testConfig.voltage);
		testConfig.isPID = false;
	}
	else {
		cout << "invalid voltage value:" << testConfig.voltage <<
				" (should be 0-24)" << endl;
	}

	return test;
}

state PIDTestInit(void) {
	// Set up PID prerequisites
	testConfig.isPID = true;
	return test;
}

state ErrorConnectionState(void) {
    int count = 0;
    while (1) {
        //BLINK LED PIN 3 TIMES
        Sleep(IDLE_WAIT);
        if (testConnection() == 0) {
            return idle;
        }
        else {
            count++;
            if (count == 3) {
            cout << "Still not resolved, rebooting now." << endl;
            system("reboot -h now");
            }
        }
    //debug
    cout << "Connection error" << endl;
    }
}

void ErrorPinState(void) {
	//debug
	cout << "Pin error" << endl;
}

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

int testConnection(void) {
	int ret = system(INTERNET_TEST);
	return WEXITSTATUS(ret);
}

void Sleep(int seconds) {
	this_thread::sleep_for(chrono::seconds(seconds));
}

void readJSON(void){
	//TO DO setup function for taking configuration from JSON file
	// fill in class named testConfig in states.h
	// isRecieved is flag for config recieved
	cout << "Reading config!" << endl;
}

void Lights(onOff state) {
	if(state == on)
		Light.write(1);
	else if(state == off)
		Light.write(0);
	//debug
	cout << "Fiddling with light!" << endl;
}

void Camera(onOff state) {
	//TO DO setup function to turn camera on or off with input as enum on or off
	cout << "Fiddling with camera!" << endl;
}

void CoolingFan(onOff state) {
	//TO DO setup function to turn cooling fan on or off with input as enum on or off
	cout << "Fiddling with cooling fan!" << endl;
}

void InitTimerInterrupt(int sec, int usec) {
	struct sigaction act;
	struct itimerval timer;
	memset (&act, '\0', sizeof(act));
	// Install a signal handler, set it to timerHandler
	act.sa_handler = &timerHandler;
	// Install the signal handler and relate it to the SIGALRM flag
	int ret = sigaction (SIGALRM, &act, NULL);
	if (ret)
	{
		cout << "timer signal handler failed to install" << endl;
		exit (EXIT_FAILURE);
	}
	// Configure timer to expire after sec.usec seconds
	timer.it_value.tv_sec = sec;
	timer.it_value.tv_usec = usec;
	// and every sec.usec after that
	timer.it_interval.tv_sec = sec;
	timer.it_interval.tv_usec = usec;
	//Start virtual timer, it counts down whenever this process is executing.
	ret = setitimer (ITIMER_REAL, &timer, NULL);
	if (ret) {
		cout << "Could not install timer" << endl;
		exit (EXIT_FAILURE);
	}
	cout << "timer installed successfully." << endl;
	//TO DO setup timer interrupt with 10ms time span
	cout << "Initialising Timer!" << endl;
}

void timerHandler (int signum) {
	if (signum == SIGALRM) {
		//debug
		Lights(on);
		Lights(off);
		cout << "Timer handler" << endl;
	}
}

int CalculateSamples(int time) {
	//TO DO calculate how many sets of samples we get from time
	//return numberOfSamples as integer
	return 1;
	cout << "Calculating Samples!" << endl;
}

void SetLoad(load testload){
	//TO DO set load specified by config
	cout << "Setting load!" << endl;
}

/////////////////////////////Pid Test Init/////////////////////////////////////

void setupPID(void){
	//TO DO setup pid regulation for motor with desired rpm
	cout << "setting rpm!" << endl;
}


///////////////////////////Voltage Test Init///////////////////////////////////

void CalculateDutycycle(float voltage) {
	//TO DO calculate Dutycycle to get RMS voltage level of desired voltage
	//24 VDC with 100% dutycycle
	//set PWM
	cout << "calculating duty cycle!" << endl;
}

/////////////////////////////Test//////////////////////////////////////////////

void Test(void){
	//TO DO setup test reading current, voltage, Temperature, ambient Temperature
	// & rpm for each timer interrupt.
	// increment counter for each sample set and end test once desired samples aquired
	// each set of samples are saved in a linked list consisting of structs
	cout << "Testing!" << endl;
}

///////////////////////////Return Results//////////////////////////////////////

void WriteJSON(void){
	//TO DO cycle through linked list with samples and write into JSON file
	// free memory of structs in linked list
	// set flag for test complete
	cout << "Returning Results!" << endl;
}

//////////////////////Error, Warning, Busy & OK////////////////////////////////

void BlinkLed(led indication){
	//TO DO setup blink of specified LED indication
	cout << "Blinking LED Indication!" << endl;
}

///////////////////////////////////////////////////////////////////////////////
