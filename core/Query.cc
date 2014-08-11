#include "Query.h"


Query::Query(std::string queryString, std::string id) : _queryString(queryString), _id(id) {
}

std::string Query::command(void) const {
	return _queryString;
}

std::string Query::id(void) const {
	return _id;
}
