/* main.cpp */


#include "src/includes.h" //General includes.
#include "src/types.h"    //Typedefs & Classes
#include "src/utils.h"    //Utility functions
#include "src/commands.h" //For general user commands.
#include "src/message.h"  //For what to do when a user sends a message.



int main() {
	env::load();
	dpp::cluster D6 = dpp::cluster(
		env::get("BOT_TOKEN"),
		dpp::i_default_intents | dpp::i_message_content //Intents
	);


	cmd::define(); //Add commands.
	types::CommandRegistry& reg = cmd::registry;


	//Register commands w/ descs
	D6.on_ready([&D6, &reg](const dpp::ready_t& event) {
		if (dpp::run_once<struct register_D6_commands>()) {
	        D6.global_bulk_command_delete(); //Delete globals (Not needed)
			cmd::registry.register_all(D6); //Register commands
		}
	});

	//Handle messages being sent for things like /count logging.
	D6.on_message_create([&D6](const dpp::message_create_t& event) {
	    //Ignore bots (including itself)
	    if (event.msg.author.is_bot()) {return;}


	    const std::string& content = event.msg.content;
	    std::cout << std::format(
	    	"{} [{}]: {}",
	    	event.msg.member.get_nickname(),
	    	event.msg.author.username, content
	    ) << std::endl;
	});

	//Handle /commands
	D6.on_slashcommand([&reg](const dpp::slashcommand_t& event) {
		cmd::registry.handle_command(event);
	});


	//Run D6.
	std::cout << "Running D6..\n^C to exit." << std::endl;
	D6.start(dpp::st_wait);

	return 0;
}

