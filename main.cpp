/* main.cpp */


#include "src/includes.h"
#include "src/types.h"
#include "src/utils.h"


int main() {
	std::unordered_map<std::string, std::string> env = file::loadENV();
	dpp::cluster bot = dpp::cluster(env["BOT_TOKEN"]);

	bot.on_slashcommand([](auto event) {
		if (event.command.get_command_name() == "ping") {
			event.reply("Recv");
		}
	});

	bot.on_ready([&bot](const dpp::ready_t& event) {
		if (dpp::run_once<struct register_bot_commands>()) {
			bot.global_command_create(dpp::slashcommand("ping", "Ping pong!", bot.me.id));
		}
	});

	std::cout << "Running D6..\n^C to exit." << std::endl;
	bot.start(dpp::st_wait);

	return 0;
}