/*
   ############################################
   # Copyright 2023 Panagiotis (MalwarePad)   #
   # WinAPI only ransomware made from scratch #
   ############################################
*/

#include "ParseJSON.h"

// simple JSON parsing from an older project
std::map<std::string, std::string> parseJson(const std::string& json) {
	std::map<std::string, std::string> result;

	size_t pos = 0;
	while (pos < json.size()) {
		size_t keyStart = json.find("\"", pos);
		if (keyStart == std::string::npos)
			break;

		size_t keyEnd = json.find("\"", keyStart + 1);
		if (keyEnd == std::string::npos)
			break;

		size_t colonPos = json.find(":", keyEnd);
		if (colonPos == std::string::npos)
			break;

		size_t valueStart = json.find_first_not_of(" \t\n\r", colonPos + 1);
		if (valueStart == std::string::npos)
			break;

		size_t valueEnd = json.find_first_of(" \t\n\r,}", valueStart + 1);
		if (valueEnd == std::string::npos)
			break;

		std::string key = json.substr(keyStart + 1, keyEnd - keyStart - 1);
		std::string value = json.substr(valueStart, valueEnd - valueStart);

		// Trim the value if it's enclosed in quotes
		if (value.front() == '"' && value.back() == '"') {
			value = value.substr(1, value.size() - 2);
		}

		result[key] = value;

		pos = valueEnd + 1;
	}

	return result;
}