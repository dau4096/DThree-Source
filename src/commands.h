/* commands.h */
#ifndef COMMANDS_H
#define COMMANDS_H


static types::CommandRegistry cmdRegistry;

//Definitions of commands.
namespace cmdDefinition {



void testDef(const dpp::slashcommand_t& event) {
	event.reply("Testing!");
}


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
    	if (cmdRegistry.get(cmdName, &cmd)) {
    		//Success, found command with that name.
    		message = std::format("{}: \"{}\"", cmdName, cmd.help);
    	} else {
    		//Could not find command with that name.
    		message = std::format("Unknown command name: {}. Use `/help list` to show all command names.", cmdName);
    	}
    }

    event.reply(
    	dpp::message(message).set_flags(dpp::m_ephemeral)
    );
}



}



//Management of commands.
namespace cmd {

//All Command instances to be added.
static const std::vector<types::Command> commandList = {
	types::Command("testing", "Tests", "If quizzes are quizzical..", cmdDefinition::testDef),
	types::Command(
		"help", "Get help about another command.", "Displays help for another command.",
		cmdDefinition::help, {dpp::command_option(dpp::co_string, "command", "Command to get help about.", true)}
	),
};


void examples() {
	//Some example commands for my own ref, using lambda funcs.
	cmdRegistry.add(types::Command("ping", "Is it on?", "Tests the bot", [](const dpp::slashcommand_t& event) {
		event.reply("Yeah :)");
	}));

	cmdRegistry.add(types::Command("hello", "hi", "Says hi", [](const dpp::slashcommand_t& event) {
		event.reply("hi");
	}));

	cmdRegistry.add(types::Command("echo", "echos an arg", "Works like the terminal command",
        [](const dpp::slashcommand_t& event){
            std::string arg = std::get<std::string>(event.get_parameter("arg"));
            event.reply(arg);
        },
        {dpp::command_option(dpp::co_string, "arg", "What to echo", true)}
    ));
}


void define() {
	//Define commands.
	examples();

	for (const types::Command& cmd : commandList) {
		//Add to cmdRegistry.
		cmdRegistry.add(cmd);
	}
}


	
}


#endif

