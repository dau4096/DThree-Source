/* utils.h */
#pragma once

#include "includes.h"
#include "types.h"

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



namespace utils {

void toUpper(std::string& text) {
    std::transform(
    	text.begin(), text.end(), text.begin(),
    	[](unsigned char c){return std::toupper(c);}
    );
}
void toLower(std::string& text) {
    std::transform(
    	text.begin(), text.end(), text.begin(),
    	[](unsigned char c){return std::tolower(c);}
    );
}

}



namespace file {

static std::string readFile(const std::string& filePath) {
	std::ifstream fileStream(filePath, std::ios::binary);
	if (!fileStream.is_open()) {
		throw std::runtime_error("Error: Could not open file: " + filePath);
	}

	std::ostringstream buffer;
	buffer << fileStream.rdbuf();
	return buffer.str();
}


std::vector<std::string> readFileLines(const std::string& filePath) {
	std::string text = readFile(filePath);

    std::vector<std::string> lines;
    std::stringstream ss(text);
    std::string line;

    while (std::getline(ss, line)) {
        lines.push_back(line);
    }

    return lines;
}


}
