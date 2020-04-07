///////////////////////////////////////////////////////////////////////////////
/// \file		states.h
///				Header for the functions defining the states for the state 
///				machine used in the PRO4 projekt at Electrical Engineering at 
///				AU
///				Herning.
/// \author		Christoffer Wesselhoff
/// \author		Kasper Jensen
/// \date		07/04-2020
/// \version	1.0
///////////////////////////////////////////////////////////////////////////////

#ifndef STATES_H_
#define STATES_H_

/// Seconds to sleep in the idle state.
#define IDLE_WAIT 5
/// Path for internet test script.
#define INTERNET_TEST "/home/blob/internettest.sh"

/// enum value containing the possible states.
enum state{power_on, start_test, idle, error_pin, error_connection};
// When a config has been recieved, a class should be filled with parameters
static class config{
public:
	bool isRecieved = 1;
	bool isPID;
	float rpm;
	float voltage;
} testConfig;

/// Power on state, configures the pins needed in the project, and tests for
/// internet connection. \return enum value for the next state.
enum state powerOnState(void);
/// Idle state, checks if run configuration is recieved, if not, sleeps, then
/// tests for internet connection. \return enum value for the next state.
enum state idleState(void);
void ErrorConnectionState(void);
void ErrorPinState(void);



#endif /* STATES_H_ */
