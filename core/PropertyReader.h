#ifndef __PROPERTY_READER_H__
#define __PROPERTY_READER_H__

#include <exception>
#include <fstream>
#include <map>
#include <vector>

#include "Property.h"


class PropertyReader {
public:
	PropertyReader(std::ifstream &propertyFile, const std::string &defaultTitle) throw(std::invalid_argument);
	void setTitle(const std::string name);
	std::string getTitle(void) const;
	bool contains(const std::string &key) const;
	std::string get(const std::string &key, const std::string defaultValue="") const;

private:
	std::string _title;
	std::map<std::string, Property> _properties;

	std::vector<std::string> seperateLines(std::ifstream &propertyFile);
	std::string trim(const std::string &) const;
	std::pair<std::string, std::string> splitByFirstOccurrence(const std::string &, const char) const;
	std::string toLowerCase(const std::string &) const;
};

#endif
