///////////////////////////////////////////////////////////////////////////////
/// \file		startup.h
///				Header file containing the functions configPin and whichMachine
///				used in the PRO4 projekt at Electrical Engineering at AU
///				Herning.
/// \author		Christoffer Wesselhoff
/// \author		Kasper Jensen
/// \date		27/03-2020
/// \version	1.0
///////////////////////////////////////////////////////////////////////////////

#ifndef STARTUP_H_
#define STARTUP_H_

#include<iostream>
#include<string>
#include<sstream>
#include<cstdlib>


using namespace std;

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



#endif /* STARTUP_H_ */
