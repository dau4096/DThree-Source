/* words.h */
#ifndef WORDS_H
#define WORDS_H


#include "types.h"
#include "utils.h"
#include "graph.h"


namespace words {


//Uses custom hash KeyHash.
static std::unordered_map<types::Key, unsigned int, types::KeyHash> dataset = {};


void loadCSV() {
	//Loads wordOccurrences.csv into memory.
	const std::string CSVpath = env::get("DISK_DIR") + "RWM/wordOccurrences.csv";
	std::ifstream csv = std::ifstream(CSVpath);
	
	std::string unparsableLinesMessage = "[Unparsable wordOccurrences.csv lines}:\n";
	
	std::string line;
	unsigned int lineNumber = 0u;
	while (std::getline(csv, line)) {
		if (lineNumber == 0u) {lineNumber++; continue; /* Header line, do not read. */}
		try {
		//Get each line in sequence.
		size_t p1 = line.find(",");
		size_t p2 = line.find(",", p1+1u);
		size_t p3 = line.find(",", p2+1u);

		types::Key key = types::Key(
			line.substr(0,     p1      ), //User
			line.substr(p1+1u, p2-p1-1u), //Word
			line.substr(p2+1u, p3-p2-1u)  //Date
		);
		unsigned int count = std::stoi(line.substr(p3+1u));

		words::dataset[key] = count;

		} catch (...) {
			#ifdef VERBOSE
			std::cout << std::format("Could not parse line [{}]: \"{}\"", lineNumber, line) << std::endl;
			#endif
			unparsableLinesMessage += std::format("LN {} : {}\n", lineNumber, line);
		}

		lineNumber++;
	}

	csv.close();

#ifdef VERBOSE
	std::cout << std::format("Successfully loaded [{}] datapoints from \"{}\"", lineNumber, CSVpath) << std::endl;
#endif


	//If unparsed lines were found. Clears file, if none were found.
	std::ofstream logfile = std::ofstream("unparsable.log");
	logfile << unparsableLinesMessage;
	logfile.close();

}


std::string sanitiseWord(const std::string& word) {
	//Clean up the word, stop it from being counted if certain criteria are met.
	std::string result;
	result.reserve(word.size());

	for (unsigned char c : word) {
		if (std::isalnum(c)) {
			//If is alpha-numeric.
			result += c;
		}
	}

	utils::toLower(result);

	return result;
}


void incrementWord(const std::string& who, const std::string word) {
	//Increments the occurrences of the value stored in the CSV.
	std::string sanWord = sanitiseWord(word);
	unsigned int count;
	if (sanWord.size() > 0u) {
		types::Key key = types::Key(who, sanWord, utils::getCurrentDate()); //Who, What, When.
		count = ++dataset[key]; //Increment.
	} else {
		count = 0u;
	}
#ifdef DEBUG_WORDS
	std::cout << std::format(
		"[{}] said \"{}\" → \"{}\", New count is [{}]",
		who, word, sanWord, count
	) << std::endl;
#endif
}




void saveCSV() {
	const std::string CSVpath = env::get("DISK_DIR") + "RWM/wordOccurrences.csv.TEMPORARY";
	std::ofstream csv = std::ofstream(CSVpath);

	//Write header
	csv << "Name,Word,Date,Occurrences\n";

	for (const auto& [key, count] : dataset) {
		csv << std::format(
			"{},{},{},{}\n",
			key.user, key.word, key.date, count
		);
	}

	csv.close();
}




bool getWord(const std::string& query, std::unordered_map<types::Key, unsigned int, types::KeyHash>& result) {
	//Get all occurrences of a certain word.
	for (const auto& [key, count] : dataset) {
		if (key.word == query) {
			result[key] = count;
		}
	}
	return result.size() > 0u;
}

bool getUser(const std::string& query, std::unordered_map<types::Key, unsigned int, types::KeyHash>& result) {
	//Get daily count attributed to some user.
	for (const auto& [key, count] : dataset) {
		if (key.user == query) {
			//Convert to be counts per day. Index by some constant string, but with variable date.
			types::Key newKey = types::Key(key.user, "<NONE>", key.date);
			result[newKey] = count;
		}
	}

	return result.size() > 0u;
}


void saveGraph(std::unordered_map<types::Key, unsigned int, types::KeyHash>& slice) {
	
}


}


#endif
