#include <gtest/gtest.h>

#include <fstream>
#include <Json/Json.hpp>

#include "MemoryData.hpp"

static class AcidData {
public:
	serial::Node canada;
	serial::Node catalog;
	serial::Node twitter;
} serialData;

TEST(serial, parseInMemory) {
	serialData.canada = serial::Json::ParseString(MemoryData::canadaString);
	serialData.catalog = serial::Json::ParseString(MemoryData::catalogString);
	serialData.twitter = serial::Json::ParseString(MemoryData::twitterString);
}

inline const serial::Node &operator>>(const serial::Node &node, Status &status) {
	node["id"].Get(status.id);
	node["id_str"].Get(status.id_str);
	node["user"].Get(status.user);
	return node;
}

inline const serial::Node &operator>>(const serial::Node &node, Status::User &user) {
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

TEST(serial, getTwitterData) {
	auto statuses = serialData.twitter["statuses"].Get<std::vector<Status>>();
}

TEST(serial, stringify) {
	auto canadaString = serial::Json::WriteString(serialData.canada);
	auto catalogString = serial::Json::WriteString(serialData.catalog);
	auto twitterString = serial::Json::WriteString(serialData.twitter);
}

TEST(serial, prettify) {
	auto canadaString = serial::Json::WriteString(serialData.canada, serial::NodeFormat::Beautified);
	auto catalogString = serial::Json::WriteString(serialData.catalog, serial::NodeFormat::Beautified);
	auto twitterString = serial::Json::WriteString(serialData.twitter, serial::NodeFormat::Beautified);
}

TEST(serial, writeToFiles) {
	std::ofstream canadaFile("Tests/canada.serial.json", std::ios_base::binary | std::ios_base::out);
	serial::Json::WriteStream(serialData.canada, canadaFile);
	std::ofstream catalogFile("Tests/citm_catalog.serial.json", std::ios_base::binary | std::ios_base::out);
	serial::Json::WriteStream(serialData.catalog, catalogFile, serial::NodeFormat::Beautified);
	std::ofstream twitterFile("Tests/twitter.serial.json", std::ios_base::binary | std::ios_base::out);
	serial::Json::WriteStream(serialData.twitter, twitterFile, serial::NodeFormat::Beautified);
}
