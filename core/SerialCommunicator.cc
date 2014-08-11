#include <SerialCommunicator.h>

#include <chrono>
#include <cstring>
#include <thread>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "ExceptionFactory.h"


SerialCommunicator::SerialCommunicator(std::string port, int baudRate, int byteSize, int stopBits, int parity) :
_port(port),
_baudRate(baudRate),
_byteSize(byteSize),
_stopBits(stopBits),
_parity(parity), 
_sizeOfReadString(100) {
}

SerialCommunicator::~SerialCommunicator(void) {
	if (_connected) {
		disconnect();
	}
}

void SerialCommunicator::connect(void) throw(std::runtime_error) {
	_fd = open(_port.c_str(), (O_RDWR | O_NOCTTY) & ~O_NONBLOCK);
	if (_fd < 0) {
		throw std::runtime_error(ExceptionFactory::generateMessage("Could not open port \'" + _port + "\'!", "SerialCommunicator.cc", __LINE__));
	}

	struct termios options; 
	tcgetattr(_fd, &options);			//get the current settings of the serial port
	cfsetispeed(&options, B9600);			//set read and write speed to 19200 BAUD
	options.c_cflag &= ~PARENB;				//set no parity
	options.c_cflag &= ~CSTOPB;				//set one stop bit
	options.c_cflag &= ~CSIZE;				//clear current data size setting
	options.c_cflag |= CS8;					//set 8 bit per work
	options.c_cc[VMIN] = 2;					//minimum amount of characters to read
	options.c_cc[VTIME] = 10;				//amount of time to wait for amount of data specified in VMIN in tenths of a second
	options.c_cflag |= (CLOCAL | CREAD);	//don't allow changing of port control + enable the receiver

	if (tcsetattr(_fd, TCSANOW, &options) != 0) {
		throw std::runtime_error(ExceptionFactory::generateMessage("Error while connecting to port " + _port + "!", "SerialCommunicator.cc", __LINE__));
	}

	_connected = true;
}

bool SerialCommunicator::connected(void) const {
	return _connected;
}

void SerialCommunicator::disconnect(void) throw(std::runtime_error) {
	if (!_connected || close(_fd)==-1) throw std::runtime_error(ExceptionFactory::generateMessage("Error while disconnecting from port " + _port + "!", "SerialCommunicator.cc", __LINE__));

	_connected = false;
}

void SerialCommunicator::send(const Query query) throw(std::runtime_error) {
	if (_connected) {
		std::string command = query.command();
		command += 10;		//line feed
		if (write(_fd, command.c_str(), command.size()) < 1) {
			throw std::runtime_error(ExceptionFactory::generateMessage("Couldn't send query \'" + query.command() + "\'!", "SerialCommunicator.cc", __LINE__));
		}
	} else {
		throw std::runtime_error(ExceptionFactory::generateMessage("Couldn't send query because you are not connected!", "SerialCommunicator.cc", __LINE__));
	}
}

std::string SerialCommunicator::plainRead(void) throw(std::runtime_error) {
	if (!_connected) {
		throw std::runtime_error(ExceptionFactory::generateMessage("Couldn'nt read because there is no connection!", "SerialCommunicator.cc", __LINE__));
	}
	
	const int n = _sizeOfReadString; 
	char *buffer = new char[n+1];
	memset(buffer, 0, n+1);		//to see the end of the recieved string

	if (read(_fd, buffer, n) < 1) {
		delete[] buffer;
		throw std::runtime_error(ExceptionFactory::generateMessage("Reading port \'" + _port + "\' leads to no output!", "SerialCommunicator.cc", __LINE__));
	}

	std::string output(buffer);
	delete[] buffer;

	return output;
}

std::string SerialCommunicator::query(const Query query, const long sleep /*=0L*/) throw(std::runtime_error) {
	send(query);
	std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
	std::string plain = plainRead();

	_cf.

	return plain;
}
