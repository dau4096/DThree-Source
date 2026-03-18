/* message.h */
#ifndef MESSAGE_H
#define MESSAGE_H


#include "env.h"


namespace onMessage {


void inquisition(const dpp::message_create_t& event) {
	//Sends the spanish inquisition gif, if random chance and enough time has passed.

	//Read the current value from file.
	unsigned int messagesSinceLastInquisition;
	std::string path = env::get("DISK_DIR") + "RWM/messagesSinceLastInquisition.txt";
	std::ifstream inFile = std::ifstream(path);
	if (!inFile) {
		//Could not load file.
		messagesSinceLastInquisition = 0u; //Initialise.
	} else {
		inFile >> messagesSinceLastInquisition;
	}


	//Check if the gif should be sent
	if (
		((messagesSinceLastInquisition > 0x400u) && !(rand() & 0x400u)) ||
		(messagesSinceLastInquisition > 0x800u)
	) {
		//If its more than 1024, AND the random number rolls a zero:
		//OR if its more than 2048:
		//Send the gif.
	    #ifdef ON_MSG_DEBUG
		std::cout << "Nobody expects the Spanish Inquisition!" << std::endl;
		#endif

		dpp::message msg = dpp::message("Nobody expects the Spanish Inquisition!");
		msg.add_file("Inquisition.gif", dpp::utility::read_file(env::get("DISK_DIR") + "ROM/Inquisition.gif"));
		event.reply(msg);
	}


	//Update number in file.
	messagesSinceLastInquisition++;
	std::ofstream outFile = std::ofstream(path);
	if (!outFile) {
		//Failed to open/create file.
		std::cerr << "Failed to open/create \"" << path << "\"" << std::endl;
		return;
	}
    #ifdef ON_MSG_DEBUG
	outFile << "messagesSinceLastInquisition: " << messagesSinceLastInquisition << "\n";
	#endif
}


void logWords(const std::string& content, const std::string& username) {
	//TBA
}

}


#endif
