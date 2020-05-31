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
	serialData.canada.ParseString<serial::Json>(MemoryData::canadaString);
	serialData.catalog.ParseString<serial::Json>(MemoryData::catalogString);
	serialData.twitter.ParseString<serial::Json>(MemoryData::twitterString);
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
	auto canadaString = serialData.canada.WriteString<serial::Json>();
	auto catalogString = serialData.catalog.WriteString<serial::Json>();
	auto twitterString = serialData.twitter.WriteString<serial::Json>();
}

TEST(serial, prettify) {
	auto canadaString = serialData.canada.WriteString<serial::Json>(serial::Node::Format::Beautified);
	auto catalogString = serialData.catalog.WriteString<serial::Json>(serial::Node::Format::Beautified);
	auto twitterString = serialData.twitter.WriteString<serial::Json>(serial::Node::Format::Beautified);
}

TEST(serial, writeToFiles) {
	std::ofstream canadaFile("Tests/canada.serial.json", std::ios_base::binary | std::ios_base::out);
	serialData.canada.WriteStream<serial::Json>(canadaFile);
	std::ofstream catalogFile("Tests/citm_catalog.serial.json", std::ios_base::binary | std::ios_base::out);
	serialData.catalog.WriteStream<serial::Json>(catalogFile, serial::Node::Format::Beautified);
	std::ofstream twitterFile("Tests/twitter.serial.json", std::ios_base::binary | std::ios_base::out);
	serialData.twitter.WriteStream<serial::Json>(twitterFile, serial::Node::Format::Beautified);
}
