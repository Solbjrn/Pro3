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

#define MAXTIME 300					///< Max time for test in seconds.
#define MAXSAMPLES	MAXTIME*100		///< Max number of samples in a test.
#define LIGHT 46					///< Kernel number for P8-16
#define FAN 87 						///< Find nummer for P1_2
#define LOAD1 89 					///< Find nummer for P1_4
#define LOAD2 5 					///< Find nummer for P1_6
#define ERRORLED 2 					///< Find nummer for P1_8
#define WARNINGLED 3 				///< Find nummer for P1_10
#define OKLED 4 					///< Find nummer for P1_12
#define BUSYLED 20 					///< Find nummer for P1_20

using namespace std;

volatile unsigned int testCounter;	///< Counter to keep track of sampling
volatile unsigned int testSamples;	///< Holds amount of samples in test
volatile bool testRunning;			///< True while test is running.

Voltage vol(1);						///< Voltage measuring object P9-40.
Current curr(3);					///< Current measuring object P9-38.
Temperature temp(2);				///< Temperature measuring object P9-37.
Result results[MAXSAMPLES];			///< Array of result structs.

Gpio Light(LIGHT,"out");			///< Object for controlling internal lights.
Gpio CoolingFan(FAN, "out");		///< Object for controlling cooling fan.
Gpio Load1(LOAD1, "out");			///< Object for controlling Load.
Gpio Load2(LOAD2, "out");			///< Object for controlling Load.
Gpio ErrLed(ERRORLED, "out");		///< Object for controlling error LED.
Gpio WarnLed(WARNINGLED, "out");	///< Object for controlling warning LED.
Gpio OKLed(OKLED, "out");			///< Object for controlling ok LED.
Gpio BusyLed(BUSYLED, "out");		///< Object for controlling busy LED.

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
void CoolingFAN(onOff state);
///< Turns cooling fan on or off
///< \param state - on or off.
void InitTimerInterrupt(int sec, int usec);
///< Initiates a timer which calls timerHandler with the interval specified.
///< Set time to 0 to Stop timer.
///< \param sec - Amount of seconds to set the timer interval.
///< \param usec - Amount of microseconds to set the timer interval.
void timerHandler (int signum);
///< Timer handler, which which is executed every timer expires.
///< Takes Voltage, Current and Temperature measurements under test.
int CalculateSamples(int time);
///< Converts time in seconds for testing to number of samples to collect.
///<
///< The equation used to calculate samples:
///< \f{eqnarray*}{ Samples &=& time \times 100\f}
///< \param time - Duration of test in seconds.
///< \return number of samples calculated to collect in test.
void SetLoad(load testload);
///< Toggles GPIO pins for desired load level.
///< \param testload - minimum: Turns off both pins.
///< medium: Turns on one pin and other off.
///< maximum: Turns both pins on.
void setupPID(void);
///< Not implemented yet
///<
float CalculateDutycycle(float voltage);
///< Calculates dutycycle of pwm for motor, based on desired voltage.
///< The equation used to calculate the dutycycle:
///<\f{eqnarray*}{ Dutycycle &=& \frac {voltage} {MOTORVOLTAGE} \times 100
///							 \\ \\  &=& \frac {voltage} {24} \times 100 \f}
///< \param voltage - Desired voltage level
///< \return Dutycycle calculated.
void WriteJSON(void);
///< Not implemented yet
///<
void BlinkLed(led indication);
///< Blink led indication 3 times, led is chosen by the indication argument.
///< \param indication - ok, busy, warning or error.

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

state TestState(void){
	testSamples = CalculateSamples(testConfig.timeInSeconds);
	memset (results, '\0', sizeof(results));
	testCounter = 0;
	testRunning = true;
	InitTimerInterrupt(0, 10000);
	cout << "Testing!" << endl;
	while(testRunning) {
		// Want to do something while testing?
	}
	cout << "Test Ended" << endl;
	return return_results;
}

state ReturnResultsState(void){
	//debug
	for(unsigned int i = 0; i<=testSamples;i++){
		cout << results[i].voltage << " " << results[i].current << " "
			 << results[i].temperature << " " << results[i].timestamp << endl;
	}
	return error_pin;
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

void CoolingFAN(onOff state) {
	cout << "Fiddling with cooling fan!" << endl;
	if (state == on) {
		CoolingFan.write(1);
		std::cout << "Fan is on!" << std::endl;
	}
	if (state == off) {
		CoolingFan.write(0);
		std::cout << "Fan is off!" << std::endl;
	}
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
		cout << "Timer handler" << endl;
		results[testCounter].voltage = vol.measure();
		results[testCounter].current = curr.measure();
		results[testCounter].temperature = temp.measure();
		results[testCounter].timestamp = testCounter*0.01;
		if(testCounter == testSamples) {
			InitTimerInterrupt(0, 0);
			testRunning = false;
		}
		testCounter++;
	}
}

int CalculateSamples(int time) {
	cout << "Calculating Samples!" << endl;

	if (time <= 0) {
		std::cout << "Invalid value for time!" << std::endl;
		return -1;
	} else {
		std::cout << "Time selected in seconds is: " << time << " seconds"
				<< std::endl;
		const int SAMPLINGTIME = 100;
		int NumberOfSamples = 0;

		//Samples = time/0.01 or Samples = time*100
		NumberOfSamples = time * SAMPLINGTIME;
		std::cout << "Number of samples to collect: " << NumberOfSamples << std::endl;

		//return int containing desired number of samples
		return NumberOfSamples;
	}
}

void SetLoad(load testload) {
	cout << "Setting load!" << endl;
	if (testload == minimum) {
		Load1.write(0);
		Load2.write(0);
		std::cout << "Minimal load!" << std::endl;
	} else if (testload == medium) {
		Load1.write(1);
		Load2.write(0);
		std::cout << "Medium load!" << std::endl;
	} else if (testload == maximum) {
		Load1.write(1);
		Load2.write(1);
		std::cout << "Maximum load!" << std::endl;
	}
}

/////////////////////////////Pid Test Init/////////////////////////////////////

void setupPID(void){
	//TO DO setup pid regulation for motor with desired rpm
	cout << "setting rpm!" << endl;
}


///////////////////////////Voltage Test Init///////////////////////////////////

float CalculateDutycycle(float voltage) {
	cout << "calculating duty cycle!" << endl;
	float Dutycycle = 0;
	// Maybe input check should be performed in the webinterface
	if (voltage <= 0 || voltage > 24) {
		std::cout << "Invalid voltage level must be above 0 and 24 or below!"
				<< std::endl;
		return -1;
	} else {
		Dutycycle = (voltage / MOTORVOLTAGE) * 100;
		std::cout << "Dutycycle calculated to: " << Dutycycle << " %"
				<< std::endl;
		return Dutycycle;
	}
}

///////////////////////////Return Results//////////////////////////////////////

void WriteJSON(void){
	//TO DO cycle through linked list with samples and write into JSON file
	// free memory of structs in linked list
	// set flag for test complete
	cout << "Returning Results!" << endl;
}

//////////////////////Error, Warning, Busy & OK////////////////////////////////

void BlinkLed(led indication) {

	cout << "Blinking LED Indication!" << endl;
	if (indication == error) {
		std::cout << "ERROR" << std::endl;
		for (int i = 0; i < 3; i++) {
			ErrLed.write(1);
			Sleep(1);
			ErrLed.write(0);
			Sleep(1);
		}
	} else if (indication == warning) {
		std::cout << "WARNING" << std::endl;
		for (int i = 0; i < 3; i++) {
			WarnLed.write(1);
			Sleep(1);
			WarnLed.write(0);
			Sleep(1);
		}
	} else if (indication == ok) {
		std::cout << "OK" << std::endl;
		for (int i = 0; i < 3; i++) {
			OKLed.write(1);
			Sleep(1);
			OKLed.write(0);
			Sleep(1);
		}
	} else if (indication == busy) {
		std::cout << "BUSY" << std::endl;
		for (int i = 0; i < 3; i++) {
			BusyLed.write(1);
			Sleep(1);
			BusyLed.write(0);
			Sleep(1);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
