/* main.cpp */


#include "src/includes.h" //General includes.
#include "src/env.h"      //.env loading & parsing.
#include "src/types.h"    //Typedefs & Classes
#include "src/utils.h"    //Utility functions
#include "src/commands.h" //For general user commands.
#include "src/message.h"  //For what to do when a user sends a message.



//Template
void init();


std::atomic<bool> D6isRunning = true;
void signalHandler(int signal) {
	if (signal == SIGINT) {
		D6isRunning = false;
	}
}


int main() {
	init();

	dpp::cluster D6 = dpp::cluster(
		env::get("BOT_TOKEN"),
		dpp::i_default_intents | dpp::i_message_content //Intents
	);

	types::CommandRegistry& reg = cmdRegistry;


	//Register commands w/ descs
	D6.on_ready([&D6, &reg](const dpp::ready_t&) {
		if (dpp::run_once<struct register_D6_commands>()) {
	        D6.global_bulk_command_delete(); //Delete globals (Not needed)
			cmdRegistry.register_all(D6); //Register commands
		}
	});

	//Handle messages being sent for things like /count logging.
	D6.on_message_create([&D6](const dpp::message_create_t& event) {
	    //Ignore bots (including itself)
	    if (event.msg.author.is_bot()) {return;}

	    const std::string& content = event.msg.content;
	    const std::string& username = event.msg.author.username;
	    #ifdef ON_MSG_DEBUG
	    std::cout << std::format(
	    	"{} [{}]: {}",
	    	event.msg.member.get_nickname(),
	    	username, content
	    ) << std::endl;
	    #endif

	    onMessage::inquisition(event);
	    onMessage::logWords(content, username);

	});

	//Handle /commands
	D6.on_slashcommand([&reg](const dpp::slashcommand_t& event) {
		cmdRegistry.handle_command(event);
	});


	//Run D6.
	std::cout << "\nRunning D6..\n^C to exit." << std::endl;
	D6.start(dpp::st_return); //Non-blocking

	do {
	    std::this_thread::sleep_for(std::chrono::milliseconds(100)); //Check every 100ms.
	} while (D6isRunning);

	//Call proper shutdown.
	D6.shutdown();
	std::cout << std::endl;

	return 0;
}





void init(void) {
	std::signal(SIGINT, signalHandler); //Tell it to call that callback when ^C given.

	env::load(); //Load .env file.
	srand(time(0)); //Randomise seed based on time.

	xml::loadVibeXML(); //Load vibe/songData.xml
	words::loadCSV(); //Load the wordOccurrences CSV file.
	cmd::define(); //Add commands.
}