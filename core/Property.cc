#include "Property.h"


Property::Property(void) {
}

Property::Property(std::string title) : _title(title) {
}

Property::Property(const Property &other) : _title(other.title()) {
	for (std::string &key : other.keys()) {
		std::string value = other.get(key);
		add(key, value);
	}
}

std::string Property::title(void) const {
	return _title;
}

bool Property::contains(const std::string &key) const {
	return _properties.count(key);
}

bool Property::add(const std::string &key, const std::string &value) {
	if (contains(key)) return false;

	_properties.insert(std::pair<std::string, std::string>(key, value));
}

std::string Property::get(const std::string &key, const std::string defaultValue) const {
	if (!contains(key)) {
		return defaultValue;
	}

	std::string value = _properties.find(key)->second;
	return value;
}

std::vector<std::string> Property::keys(void) const {
	std::vector<std::string> k;

	for (auto &pair : _properties) {
		std::string key = pair.first;
		k.push_back(key);
	}

	return k;
}
