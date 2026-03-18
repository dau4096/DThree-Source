/* env.h */
#pragma once


namespace env {

static std::unordered_map<std::string, std::string> data;

void load() {
	std::ifstream file = std::ifstream(".env"); //File called ".env"

	std::string line;
	while (std::getline(file, line)) {
		//Not empty or comment.
		if ((line.empty()) || (line[0] == '#')) {continue;}

		//Try find equ sym.
		auto pos = line.find('=');
		if (pos == std::string::npos) {continue;}

		//Extract key=value.
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);
		data[key] = value;

		setenv(key.c_str(), value.c_str(), 1);
	}	
}

std::string& get(const std::string& name) {
	return data.at(name);
}

}