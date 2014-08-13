#ifndef __PROPERTY_H__
#define __PROPERTY_H__

#include <map>
#include <string>
#include <vector>


class Property {
public:
	Property(void);
	Property(std::string title);
	Property(const Property &other);
	std::string title(void) const;
	bool contains(const std::string &key) const;
	bool add(const std::string &key, const std::string &value);
	std::string get(const std::string &key, const std::string defaultValue="") const;
	std::vector<std::string> keys(void) const;

private:
	std::string _title;
	std::map<std::string, std::string> _properties;
};

#endif
