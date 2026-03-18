/* commands.h */
#ifndef COMMANDS_H
#define COMMANDS_H


//Definitions of commands.
namespace cmdDefinition {



void testDef(const dpp::slashcommand_t& event) {
	event.reply("Testing!");
}



}



//Management of commands.
namespace cmd {

static types::CommandRegistry registry;

//All Command instances to be added.
static const std::vector<types::Command> commandList = {
	types::Command("testing", "Tests", "If quizzes are quizzical..", cmdDefinition::testDef),
};


void examples() {
	//Some example commands for my own ref, using lambda funcs.
	registry.add(types::Command("ping", "Is it on?", "Tests the bot", [](const dpp::slashcommand_t& event) {
		event.reply("Yeah :)");
	}));

	registry.add(types::Command("hello", "hi", "Says hi", [](const dpp::slashcommand_t& event) {
		event.reply("hi");
	}));

	registry.add(types::Command("echo", "echos an arg", "Works like the terminal command",
        [](const dpp::slashcommand_t& event){
            std::string arg = std::get<std::string>(event.get_parameter("arg"));
            event.reply(arg);
        },
        { dpp::command_option(dpp::co_string, "arg", "What to echo", true) }
    ));
}


void define() {
	//Define commands.
	examples();

	for (const types::Command& cmd : commandList) {
		//Add to registry.
		registry.add(cmd);
	}
}


	
}


#endif

