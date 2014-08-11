#ifndef __SERIAL_COMMUNICATOR_H__
#define __SERIAL_COMMUNICATOR_H__

#include <stdexcept>
#include <string>

#include "CommandFactory.h"
#include "Query.h"


class SerialCommunicator {
public:
	SerialCommunicator(std::string port, int baudRate, int byteSize, int stopBits, int parity);
	~SerialCommunicator(void);
	void connect(void) throw(std::runtime_error);
	bool connected(void) const;
	void disconnect(void) throw(std::runtime_error);
	void send(const Query query) throw(std::runtime_error);
	std::string plainRead(void) throw(std::runtime_error);
	std::string query(const Query, const long sleep=0L) throw(std::runtime_error);

private:
	std::string _port;
	int _baudRate;
	int _byteSize;
	int _stopBits;
	int _parity;
	int _connected;
	int _fd;
	size_t _sizeOfReadString;
};

#endif
