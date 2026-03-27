/* vibe.h */
#ifndef VIBE_H
#define VIBE_H


#include "includes.h"
#include "types.h"



static void shuffleAndResize(std::vector<types::Song*>& v, int n) {
	static std::mt19937 rng(std::random_device{}());
	std::shuffle(v.begin(), v.end(), rng);
	v.resize(std::min(v.size(), static_cast<size_t>(n)));
}




namespace vibe {



//////// FUNC DEFS ////////

static void random(
	const std::string& value, std::string& query,
	std::vector<types::Song*>& results
) {
	//Very simple random set of values.
	query = std::format("{} random songs", value);
	int numberOfSongs = 0;
	try { numberOfSongs = std::stoi(value); }
	catch (...) {return;}
	if (numberOfSongs <= 0) {return;}
	for (types::Song& s : xml::songList) {results.push_back(&s);}
	shuffleAndResize(results, numberOfSongs);
}

static void id(
	const std::string& value, std::string& query,
	std::vector<types::Song*>& results
) {
	//Search for an ID
	query = std::format("ID={}", value);
	int id = 0;
	try { id = std::stoi(value); }
	catch (...) {return;}
	if (id <= 0) {return;}
	for (types::Song& s : xml::songList) {
		if (s.ID == static_cast<unsigned int>(id)) {
			results.push_back(&s);
			break;
		}
	}
}

static void name(
	const std::string& value, std::string& query,
	std::vector<types::Song*>& results
) {
	//Search for titles containing the value.
	query = std::format("titles containing \"{}\"", value);
	if (value.size() == 0u) {return; /* Empty */}
	for (types::Song& s : xml::songList) {
		if (s.name.contains(value)) {
			results.push_back(&s);
		}
	}
}

static void artist(
	const std::string& value, std::string& query,
	std::vector<types::Song*>& results
) {
	//Songs made by this artist.
	query = std::format("songs made by {}", value);
	if (value.size() == 0u) {return; /* Empty */}
	for (types::Song& s : xml::songList) {
		if (std::find(s.artist.begin(), s.artist.end(), value) != s.artist.end()) { //If list of artistes contains the searched for artist
			results.push_back(&s);
		}
	}
}

static void duration(
	const std::string&, std::string& query,
	std::vector<types::Song*>&
) {
	//UNDETERMINED:TBA
	/*
	query = std::format("names containing {}", value);
	for (types::Song& s : xml::songList) {
		if (s.name.contains(value)) {
			results.push_back(&s);
		}
	}*/
	query = "UNDETERMINED:TBA";
}

static void genre(
	const std::string& value, std::string& query,
	std::vector<types::Song*>& results
) {
	//If this genre is one the song falls into.
	query = std::format("genre \"{}\"", value);
	if (value.size() == 0u) {return; /* Empty */}
	for (types::Song& s : xml::songList) {
		if (std::find(s.genre.begin(), s.genre.end(), value) != s.genre.end()) { //If list of genres contains the searched for genre
			results.push_back(&s);
		}
	}
}

static void suggestedBy(
	const std::string& value, std::string& query,
	std::vector<types::Song*>& results
) {
	//Songs that were added to the dataset by a given person
	query = std::format("songs added by {}", value);
	if (value.size() == 0u) {return; /* Empty */}
	for (types::Song& s : xml::songList) {
		if (s.suggestedBy == value) {
			results.push_back(&s);
		}
	}
}

static void intensity(
	const std::string& value, std::string& query,
	std::vector<types::Song*>& results
) {
	//If this intensity is one the song falls into.
	query = std::format("\"{}\"", value);
	if (value.size() == 0u) {return; /* Empty */}
	for (types::Song& s : xml::songList) {
		if (std::find(s.intensity.begin(), s.intensity.end(), value) != s.intensity.end()) { //If list of intensities contains the searched for intensity
			results.push_back(&s);
		}
	}
}

static void mood(
	const std::string& value, std::string& query,
	std::vector<types::Song*>& results
) {
	//If this mood is one the song falls into.
	query = std::format("\"{}\"", value);
	if (value.size() == 0u) {return; /* Empty */}
	for (types::Song& s : xml::songList) {
		if (std::find(s.mood.begin(), s.mood.end(), value) != s.mood.end()) { //If list of moods contains the searched for mood
			results.push_back(&s);
		}
	}
}

static void association(
	const std::string& value, std::string& query,
	std::vector<types::Song*>& results
) {
	//If this association is one the song falls into.
	query = std::format("\"{}\"", value);
	if (value.size() == 0u) {return; /* Empty */}
	for (types::Song& s : xml::songList) {
		if (std::find(s.association.begin(), s.association.end(), value) != s.association.end()) { //If list of associations contains the searched for association
			results.push_back(&s);
		}
	}
}

//////// FUNC DEFS ////////






static dpp::message formatIntoMessage(std::vector<types::Song*>& results, const std::string& query) {
	//Formats into 1 coherent message.
	dpp::message msg = dpp::message(std::format("# Results for {};\n", query));
	unsigned int i = 1u;
	for (const types::Song* ptr : results) {
		std::string artists = "";
		for (size_t i=0u; i<ptr->artist.size(); i++) {
			artists += ptr->artist[i];
			if (i != ptr->artist.size()-1u) {
				artists += ", "; //Add spacing comma.
			}
		}

		std::string line = std::format(
			"## {}) *\"{}\"* by *{}*",
			i, ptr->name, artists
		);
		switch (ptr->type) {
			case SET_FILEPATH: {
				//Add file to the message.
				try {
					msg.add_file(
						ptr->name,
						dpp::utility::read_file(env::get("TEXT_DIR") + "vibe/" + ptr->name)
					);
				} catch (...) {
					//Don't crash.
					std::cout << "Failed to load file: " << ptr->URL << std::endl;
				}
				break;
			}
			case SET_URL: {
				//Must be link.
				line += std::format(
					"\n-# {}", ptr->URL
				);
				break;
			}
			default: {
				continue; //Go to next iter. Skip this entry.
				break;
			}
		}
		msg.content += line + "\n\n";
		i++;
	}

	return msg;
}



inline std::unordered_map<
	std::string,
	std::function<void(const std::string&, std::string&, std::vector<types::Song*>&)>
> vibeMap = {
	{"random", random}, {"id", id}, {"name", name}, {"artist", artist},
	{"duration", duration}, {"genre", genre}, {"suggestedby", suggestedBy},
	{"intensity", intensity}, {"mood", mood}, {"association", association}
};

#define MAX_RETURNED_SONGS 3
static dpp::message query(const std::string& metric, const std::string& value) {
	auto it = vibeMap.find(metric);
	if (it == vibeMap.end()) {
		return dpp::message("Unknown metric: " + metric);
	}

	std::vector<types::Song*> results;
	std::string query; //Used to format the metric/value correctly.
	it->second(value, query, results);

	shuffleAndResize(results, MAX_RETURNED_SONGS);

	return formatIntoMessage(results, query);
}

}



#endif
