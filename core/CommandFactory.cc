#include "CommandFactory.h"

#include <regex>

#include "ExceptionFactory.h"


CommandFactory::CommandFactory(std::ifstream &commandConfigFile) throw(std::invalid_argument) {
	if (!commandConfigFile.good()) {
		throw std::invalid_argument(ExceptionFactory::generateMessage("Cannot read command config file. Ensure that the file exists, is open and readable!", "CommandFactory.cc", __LINE__));
	}

	bool qmark = false;
	std::string line("");
	_commands.clear();
	for (char c=commandConfigFile.get(); commandConfigFile.good(); c=commandConfigFile.get()) {
		if (!qmark && c=='\n') {
			std::array<std::string, 4> values = split(line + ';');
			std::array<std::string, 3> x = {values[1], values[2], values[3]};
			_commands.insert(std::pair<std::string, std::array<std::string, 3>>(values[0], x));
			line = "";
		} else {
			if (c == '\"') qmark = !qmark;
			line += c;
		}
	}

	commandConfigFile.close();
}

std::array<std::string, 4> CommandFactory::split(const std::string &line) throw(std::invalid_argument) {
	bool qmark = false;
	std::string x("");
	int c = 0;
	std::array<std::string, 4> values;
	for (size_t i=0; i<line.length(); i++) {
		if ((!qmark && line[i]=='\n') || c>3) break;
		if (line[i] == '\"') qmark = !qmark; 
		if ((qmark || line[i]!=';') && line[i]!='\"') x += line[i];	
		if (!qmark && line[i]==';') {
			values[c++] = x;
			x = "";
		}
	}

	if (c != 4) throw std::invalid_argument(ExceptionFactory::generateMessage("Command config file has bad format. Please consider the documentation!", "CommandFactory.cc", __LINE__));
	return values;
}

Query CommandFactory::generateQuery(const std::string &commandID) const throw(std::invalid_argument) {
	if (_commands.count(commandID) != 1) throw std::invalid_argument(ExceptionFactory::generateMessage("Command ID \'" + commandID + "\' not found!", "CommandFactory.cc", __LINE__));
	return Query(commandID, (_commands.find(commandID)->second)[0]);
}

std::string CommandFactory::generateAnswer(const std::string &commandID, const std::string &answer) const throw(std::invalid_argument) {
	std::regex rgx;
	try {
		rgx = std::regex((_commands.find(commandID)->second)[1]);
	} catch(...) {
		throw std::invalid_argument(ExceptionFactory::generateMessage("Regular Expression \'" + (_commands.find(commandID)->second)[1] + "\' does not compile!", "CommandFactory.cc", __LINE__));
	}

	std::string input = answer;
	std::vector<std::string> matches;
	for (std::smatch match; std::regex_search(input, match, rgx); input=match.suffix().str()) {
		matches.push_back(match[0]);
	}

	std::string pattern = (_commands.find(commandID)->second)[2];
	pattern += ";";
	bool qmark = false;
	int x = 0;
	std::string out("");
	size_t c = 0;
	for (size_t i=0; i<pattern.length(); i++) {
		if (c >= matches.size()) out += pattern[i];
		else if (pattern[i] == '?') qmark = true;
		else {
			if (qmark) {
				if (pattern[i]>='0' && pattern[i]<='9') x = x*10 + pattern[i]-'0';
				else {
					qmark = false;
					out += matches[c++].substr(x) + pattern[i];
					x = 0;
				}
			} else out += pattern[i];
		}
	}
    
	return out.substr(0, out.length()-1);
}
