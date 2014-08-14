#include "Query.h"


Query::Query(const std::string command) : _command(command) {
}

std::string Query::command(void) const {
	return _command;
}

