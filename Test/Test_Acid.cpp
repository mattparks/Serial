#include <gtest/gtest.h>

#include <Files/Json/Json.hpp>

#include "MemoryData.hpp"

static class AcidData {
public:
	acid::Json canada;
	acid::Json catalog;
	acid::Json twitter;
} acidData;

TEST(acid, parseInMemory) {
	acidData.canada.LoadString(memoryData.canadaString);
	acidData.catalog.LoadString(memoryData.catalogString);
	acidData.twitter.LoadString(memoryData.twitterString);
}

inline const acid::Node &operator>>(const acid::Node &node, Status &status) {
	node["id"].Get(status.id);
	node["id_str"].Get(status.id_str);
	node["user"].Get(status.user);
	return node;
}

inline const acid::Node &operator>>(const acid::Node &node, Status::User &user) {
	node["id"].Get(user.id);
	node["id_str"].Get(user.id_str);
	
	node["name"].Get(user.name);
	node["screen_name"].Get(user.screen_name);
	node["location"].Get(user.description);
	node["followers_count"].Get(user.followers_count);
	node["friends_count"].Get(user.friends_count);
	node["listed_count"].Get(user.listed_count);
	node["favourites_count"].Get(user.favourites_count);
	node["statuses_count"].Get(user.statuses_count);

	return node;
}

TEST(acid, getTwitterData) {
	auto statuses = acidData.twitter["statuses"].Get<std::vector<Status>>();
}

TEST(acid, stringify) {
	auto canadaString = acidData.canada.WriteString();
	auto catalogString = acidData.catalog.WriteString();
	auto twitterString = acidData.twitter.WriteString();
}

TEST(acid, prettify) {
	auto canadaString = acidData.canada.WriteString(acid::Json::Format::Beautified);
	auto catalogString = acidData.catalog.WriteString(acid::Json::Format::Beautified);
	auto twitterString = acidData.twitter.WriteString(acid::Json::Format::Beautified);
}

TEST(acid, writeToFiles) {
	std::ofstream canadaFile("Tests/canada.acid.json", std::ios_base::binary | std::ios_base::out);
	acidData.canada.WriteStream(canadaFile);
	std::ofstream catalogFile("Tests/citm_catalog.acid.json", std::ios_base::binary | std::ios_base::out);
	acidData.catalog.WriteStream(catalogFile, acid::Json::Format::Beautified);
	std::ofstream twitterFile("Tests/twitter.acid.json", std::ios_base::binary | std::ios_base::out);
	acidData.twitter.WriteStream(twitterFile, acid::Json::Format::Beautified);
}
