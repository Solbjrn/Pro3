///////////////////////////////////////////////////////////////////////////////
/// \file		io.cpp
///				Source file containing the implementations of the member
///				functions classes Adc, Pwm and Gpio and the derived classes
///				Measurement and DCmotor, and the functions used by the member 
///				functions. 
///				Used in the PRO4 projekt at Electrical Engineering at AU 
///				Herning. Designed to run on Beagle Bone Black.
/// \author		Christoffer Wesselhoff
/// \date		12/03-2020
/// \version	1.0
///////////////////////////////////////////////////////////////////////////////

#include "io.h"
#include<fstream>

int readFrom(string str);
///< Function to read from files
///< \param str - The path to the file read from.
///< \return Returns the value read from file. -1 on failure.
int writeTo(string str, string val);
///< Function to write to files, overloaded to accept both integer and
///< string input.
///< \param str - The path to the file written to.
///< \param val - The value written to the file.
///< \return Returns 0 on success -1 on failure.
int writeTo(string str, int val);
///< Function to write to files, overloaded to accept both integer and
///< string input.
///< \param str - The path to the file written to.
///< \param val - The value written to the file.
///< \return Returns 0 on success -1 on failure.

int readFrom(string str){
	int val = -1;
    ifstream file(str);
	// Initializes input file stream to the path specified.
    if (file.is_open()){
    	file >> val;
    	file.close();
	// If the file opens without error, read from file and close afterwards.
    }
    return val; 
	// Return the value read, or -1 if file opening failed.
}

int writeTo(string str, string val){
    ofstream fs(str);
	// Initializes output file stream to the path specified.
    if (fs.is_open()){
    	fs<<val;
    	fs.close();
    }
	// If the file opens without error, write to file and close afterwards.
    else {
    	return -1;
	// If the file is not opened, return -1.
    }
    return 0;
	// If everything goes as planned return 0.
}

int writeTo(string str, int val){
    ofstream fs(str);
	// Initializes output file stream to the path specified.
    if (fs.is_open()){
    	fs<<val;
    	fs.close();
    }
	// If the file opens without error, write to file and close afterwards.
    else {
    	return -1;
	// If the file is not opened, return -1.
    }
    return 0;
	// If everything goes as planned return 0.
}

Adc::Adc(int pin) {
	devicePath << path1 << pin << path2;
	// Combines the path, with the channel number specified.
}

int Adc::read() {
	return readFrom(devicePath.str());
	// Returns the raw value read from the path specified.
}

double Measurement::voltage() {
	return read()*(VREF/ADC_RESOLUTION)*(MOTORVOLTAGE/VREF);
	// Reads from ADC converts into 24V scale.
}

double Measurement::current() {
	return (read()*(VREF/ADC_RESOLUTION)*IDA_GAIN)/R_SHUNT;
	// Reads from ADC converts into current.
}

double Measurement::temperature() {
	return (read()*(VREF/ADC_RESOLUTION)*100)-TEMP_OFFSET;
	// Reads from ADC converts into celcius
}

Pwm::Pwm(int channel) {
	devicePath << path1;
	// Writes path1 to the stringstream devicePath.
	switch(channel) {
	case 0:
		devicePath << "0:0/";
		break;
		// If channel == 0 add path to PWM channel 0 to devicePath
	case 1:
		devicePath << "2:0/";
		break;
		// If channel == 1 add path to PWM channel 1A to devicePath
	case 2:
		devicePath << "4:1/";
		break;
		// If channel == 2 add path to PWM channel 1B to devicePath
	case 3:
		devicePath << "7:0/";
		break;
		// If channel == 3 add path to PWM channel 2A to devicePath
	case 4:
		devicePath << "7:1/";
		break;
		// If channel == 4 add path to PWM channel 2B to devicePath
	default:
		break;
	}

}

void Pwm::period(int hz) {
	devicePeriod = 1000000000/hz;
	// Converts value in Hz into value used by the PWM module
	writeTo(devicePath.str()+"period",devicePeriod);
	// Writes devicePeriod to period file in PWM module
}

void Pwm::dutyCycle(int dutyCycle) {
	dutyCycle = (devicePeriod/100)*dutyCycle;
	//Converts percentage into value based on period specified
	writeTo(devicePath.str()+"duty_cycle",dutyCycle);
	// Writes dutyCycle to duty cycle file in PWM module
}

void Pwm::polarity(string polarity) {
	writeTo(devicePath.str()+"polarity",polarity);
	// Writes polarity to polarity file in PWM module
}

void Pwm::start() {
	writeTo(devicePath.str()+"enable",1);
	// Writes 1 to enable file in PWM module
}

void Pwm::stop() {
	writeTo(devicePath.str()+"enable",0);
	// Writes 0 to enable file in PWM module
}

Pwm::~Pwm(){
	stop();
	// Stops PWM output.
}

DCmotor::DCmotor(int channel) : Pwm(channel){
	period(100);
	// Sets Period of 100Hz
	polarity("inversed");
	// Sets polarity to inversed.
}

Gpio::Gpio(int kernelNum,string direction) {
	devicePath << path1 << kernelNum;
	// Combines the path, with the pin number specified.
	writeTo(devicePath.str()+"/direction",direction);
	// Writes direction to direction file in PWM module
	deviceDirection = direction;
}

int Gpio::read() {
	if(deviceDirection == "in")
		return readFrom(devicePath.str()+"/value");
		// Reads the GPIO value
	else
		return -1;
		// Returns -1 if its not an input.
}

void Gpio::write(int value) {
	if(deviceDirection == "out")
		writeTo(devicePath.str()+"/value",value);
		// Writes the GPIO value
}

Gpio::~Gpio(){
	if(deviceDirection == "out")
		write(0);
		// If object was output, turn off
};
