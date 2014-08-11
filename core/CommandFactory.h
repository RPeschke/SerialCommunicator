#ifndef __COMMAND_FACTORY_H__
#define __COMMAND_FACTORY_H__

#include <array>
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>

#include "Query.h"


class CommandFactory {
public:
	CommandFactory(std::ifstream &commandConfigFile) throw(std::invalid_argument);
	Query generateQuery(const std::string &commandID) const throw(std::invalid_argument);
	std::string generateAnswer(const std::string &commandID, const std::string &answer) const throw(std::invalid_argument);

private:
	std::array<std::string, 4> split(const std::string &line) throw(std::invalid_argument);
	std::map<std::string, std::array<std::string, 3>> _commands;
};

#endif