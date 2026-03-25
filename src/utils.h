/* utils.h */
#pragma once

#include "includes.h"
#include "env.h"
#include "types.h"





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

std::string getCurrentDate() {
	auto now = std::chrono::system_clock::now();
	auto days = floor<std::chrono::days>(now);
	auto ymd = std::chrono::year_month_day(days);

	return std::format(
		"{:04}-{:02}-{:02}",
		static_cast<int>(ymd.year()),
		static_cast<unsigned int>(ymd.month()),
		static_cast<unsigned int>(ymd.day())
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



namespace xml {


inline unsigned int getUInt(pugi::xml_node& node, const std::string& name) {
	return static_cast<unsigned int>(node.attribute(name.c_str()).as_int());
}
inline std::string getStr(pugi::xml_node& node, const std::string& name, bool lowercase=true) {
	std::string s = node.attribute(name.c_str()).as_string();
	if (lowercase) {utils::toLower(s);}
	return s;
}
inline std::vector<std::string> getStrVec(pugi::xml_node& node, const std::string& name) {
	std::string combined = getStr(node, name);
	std::vector<std::string> result;
	std::stringstream ss = std::stringstream(combined);
	std::string item;

	while (std::getline(ss, item, ';')) { //Split on ; chars.
		utils::toLower(item);
		result.push_back(item);
	}

	return result;
}


#define URL_REGEX std::regex(R"(^(https://.+)$)") /* For unknown reasons, the dataset encloses these in []. */
#define PATH_REGEX std::regex(R"(^[a-zA-Z0-9/]+?\.(mp3|mp4|ogg|wav|m4a)$)") /* Valid filepath, only allows certain formats. */
void getSongURL(pugi::xml_node& node, std::string* URL, SongEntryType* type) {
	//Identifies type based on the "link" attribute's format.
	const std::string linkAttr = getStr(node, "link", false);
	if (std::regex_match(linkAttr, URL_REGEX)) {
		//Must be a URL.
		*URL = linkAttr;
		*type = SET_URL;
	} else if (std::regex_match(linkAttr, PATH_REGEX)) {
		//Must be a valid audio file (MP4 also permitted)
		*URL = linkAttr;
		*type = SET_FILEPATH;
	} else {
		//Is not either of the accepted formats.
		std::cout << std::format("Could not find valid URL/Path for \"{}\" in Song \"{}\"", linkAttr, getStr(node, "name")) << std::endl;
		*URL = ""; //No valid file.
		*type = SET_INVALID;
	}
}




static std::vector<types::Song> songList;
void loadVibeXML() {
	std::string XMLSrc = file::readFile(env::get("TXT_DIR") + "/vibe/songData.xml");

	pugi::xml_document doc;
	pugi::xml_parse_result parseResult = doc.load_string(XMLSrc.c_str());
	if (!parseResult) {
		throw std::runtime_error("Failed to parse XML: " + std::string(parseResult.description()));
	}

	const char* xpath = "//songs/song";
	pugi::xpath_node_set nodeList = doc.select_nodes(xpath);
	size_t count = static_cast<size_t>(nodeList.size());

	songList.clear();
	songList.reserve(count);
	
	std::string functionString, keyString;
	for (size_t i=0; i<count; i++) {
		pugi::xml_node node = nodeList[i].node();

		std::string URL;
		SongEntryType type;
		getSongURL(node, &URL, &type);

		songList.push_back((types::Song){
			.ID = getUInt(node, "id"),
			.name = getStr(node, "name"),
			.artist = getStrVec(node, "artist"),
			.duration = getUInt(node, "duration"),
			.genre = getStrVec(node, "genre"),
			.suggestedBy = getStr(node, "suggestedBy"),
			.URL = URL, .type = type, //Managed properly.
			.intensity = getStrVec(node, "intensity"),
			.mood = getStrVec(node, "mood"),
			.association = getStrVec(node, "association")
		});
	}
}

}