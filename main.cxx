#include <cstdlib>
#include <iostream>
#include <string>

#include "SecoConfig.h"

#include "CommandFactory.h"
#include "PropertyReader.h"
#include "SerialCommunicator.h"


int main(void) {
	std::cout << "SeCo (version: " << seco_VERSION_MAJOR << "." << seco_VERSION_MINOR << ")" << std::endl;

	std::ifstream config("../ttiCPX400.config", std::ifstream::in);
	PropertyReader pr(config, "connection");
	std::string port = pr.get("port");
	int baudRate = std::stoi(pr.get("baudRate"));
	int characterSize = std::stoi(pr.get("characterSize"));
	bool stopBits = (pr.get("sendTwoStopBits").compare("true")) ? true : false;
	bool parity = (pr.get("enableParity").compare("true")) ? true : false;
	SerialCommunicator sc(port, baudRate, characterSize, stopBits, parity, 10);
	sc.connect();

	pr.setTitle("commands");

	Query query = CommandFactory::generateQuery(pr.get("setVoltage_ch1"), "0.042");
	sc.send(query);

	RichQuery richQuery(pr.get("getVoltage_ch1.command"), pr.get("getVoltage_ch1.regexp"), pr.get("getVoltage_ch1.pattern"));
	std::cout << "Voltage (channel 1): \'" << sc.query(richQuery) << "\' V" << std::endl;

	sc.disconnect();

	return EXIT_SUCCESS;
}
