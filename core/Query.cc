#include "Query.h"


Query::Query(std::string id, std::string queryString) : _id(id), _queryString(queryString) {
}

std::string Query::id(void) const {
	return _id;
}

std::string Query::command(void) const {
	return _queryString;
}

