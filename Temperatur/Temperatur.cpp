#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<ctime>

using namespace std;

#define LDR_PATH "/sys/bus/iio/devices/iio:device0/in_voltage"

float readAnalog(float number) {
	stringstream ss;
	ss << LDR_PATH << number << "_raw";
	fstream fs;
	fs.open(ss.str().c_str(), fstream::in);
	fs >> number;
	fs.close();
	return number;
}

int main(int argc, char *argv[]) {
	ofstream outfile;
	outfile.open("log.txt", ios::ate | ios::app);

	std::string textOut;
	int i = 0;

	cout << "Starting the program" << endl;
	for (i = 0; i < 2; i++) {
		float value = readAnalog(i + 5);
		value = value / 10; //omdanner værdien fra mV til Celtius
		switch (i) {
		case '0': {
			//output logic for sensor 1
			value = value - 5.4;
			break;
		}
		case '1': {
			//output logic for sensor 2
			value = value - 3.7;
			break;
		}
		}
		time_t now = time(0);
		char *dt = ctime(&now);
		cout << "The temperature of element " << i << " was " << value << "C"
				<< endl;
		outfile << "The temperature of element " << i << " was " << value
				<< "C " << dt << endl;
	}
	outfile.close();
}
