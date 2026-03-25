/* types.h */
#ifndef TYPES_H
#define TYPES_H


#include "includes.h"
#include "env.h"



enum SongEntryType {
	SET_INVALID, //No provided link/URL/path.
	SET_URL, //Web URL, youtube/spotify/etc.
	SET_FILEPATH //Local filepath to a compatible audio file.
};


namespace types {

struct Command {
	std::string name;
	std::string description;
	std::string help;
	std::function<void(const dpp::slashcommand_t&)> callback;
	std::vector<dpp::command_option> options; //Args

	Command() : name(""), description(""), help(""), callback(), options() {}

	Command(
		const std::string& n, const std::string& d, const std::string& h,
		std::function<void(const dpp::slashcommand_t&)> cb, const std::vector<dpp::command_option>& opts={}
	)	: name(n), description(d), help(h), callback(cb), options(opts) {}
};




class CommandRegistry {
public:
	std::vector<Command> commands;

	void add(const Command& cmd) {
		commands.push_back(cmd);
	}

	void register_all(dpp::cluster& D6) {
		for (const Command& cmd : commands) {
			dpp::slashcommand sc(cmd.name, cmd.description, D6.me.id);
			for (const dpp::command_option& opt : cmd.options) {
				sc.add_option(opt);
			}

			//One for D's repo, one for SW GC.
			D6.guild_command_create(sc, env::get("DR_ID"));
			D6.guild_command_create(sc, env::get("SWGC_ID")); //Disabled for now.
		}
	}
	
	void handle_command(const dpp::slashcommand_t& event) {
		for (const Command& cmd : commands) {
			if (event.command.get_command_name() == cmd.name) {
				cmd.callback(event);
				break;
			}
		}
	}

	bool get(const std::string& name, Command* cmd) {
		for (Command& c : commands) {
			if (c.name == name) {
				*cmd = c;
				return true; //Found valid command of the same name.
			}
		}
		return false; //Failed to find.

	}
};



struct Song {
	unsigned int ID; //Unique ID
	std::string name; //What is it called?
	std::vector<std::string> artist; //Who made it?
	unsigned int duration; //How long, in seconds?
	std::vector<std::string> genre; //What genres does it fit into?
	std::string suggestedBy; //User
	std::string URL; //Or filepath.
	SongEntryType type; //What sort of URL/filepath it is.
	std::vector<std::string> intensity; //How intense is it?
	std::vector<std::string> mood; //What moods is this song?
	std::vector<std::string> association; //Where is it from?
};



//For the word-occurrences CSV.
struct Key {
	std::string user;
	std::string word;
	std::string date;

	Key(const std::string& u, const std::string& w, const std::string& d)
		: user(u), word(w), date(d) {}

	bool operator==(const Key& other) const {
		return (
			(user == other.user) && 
			(date == other.date) && 
			(word == other.word)
		);
	}
};

struct KeyHash {
	std::size_t operator()(const Key& k) const {
		return (
			std::hash<std::string>()(k.user) ^
			std::hash<std::string>()(k.word) ^
			std::hash<std::string>()(k.date)
		);
	}
};

}


#endif
