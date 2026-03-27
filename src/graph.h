/* graph.h */
#ifndef GRAPH_H
#define GRAPH_H


#include "env.h"
#include "drawing.h"


namespace graph {

	void wordsGraph(std::unordered_map<types::Key, unsigned int, types::KeyHash>& results) {
		//Create a graph based on the results vector.
		std::cout << "Number of results: " << results.size() << std::endl;
	}


	void drawWordsGraph(
		const std::string& query, const std::string& value,
		std::unordered_map<types::Key, unsigned int, types::KeyHash>& results
	) {
		//Draws a graph from the given words data.
		std::string title;
		if (query == "word") {
			title = std::format(
				"Occurrences of \'{}\' over time", value
			);
		} else if (query == "user") {
			title = std::format(
				"Words per day, posted by [{}]", value
			);
		} else {
			std::cout << "Invalid query: " << query << std::endl;
			return;
		}
		draw::text(title, glm::ivec2(0, 0)); //Draw title at top left.

		wordsGraph(results);

		draw::saveBuffer(env::get("DISK_DIR") + "RWM/graph.png");
	}

}



#endif