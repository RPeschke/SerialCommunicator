#ifndef __QUERY_H__
#define __QUERY_H__

#include <string>

class Query {
public:
	Query(std::string queryString, std::string id);
	std::string command(void) const;
	std::string id(void) const;

private:
	std::string _queryString;
	std::string _id;
};

#endif
