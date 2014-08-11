#include "PropertyReader.h"

#include "ExceptionFactory.h"


PropertyReader::PropertyReader(std::ifstream &propertyFile) throw(std::invalid_argument) {
	if (!propertyFile.good()) {
		throw std::invalid_argument(ExceptionFactory::generateMessage("Cannot read property file. Ensure that the file exists, is open and readable!", "PropertyReader.cc", __LINE__));
	}

	std::string key("");
	std::string value("");
	bool x = true;
	_properties.clear();
	for (char c=propertyFile.get(); propertyFile.good(); c=propertyFile.get()) {
		if (c!='=' && c!='\n') {
			if (x) key += c;
			else value += c;
		} else if (c == '=') {
			x = false;
		} else {
			_properties.insert(std::pair<std::string, std::string>(key, value));
			key = "";
			value = "";
			x = true;
		}
	}
}

bool PropertyReader::contains(std::string key) const {
	return _properties.count(key) == 1;
}

std::string PropertyReader::get(const std::string key, std::string defaultValue /*=""*/) const {
	if (PropertyReader::contains(key)) return _properties.find(key)->second;
	return defaultValue;
}
