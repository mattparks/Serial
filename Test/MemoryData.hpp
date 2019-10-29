#pragma once

#include <string>

static class MemoryData {
public:
	MemoryData();
	
	std::string canadaString;
	std::string catalogString;
	std::string twitterString;
} memoryData;

struct Status {
	struct User {
		uint32_t id = -1;
		std::string id_str;
		
		std::string name, screen_name;
		std::string location, description;
		uint32_t followers_count, friends_count, listed_count, favourites_count, statuses_count;
	} user;

	uint64_t id = -1;
	std::string id_str;
};
