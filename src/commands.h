/* commands.h */
#ifndef COMMANDS_H
#define COMMANDS_H


#include "env.h"
#include "types.h"
#include "vibe.h"


static types::CommandRegistry cmdRegistry;

//Definitions of commands.
namespace cmdDefinition {


void help(const dpp::slashcommand_t& event) {
	std::string cmdName = std::get<std::string>(event.get_parameter("command"));
	std::string message = "";

	if (cmdName == "list") {
		//List all command names.
		for (const types::Command& cmd : cmdRegistry.commands) {
			//Add each command's name to the list.
			message += std::format(
				"- {} : [{}]\n", cmd.name, cmd.description
			);
		}

	} else {
		types::Command cmd;
		#ifdef VERBOSE
		std::cout << std::format("Requested help for {}", cmdName) << std::endl;
		#endif
		if (cmdRegistry.get(cmdName, &cmd)) {
			//Success, found command with that name.
			message = std::format("***/{}:***  \"{}\"", cmdName, cmd.help);
		} else {
			//Could not find command with that name.
			message = std::format("Unknown command name: \"{}\". Use `/help list` to show all command names.", cmdName);
		}
	}

	event.reply(
		dpp::message(message).set_flags(dpp::m_ephemeral)
	);
}



void phrase(const dpp::slashcommand_t& event) {
	std::string phraseFile = std::get<std::string>(event.get_parameter("file"));
	utils::toLower(phraseFile);
	std::string message = "";

	std::vector<std::string> validFiles = file::readFileLines(env::get("TXT_DIR") + "cmds.txt");

	if (phraseFile == "list") {
		//List all filenames.
		for (const std::string& fileName : validFiles) {
			message += std::format("- {}\n", fileName);
		}

		event.reply(
			dpp::message(message).set_flags(dpp::m_ephemeral)
		);
		return;
	}


	//Must be a file.
	auto it = std::find(validFiles.begin(), validFiles.end(), phraseFile);
	if (it == validFiles.end()) {
		//Did not find the file, tell the user.
		message = std::format("Could not find {} : use `/phrase list` to see all valid files.", phraseFile);
	} else {
		//Found it, randomly select a phrase.
		//Could cache these file reads later. TBD.
		std::vector<std::string> phrases = file::readFileLines(env::get("TXT_DIR") + "phrases/" + phraseFile + ".txt");
		int randomIndex = rand() % static_cast<int>(phrases.size());
		message = std::format(
			"> _\"{}\"_\n\\- _{}_", phrases.at(randomIndex), phraseFile
		);

		#ifdef VERBOSE
		std::cout << std::format("Giving line {} from {} ({})", randomIndex, phraseFile, phrases.at(randomIndex)) << std::cout;
		#endif
	}

	event.reply(message);
}



void vibe(const dpp::slashcommand_t& event) {
	//Allows the user to search by some metric (who made it, title, genre etc) or get a random set.
	std::string metric = std::get<std::string>(event.get_parameter("metric")); //What to search by
	std::string value = std::get<std::string>(event.get_parameter("query")); //Value to search for

	//Convert to lowercase.
	utils::toLower(metric);
	utils::toLower(value);

	#ifdef VERBOSE
	std::cout << std::format("Searching for {} = {}", metric, value) << std::endl;
	#endif

	//Search by metric;
	dpp::message msg = vibe::query(metric, value);
	event.reply(msg);
}


}



//Management of commands.
namespace cmd {

//All Command instances to be added.
static const std::vector<types::Command> commandList = {
	types::Command(
		"echo", "Echoes a message", "Repeats whatever message you give it.",
		[](const dpp::slashcommand_t& event) {
			event.reply(std::get<std::string>(event.get_parameter("message")));
		}, {
			dpp::command_option(dpp::co_string, "message", "Message to echo", true)
		}
	),

	types::Command(
		"help", "Get help about another command.", "Displays help for another command.",
		cmdDefinition::help, {dpp::command_option(dpp::co_string, "command", "Command to get help about.", true)}
	),

	types::Command(
		"phrase", "Sends a random phrase from a given file.", "Selects a line from a given file at random, to send. Uses the original filenames D3 used, such as `/cesko` → `/phrase cesko`.",
		cmdDefinition::phrase, {dpp::command_option(dpp::co_string, "file", "File to read from.", true)}
	),

	types::Command(
		"vibe", "Search for a song.",
		"Either search for a specific song, or get a random selection. Search options (Metrics):\n- ID\n- name\n- artist\n- duration\n- genre\n- suggestedBy\n- intensity\n- mood\n- association\n- random",
		cmdDefinition::vibe, {
			dpp::command_option(dpp::co_string, "metric", "What metric to search by.", true),
			dpp::command_option(dpp::co_string, "query", "The value to search for.", true)
		}
	),
};


void define() {
	//Define commands.
	for (const types::Command& cmd : commandList) {
		//Add to cmdRegistry.
		cmdRegistry.add(cmd);
	}
}


	
}


#endif

