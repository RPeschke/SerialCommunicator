#include <cstdlib>
#include <iostream>
#include <string>

#include "SecoConfig.h"



#include "CommandFactory.h"
#include "Query.h"
#include "PropertyReader.h"
#include "SerialCommunicator.h"
#include <termios.h>
int main(void) {
	std::cout << "SeCo (version: " << seco_VERSION_MAJOR << "." << seco_VERSION_MINOR << ")" << std::endl;

	std::ifstream config("../ttiCPX400.config", std::ifstream::in);
	PropertyReader pr(config);
	std::string port = pr.get("port");
	int baudRate = std::stoi(pr.get("baudRate"));
	int characterSize = std::stoi(pr.get("characterSize"));
	bool stopBits = (pr.get("sendTwoStopBits").compare("true")) ? true : false;
	bool parity = (pr.get("enableParity").compare("true")) ? true : false;

	std::ifstream commands("../ttiCPX400_commands.config", std::ifstream::in);
	CommandFactory cf(commands);

	SerialCommunicator sc(port, baudRate, characterSize, stopBits, parity);
	sc.connect();
	Query query = cf.generateQuery("getVoltage_ch1");
	std::cout << "Voltage: \'" << sc.query(query, cf) << "\' V" << std::endl;
	sc.disconnect();

	return EXIT_SUCCESS;
}
