#include "PropertyReader.h"

#include "ExceptionFactory.h"


PropertyReader::PropertyReader(std::ifstream &propertyFile, const std::string &title) throw(std::invalid_argument) : _title(title) {
	if (!propertyFile.good()) {
		throw std::invalid_argument(ExceptionFactory::generateMessage("Cannot read property file. Ensure that the file exists, is open and readable!", "PropertyReader.cc", __LINE__));
	}

	std::vector<std::string> lines = seperateLines(propertyFile);
	propertyFile.close();

	Property property;
	bool first = true;
	for (auto &line : lines) {
		std::pair<std::string, std::string> key_value = splitByFirstOccurrence(line, '='); 
		std::string key = trim(key_value.first);
		std::string value = trim(key_value.second);

		if (key[0]=='[' && key[key.size()-1]==']' && value.size()==0) {
			if (!first) {
				_properties.insert(std::pair<std::string, Property>(property.title(), property));
			} else first = false;

			std::string title = key.substr(1, key.size()-2);
			property = Property(toLowerCase(title));
		} else {
			property.add(key, value);
		}
	}

	_properties.insert(std::pair<std::string, Property>(property.title(), property));
}

std::vector<std::string> PropertyReader::seperateLines(std::ifstream &propertyFile) {
	std::string line("");
	std::vector<std::string> lines;
	for (char c=propertyFile.get(); propertyFile.good(); c=propertyFile.get()) {
		if (c != '\n') line += c;
		else {
			line = trim(line);
			if (line.size() != 0) {
				lines.push_back(line);
				line = "";
			}
		}
	}

	return lines;
}

void PropertyReader::setTitle(const std::string title) {
	_title = title;
}

std::string PropertyReader::getTitle(void) const {
	return _title;
}

bool PropertyReader::contains(const std::string &key) const {
	return _properties.find(_title)->second.contains(key);
}

std::string PropertyReader::get(const std::string &key, const std::string defaultValue) const {
	Property p = _properties.find(_title)->second;
	if (p.contains(key)) return p.get(key, defaultValue);
	return defaultValue;
}

std::string PropertyReader::trim(const std::string &s) const {
	bool x = true;
	int a = 0;
	while(x && a<s.size()) {
		if (s[a]!=' ' && s[a]!='\t') x = false;
		else a++;
	}

	x = true;
	int b = s.size()-1;
	while(x && b>=0) {
		if (s[b]!=' ' && s[b]!='\t') x = false;
		else b--;
	}

	std::string out = s.substr(a);
	out = out.substr(0, b+1);

	return out;
}


std::pair<std::string, std::string> PropertyReader::splitByFirstOccurrence(const std::string &s, const char c) const {
	std::string first("");
	std::string second("");

	bool x = true;
	for (int i=0; i<s.size(); i++) {
		if (x) {
			if (s[i] != c) first += s[i];
			else x = false;
		} else {
			second += s[i];
		}
	}

	return std::pair<std::string, std::string>(first, second);
}

std::string PropertyReader::toLowerCase(const std::string &s) const {
	std::string out("");
	int diff = 'A'-'a';
	for (int i=0; i<s.size(); i++) {
		if (s[i]>='A' && s[i]<='Z') {
			out += s[i] - diff;
		} else {
			out += s[i];
		}
	}

	return out;
}
