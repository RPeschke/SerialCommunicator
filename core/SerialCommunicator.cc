#include <chrono>
#include <cstring>
#include <thread>
#include <iostream>

#ifdef WIN32
#define INVALID_PORT INVALID_HANDLE_VALUE
#include <windows.h>
#include <winbase.h>
#else
#define INVALID_PORT -1
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif

#include "CommandFactory.h"
#include "ExceptionFactory.h"
#include "Query.h"
#include "SerialCommunicator.h"

SerialCommunicator::SerialCommunicator(std::string port, int baudRate, int characterSize, bool sendTwoStopBits, bool enableParity, char commandTermination) :
_port(port),
_baudRate(baudRate),
_characterSize(characterSize),
_stopBits(sendTwoStopBits),
_parity(enableParity), 
_sizeOfReadString(100),
_sleep(0),
_connected(false),
_commandTermination(commandTermination) {
}

SerialCommunicator::~SerialCommunicator(void) throw(std::runtime_error) {
  if (_connected) {
    try{
      disconnect();
    }
    catch (...){
      std::cout << " Unable to disconnect the port " << std::endl;
    }
  }
}
void SerialCommunicator::connect(void) throw(std::runtime_error) {
#ifdef WIN32
  _fd = CreateFileA(_port.c_str(),
    GENERIC_READ | GENERIC_WRITE,
    0,
    0,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    0);// opening the com port 
#else
   _fd = open(_port.c_str(), (O_RDWR | O_NOCTTY) & ~O_NONBLOCK);
#endif
	
  
  
  if (_fd == INVALID_PORT) {
		throw std::runtime_error(ExceptionFactory::generateMessage("Could not open port \'" + _port + "\'!", "SerialCommunicator.cc", __LINE__));
	}
#ifdef WIN32
  // Setting Parameters
  DCB dcbSerialParams = { 0 };
  dcbSerialParams.DCBlength=sizeof(dcbSerialParams);
  if (!GetCommState(_fd, &dcbSerialParams)) {
    throw std::runtime_error(ExceptionFactory::generateMessage("!GetCommState(hSerial, &dcbSerialParams) is true","SerialCommunicator.cc" , __LINE__));
  }
  dcbSerialParams.BaudRate=_baudRate;
  dcbSerialParams.ByteSize=_characterSize;
    dcbSerialParams.StopBits=_stopBits;
    dcbSerialParams.Parity=_parity;
    if (!SetCommState(_fd, &dcbSerialParams)){
      //error setting serial port state
      throw std::runtime_error(ExceptionFactory::generateMessage("!SetCommState(hSerial, &dcbSerialParams) is true", "SerialCommunicator.cc", __LINE__));

    }
  
      // setting timeout condition
      COMMTIMEOUTS timeouts = { 0 };
      timeouts.ReadIntervalTimeout = 50;
      timeouts.ReadTotalTimeoutConstant = 50;
      timeouts.ReadTotalTimeoutMultiplier = 10;
      timeouts.WriteTotalTimeoutConstant = 50;
      timeouts.WriteTotalTimeoutMultiplier = 10;
      if (!SetCommTimeouts(_fd, &timeouts)){
        //error occurred. Inform user
        throw std::runtime_error(ExceptionFactory::generateMessage("!SetCommTimeouts(hSerial, &timeouts) is true", "SerialCommunicator.cc", __LINE__));

      }
    

#else 
	struct termios options; 
	tcgetattr(_fd, &options);

	try {
		cfsetispeed(&options, getBaudRate(_baudRate));
		options.c_cflag &= parityMask(_parity);
		options.c_cflag &= stopBitsMask(_stopBits);
		options.c_cflag &= ~CSIZE;						//clear current data size setting
		options.c_cflag |= characterSizeMask(_characterSize);
		options.c_cc[VMIN] = 2;							//minimum amount of characters to read
		options.c_cc[VTIME] = 10;						//amount of time to wait for amount of data specified in VMIN in tenths of a second
		options.c_cflag |= (CLOCAL | CREAD);			//don't allow changing of port control + enable the receiver
	} catch(std::invalid_argument &e) {
		throw std::runtime_error(ExceptionFactory::generateMessage("Error while connecting to port \'" + _port + "\' due to bad port settings:\n" + e.what(), "SerialCommunicator.cc", __LINE__));
	}

	if (tcsetattr(_fd, TCSANOW, &options) != 0) {
		throw std::runtime_error(ExceptionFactory::generateMessage("Error while connecting to port \'" + _port + "\'!", "SerialCommunicator.cc", __LINE__));
	}
#endif
	_connected = true;
}

bool SerialCommunicator::connected(void) const {
	return _connected;
}

void SerialCommunicator::disconnect(void) throw(std::runtime_error) {

#ifdef WIN32
  if (CloseHandle(_fd) == 0){
    throw std::runtime_error(ExceptionFactory::generateMessage("Error while disconnecting from port " + _port + "!", "SerialCommunicator.cc", __LINE__));
  }
#else
	if (!_connected || close(_fd)==-1) throw std::runtime_error(ExceptionFactory::generateMessage("Error while disconnecting from port " + _port + "!", "SerialCommunicator.cc", __LINE__));
#endif
	_connected = false;
}

void SerialCommunicator::send(const Query &query) throw(std::runtime_error) {
	if (_connected) {
		std::string command = query.command();
		command += _commandTermination;

#ifdef WIN32
    //////////////////////////////////////////////////////////////////////////
    // Windows Part
    DWORD dwBytesRead = 0;
    if (!WriteFile(_fd, command.c_str(), command.size(), &dwBytesRead, NULL)){
      throw std::runtime_error(ExceptionFactory::generateMessage("Couldn't send query \'" + query.command() + "\'!", "SerialCommunicator.cc", __LINE__));
    
    }
    // end Windows Part
    //////////////////////////////////////////////////////////////////////////
#else //Linux
    //////////////////////////////////////////////////////////////////////////
    // start Linux
    if (write(_fd, command.c_str(), command.size()) < 1){
      throw std::runtime_error(ExceptionFactory::generateMessage("Couldn't send query \'" + query.command() + "\'!", "SerialCommunicator.cc", __LINE__));
    }
    // end Linux
    //////////////////////////////////////////////////////////////////////////
#endif

	} else {
		throw std::runtime_error(ExceptionFactory::generateMessage("Couldn't send query because you are not connected!", "SerialCommunicator.cc", __LINE__));
	}
}

std::string SerialCommunicator::plainRead(void) throw(std::runtime_error) {
	if (!_connected) {
		throw std::runtime_error(ExceptionFactory::generateMessage("Couldn't read because there is no connection!", "SerialCommunicator.cc", __LINE__));
	}
	
	const int n = _sizeOfReadString; 
	char *buffer = new char[n+1];
	memset(buffer, 0, n+1);		//to see the end of the received string
#ifdef WIN32
  //////////////////////////////////////////////////////////////////////////
  // Windows Part
  DWORD dwByteRead = 0;
  if (!ReadFile(_fd, buffer, n, &dwByteRead, NULL)){
    delete [] buffer;
    throw std::runtime_error(ExceptionFactory::generateMessage("Reading port \'" + _port + "\' leads to no output!", "SerialCommunicator.cc", __LINE__));
  }
  // end Windows Part
  //////////////////////////////////////////////////////////////////////////
#else //Linux
  //////////////////////////////////////////////////////////////////////////
  // start Linux
  int bytes = 0;
  if (read(_fd, buffer, n) < 1) {
    delete[] buffer;
    throw std::runtime_error(ExceptionFactory::generateMessage("Reading port \'" + _port + "\' leads to no output!", "SerialCommunicator.cc", __LINE__));
  }
  // end Linux
  //////////////////////////////////////////////////////////////////////////
#endif



	std::string output(buffer);
	delete[] buffer;

	return output;
}

std::string SerialCommunicator::query(const RichQuery &query) throw(std::runtime_error) {
	send(query);
	std::this_thread::sleep_for(std::chrono::milliseconds(_sleep));
	std::string plain = plainRead();
	std::string answer = CommandFactory::generateAnswer(plain, query.regexp(), query.pattern());
	
	return answer;
}

void SerialCommunicator::defaultSleep(const long milliseconds) {
	_sleep = milliseconds;
}

int SerialCommunicator::getBaudRate(int baudRate) throw(std::invalid_argument) {
#ifdef WIN32
  return baudRate;
#else

  switch (baudRate) {
		case 50: return B50;
		case 75: return B75;
		case 110: return B110;
		case 134: return B134;
		case 150: return B150;
		case 200: return B200;
		case 300: return B300;
		case 600: return B600;
		case 1200: return B1200;
		case 1800: return B1800;
		case 2400: return B2400;
		case 4800: return B4800;
		case 9600: return B9600;
		case 19200: return B19200;
		case 38400: return B38400;
		default: throw std::invalid_argument(ExceptionFactory::generateMessage("Bad baud rate: " + std::to_string(baudRate) + "!", "SerialCommunicator.cc", __LINE__));
	}
#endif
}

int SerialCommunicator::characterSizeMask(int characterSize) throw(std::invalid_argument) {
#ifdef WIN32
  return 1;
#else

  switch (characterSize) {
		case 5: return CS5;
		case 6: return CS6;
		case 7: return CS7;
		case 8: return CS8;
		default: throw std::invalid_argument(ExceptionFactory::generateMessage("Bad character size: " + std::to_string(characterSize) + "!", "SerialCommunicator.cc", __LINE__));
	}
#endif
}

int SerialCommunicator::stopBitsMask(bool stopBits) throw(std::invalid_argument) {
#ifdef WIN32
  return 1;
#else



  if (stopBits) return CSTOPB;
	else return ~CSTOPB;
#endif 
}
int SerialCommunicator::parityMask(bool parity) throw(std::invalid_argument) {
#ifdef WIN32
  return 1;
#else

	if (parity) return PARENB;
	else return ~PARENB;
#endif

}
