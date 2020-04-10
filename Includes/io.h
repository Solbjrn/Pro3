///////////////////////////////////////////////////////////////////////////////
/// \file		io.h
///				Header file containing the classes ADC, PWM and GPIO and the
///				derived classes Voltage, Current, Temperature and DCmotor used
///				in the PRO4 projekt at Electrical Engineering at AU Herning.
///				Designed to run on Beagle Bone Black.
/// \author		Christoffer Wesselhoff
/// \date		12/03-2020
/// \version	1.0
///////////////////////////////////////////////////////////////////////////////

#include<string>
#include<sstream>

#define VREF 1.8			///< Refrence voltage of the ADC
#define ADC_RESOLUTION 4096 ///< Resolution of the ADC
#define MOTORVOLTAGE 24		///< Voltage used by the DC motor
#define TEMP_OFFSET 50		///< Offset in celcius of temperature sensor used.
#define IDA_GAIN 5			///< Gain of the amplifier used in current circuit.
#define R_SHUNT 0.05		///< Value of resistor used in current circuit.

using namespace std;

#ifndef IO_H_
#define IO_H_

/// The Adc class. Constructor creates object, which is tied to an ADC channel.
/// Has one function that reads from the ADC channel specified by constructor.
///
/// The following table shows correlation between the pin location, ADC 
/// channel name and the number used in the ADC constructor.
///Pin Location | PWM Channel name	| Constructor channel number
///------------	| ----------------	| --------------------------
///P9-39  		| AIN0	 			| 0
///P9-40  		| AIN1	 			| 1
///P9-37		| AIN2		   		| 2
///P9-38		| AIN3				| 3 
///P9-33		| AIN4				| 4 
///P9-36		| AIN5				| 5
///P9-35		| AIN6				| 6
///	
class Adc {
protected:
	string path1 = "/sys/bus/iio/devices/iio:device0/in_voltage"; 
	///< First part of path to ADC.
	string path2 = "_raw";	
	///< Second part of path to ADC.
	stringstream devicePath;
	///< Stringstream containing the full path created by the constructor.
public:
	explicit Adc(unsigned int channel);
	///< Constructor. Completes the full path for one of the 7 ADC channels on
	///< the pocketBeagle \param channel - An integer between 0 and 6 
	///< representing the ADC channel.
	int read();
	///< Reads from the channel specified by the constructor \return  Raw ADC 
	///< value between 0 - 4096
};

/// A child of the Adc class, contains functions that calculates the voltage,
///  from the raw ADC input.
///
/// For use on the Voltage- and Current measuring circuits and the Temperature
/// sensors in the PRO4 project.
///
/// For more information on the functions see the Adc class.
class Voltage: public Adc {	
public: 
	/// Constructor. Same as the the Adc constructor \param channel - An 
	///< integer between 0 and 6 representing the ADC channel.						
	Voltage(unsigned int channel) : Adc(channel){};
	double measure(); 		
	///< Takes a measurement from the ADC converts the raw value to the voltage
	///< that was stepped down by the measuring circuit. 
	///<
	///< The equation used to calculate voltage
	///< \f{eqnarray*}{ Voltage &=& Raw\_ADC \times \frac {VREF}
	///			{ADC\_RESOLUTION} \times \frac {MOTOR\_VOLTAGE}{VREF}
	///						\\ \\&=& Raw\_ADC \times \frac {1.8} {4096}\times
	///								\frac {24}{1.8}\f}
	///< \return Voltage in volts between 0 - 24V. Measured by the voltage
	///  circuit.
};

/// A child of the Adc class, contains functions that calculates the
///  current, from the raw ADC input.
///
/// For use on the Voltage- and Current measuring circuits and the Temperature
/// sensors in the PRO4 project.
///
/// For more information on the functions see the Adc class.
class Current: public Adc {	
public: 
	/// Constructor. Same as the the Adc constructor \param channel - An 
	///< integer between 0 and 6 representing the ADC channel.						
	Current(unsigned int channel) : Adc(channel){};
	double measure();		
	///< Takes a measurement from the ADC and converts the raw value to the
	///< current drawn by the DC motor. 
	///<
	///< The equation used to calculate current
	///< \f{eqnarray*}{ Current &=& \frac {Raw\_ADC \times \frac {VREF}
	///						{ADC\_RESOLUTION} \times IDA\_GAIN} {R\_SHUNT}
	///							 \\ \\  &=& \frac {Raw\_ADC \times \frac {1.8}
	///								{4096} \times 5} {0.05}\f}
	///< \return Current in ampere Measured by the current circuit.
};

/// A child of the Adc class, contains functions that calculates the
///  temperature, from the raw ADC input.
///
/// For use on the Voltage- and Current measuring circuits and the Temperature
/// sensors in the PRO4 project.
///
/// For more information on the functions see the Adc class.
class Temperature: public Adc {	
public: 
	/// Constructor. Same as the the Adc constructor \param channel - An 
	///< integer between 0 and 6 representing the ADC channel.						
	Temperature(unsigned int channel) : Adc(channel){};
	double measure();	
	///< Takes a measurement from the ADC and calculates the temperature
	///< measured by the temperature sensor.
	///<
	///< The equation used to calculate temperature.
	///<\f{eqnarray*}{ Temperature(C^o) &=& \frac {Raw\_ADC \times VREF \times  
	///								100}{ADC\_RESOLUTION} - TEMP\_OFFSET
	///							 \\ \\  &=& \frac {Raw\_ADC \times 1.8 \times 
	///								100} {4096}- 50\f}
	///< \return Temperature in celcius measured by the temperature sensor.
};

/// The Pwm class. Constructor creates object, which is tied to an PWM channel.
/// Has one function that reads from the PWM channel specified by constructor.
///
/// \attention In order to enable pwm on the Beagle Bone Black, you must 
/// configure the pin to pwm before running the program using the shell commands:
//
///  \code{.unparsed} configure-pin <PIN_LOCATION> pwm\endcode
///  \code{.unparsed} configure-pin -q <PIN_LOCATION>\endcode
///
/// The following table shows correlation between the pin location, PWM 
/// channel name and the number used in the PWM constructor.
///Pin Location | PWM Channel name	|Constructor channel number
///------------	| ----------------	|--------------------------
///P9-42  		| ECAPPWM0 			| 0
///P9-14  		| EHRPWM1A 			| 1
///P9-16		| EHRPWM1B   		| 2
///P8-19		| ERHPWM2A			| 3 
///P8-13		| ERHPWM2B			| 4 
///	
class Pwm {
	int devicePeriod = 0;
	///< Period of the PWM channel initialized as 0.
	string path1 = "/sys/class/pwm/pwm-";
	///< First part of path to PWM.
	stringstream devicePath;
	///< Stringstream containing the full path created by the constructor.
public:
	/// Constructor. Completes the full path for one of the 5 PWM channels on
	///< the pocketBeagle \param channel - An integer between 0 and 4
	///< representing the PWM channel.
	explicit Pwm(unsigned int channel);
	virtual ~Pwm();
	///< \details Deconstructor. Turns off output.

	void period(int hz);
	///< Sets the period of the PWM channel specified by the constructor.
	///< \attention Set this value before setting duty cycle. As duty cycle
	///< is based on the period
	///< \param hz - A value between 1Hz - 1,000,000,000Hz.
	void dutyCycle(int cycle);
	///< Sets the duty cycle of the PWM channel specified by the constructor.
	///< \attention Set period before setting duty cycle. As duty cycle
	///< is based on the period
	///< \param cycle - A value between 0% - 100%
	void polarity(string polarity);
	///< Sets the polarity of the PWM channel specified by the constructor.
	///< \param polarity - `normal` - Normal polarity\n - `inversed` - Inverted
	///< polarity
	void start();
	///< \details Starts the PWM channel specified by the constructor.
	void stop();
	///< \details Stops the PWM channel specified by the constructor.
};

/// A child of the Pwm class, contains the same functions, but the constructor
/// sets up the period an polarity for the DC motor used in the project.
///
/// For use on the DC motor in the PRO4 project.
///
/// For more information on the functions see the Pwm class.
class DCmotor: public Pwm {
public:
	/// Constructor. Completes the full path for one of the 5 PWM channels on
	///< the pocketBeagle. Sets the period to 100Hz and the polarity to
	///< inverted \param channel - An integer between 0 and 4 representing the
	///< PWM channel.
	DCmotor(unsigned int channel);
};

/// The Gpio class. Constructor creates object, which is tied to a GPIO pin.
/// Has two functions, one that reads from the pin specified by constructor,
/// if it's sat up as input, and one that writes to the pin specified by
/// constructor. if it is sat up as output. 
///
/// The following table shows correlation between the pin location and the 
/// kernel number used in the GPIO constructor.
///Pin Location | GPIO Kernel number	
///------------	| ------------------	
///P9-12  		| 60 			
///P9-15  		| 48 			   		
///P9-23		| 49			
///P9-25		| 117
///P9-27  		| 115 			
///P9-30  		| 112 			
///P9-41		| 20   		
///P8-07		| 66			
///P8-08		| 67
///P8-09		| 69			
///P8-10		| 68 		
///P8-11		| 45			
///P8-12		| 44
///P8-14		| 26 		
///P8-15		| 47			
///P8-16		| 46
///P8-17		| 27 		
///P8-18		| 65			
///P8-26		| 61			
///	
class Gpio {
	string path1 = "/sys/class/gpio/gpio";
	///< First part of path to GPIO.
	stringstream devicePath;
	///< Stringstream containing the full path created by the constructor.
	string deviceDirection;
	///< String containing the Gpio objects direction.
public:
	/// Constructor. Completes the full path for one of the GPIO pins on
	///< the pocketBeagle 
	///< \param kernelNum - An integer representing the kernel number of the
	/// GPIO pin
	///< \param direction - `out` - Sets the GPIO pin up as output.\n - `in`  -
	///< Sets the GPIO pin up as input.
	Gpio(int kernelNum, string direction);
	virtual ~Gpio();
	///< \details Deconstructor. if Gpio object direction was output, sets
	///< output to 0.
	
	int read();
	///< Reads from the pin specified by the constructor.
	///< \return pin value, 0 or 1.
	void write(int value);
	///< Sets the pin value specified by the constructor
	///< \param value - A binary value 0 or 1.
};

#endif /* IO_H_ */
