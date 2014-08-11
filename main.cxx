#include <cstdlib>
#include <iostream>

#include "SecoConfig.h"



#include "CommandFactory.h"
#include "Query.h"
#include "PropertyReader.h"
#include "SerialCommunicator.h"
int main(void) {
	std::cout << "SeCo (version: " << seco_VERSION_MAJOR << "." << seco_VERSION_MINOR << ")" << std::endl;
	
	std::ifstream f("../ttiCPX400_commands.config", std::ifstream::in);
	CommandFactory cf(f);

	std::string port = "/dev/ttyACM0";
	int baudRate = 9600;
	int byteSize = 8;
	int stopBits = 0;
	int parity = 0;

	SerialCommunicator sc(port, baudRate, byteSize, stopBits, parity);
	sc.connect();
	Query query = cf.generateQuery("getVoltage_ch1");
	std::cout << "Voltage: \'" << sc.query(query, cf) << "\' V" << std::endl;
	sc.disconnect();

	return EXIT_SUCCESS;
}
