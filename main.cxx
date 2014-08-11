#include <cstdlib>
#include <iostream>

#include "SecoConfig.h"



#include "CommandFactory.h"
#include "Query.h"
#include "PropertyReader.h"
#include "SerialCommunicator.h"
int main(void) {
	std::cout << "SeCo (version: " << seco_VERSION_MAJOR << "." << seco_VERSION_MINOR << ")" << std::endl;
	
	std::ifstream f("../testCommands.config", std::ifstream::in);
	CommandFactory cf(f);

//	std::cout << cf.generateAnswer("testCommand", "foo42bar13palim0") << std::endl;
//	std::ifstream f("../test.config", std::ifstream::in);
//	PropertyReader pr(f);
//	if (pr.contains("port")) {
//		std::cout << pr.get("port", "foobar") << std::endl;
//	} else {
//		std::cout << "\'port\' does not exists" << std::endl;
//	}

	std::string port = "/dev/ttyACM0";
	int baudRate = 9600;
	int byteSize = 8;
	int stopBits = 0;
	int parity = 0;

	SerialCommunicator sc(port, baudRate, byteSize, stopBits, parity);
	sc.connect();
	Query query("V1?", "V1 [0-9]*.[0-9]*");
	std::cout << "Answer: " << sc.query(query) << std::endl;
	sc.disconnect();

	return EXIT_SUCCESS;
}
