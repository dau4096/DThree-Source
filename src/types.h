/* types.h */
#ifndef TYPES_H
#define TYPES_H


#include "includes.h"
#include "utils.h"


namespace types {

class Command {
public:
	std::string name;
	std::string description;
	std::string help;
	std::function<void(const dpp::slashcommand_t&)> callback;
	std::vector<dpp::command_option> options; //Args

	Command(
		const std::string& n, const std::string& d, const std::string& h,
		std::function<void(const dpp::slashcommand_t&)> cb, const std::vector<dpp::command_option>& opts = {}
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

			D6.guild_command_create(sc, env::get("DR_ID"));
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

}


#endif
