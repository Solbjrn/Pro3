///////////////////////////////////////////////////////////////////////////////
/// \file		states.h
///				Header file containing the functions configPin and whichMachine
///				used in the PRO4 projekt at Electrical Engineering at AU
///				Herning.
/// \author		Christoffer Wesselhoff
/// \author		Kasper Jensen
/// \date		10/04-2020
/// \version	1.0
///////////////////////////////////////////////////////////////////////////////

#ifndef STATES_H_
#define STATES_H_

#define IDLE_WAIT 5
///< Time to wait in idle state before checking if configuration is received.
#define INTERNET_TEST "/home/blob/internettest.sh"
///< Path to internettest.sh script on filesystem.

/// Possible states for the finite state machine in the main function.
///
enum state{
	power_on, 			///< Powering up state.
	start_test,			///< Start test state.
	idle,				///< Idle state.
	error_pin,			///< Pin configuration error state.
	error_connection,	///< Internet connection error state
	voltage_test_init,	///< %Voltage test init state.
	test				///< Test stage.
};

/// Possible states for the Light, cooling fan and camera.
///
enum onOff{
	on,					///< Device is on.
	off					///< Device is off.
};

/// Different levels of load.
///
enum load{
	minimum,			///< Minimum load.
	medium,				///< Medium load.
	maximum				///< Maximum load.
};

/// Possible states for status led.
///
enum led{
	error,				///< Error state.
	warning,			///< Warning state.
	ok,					///< Ok state.
	busy				///< Busy state.
};

/// Class to hold parameters from configuration received.
///
class Config{
public:
	bool isRecieved = 1;
	///< Flag to tell if configuration has been received.
	bool isPID;
	///< Flag to tell if configuration is PID controlled or Voltage controlled.
	float rpm;
	///< If isPID is true, this should contain the desired speed.
	float voltage;
	///< If isPID is false, this should contain the desired speed.
	int timeInSeconds;
	///< Amount of seconds the test should run.
}static testConfig;
///< Object of the Config class.
///<

state powerOnState(void);
///< State for powering up device, configures pins and tests internet
///< connection. Returns next state.
///< \return idle if pins were correctly setup, and internet is
///< available.
///< \return error_pin if pins were incorrectly setup.
///< \return error_connection if internet is not available.

state idleState(void);
///< State for powering up device, configures pins and tests internet
///< connection. Returns next state.
///< \return idle if pins were correctly setup, and internet is
///< available.
///< \return error_pin if pins were incorrectly setup.
///< \return error_connection if internet is not available.
state voltageTestInit(void);
///< State for initializing voltage test.
///< Calculates dutycycle and sets next stage to test.
///< \return test
state ErrorConnectionState(void);
///< State for powering up device, configures pins and tests internet
///< connection. Returns next state.
///< \return idle if pins were correctly setup, and internet is
///< available.
///< \return error_pin if pins were incorrectly setup.
///< \return error_connection if internet is not available.
void ErrorPinState(void);
///< Not implemented yet
///<

#endif /* STATES_H_ */
