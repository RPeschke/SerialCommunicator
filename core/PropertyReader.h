#ifndef __PROPERTY_READER_H__
#define __PROPERTY_READER_H__

#include <exception>
#include <fstream>
#include <map>


class PropertyReader {
public:
	PropertyReader(std::ifstream &propertyFile) throw(std::invalid_argument);
	bool contains(std::string key) const;
	std::string get(std::string key, std::string defaultValue="") const;

private:
	std::map<std::string, std::string> _properties;
};

#endif
