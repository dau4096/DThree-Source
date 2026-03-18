/* utils.h */
#ifndef UTILS_H
#define UTILS_H


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


std::unordered_map<std::string, std::string> loadENV() {
	std::ifstream file = std::ifstream(".env");
	std::unordered_map<std::string, std::string> env;

	std::string line;
	while (std::getline(file, line)) {
		//Not empty or comment.
		if ((line.empty()) || (line[0] == '#')) {continue;}

		//Try find equ.
        auto pos = line.find('=');
        if (pos == std::string::npos) {continue;}

        //Extract key=value.
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        env[key] = value;

        setenv(key.c_str(), value.c_str(), 1);
    }

    return env;		
}

}


#endif