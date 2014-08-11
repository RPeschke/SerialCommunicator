#ifndef __QUERY_H__
#define __QUERY_H__

#include <string>

class Query {
public:
	Query(std::string id, const std::string &queryString);
	std::string id(void) const;
	std::string command(void) const;

private:
	std::string _id;
	std::string _queryString;
};

#endif
