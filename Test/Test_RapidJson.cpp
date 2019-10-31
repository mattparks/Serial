#include <fstream>

#include <gtest/gtest.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include "MemoryData.hpp"

static class RapidJsonData {
public:
	rapidjson::Document canada;
	rapidjson::Document catalog;
	rapidjson::Document twitter;
} rapidjsonData;

TEST(rapidjson, parseInMemory) {
	rapidjsonData.canada.Parse(MemoryData::canadaString.c_str());
	rapidjsonData.catalog.Parse(MemoryData::catalogString.c_str());
	rapidjsonData.twitter.Parse(MemoryData::twitterString.c_str());
}

TEST(rapidjson, getTwitterData) {
	auto ok = rapidjsonData.twitter.GetParseError();
	if (ok == rapidjson::kParseErrorNone) {
		std::vector<Status> statuses;
		rapidjson::GenericValue<rapidjson::UTF8<>> &s = rapidjsonData.twitter["statuses"];
		for (rapidjson::Value::ConstValueIterator iter = s.Begin(); iter != s.End(); ++iter) {
			Status status;
			status.id = (*iter)["id"].GetInt();
			status.id_str = (*iter)["id_str"].GetString();

			Status::User user;
			user.id = (*iter)["user"]["id"].GetInt();
			user.id_str = (*iter)["user"]["id_str"].GetString();

			user.name = (*iter)["user"]["name"].GetString();
			user.screen_name = (*iter)["user"]["screen_name"].GetString();
			user.location = (*iter)["user"]["location"].GetString();
			user.description = (*iter)["user"]["description"].GetString();
			user.followers_count = (*iter)["user"]["followers_count"].GetInt();
			user.friends_count = (*iter)["user"]["friends_count"].GetInt();
			user.listed_count = (*iter)["user"]["listed_count"].GetInt();
			user.favourites_count = (*iter)["user"]["favourites_count"].GetInt();
			user.statuses_count = (*iter)["user"]["statuses_count"].GetInt();
			status.user = user;

			statuses.emplace_back(status);
		}
	}
}

TEST(rapidjson, stringify) {
	rapidjson::StringBuffer canadaStringBuffer;
	rapidjson::Writer<rapidjson::StringBuffer> canadaWriter(canadaStringBuffer);
	rapidjsonData.canada.Accept(canadaWriter);
	std::string canadaString = canadaStringBuffer.GetString();

	rapidjson::StringBuffer catalogStringBuffer;
	rapidjson::Writer<rapidjson::StringBuffer> catalogWriter(catalogStringBuffer);
	rapidjsonData.catalog.Accept(catalogWriter);
	std::string catalogString = catalogStringBuffer.GetString();

	rapidjson::StringBuffer twitterStringBuffer;
	rapidjson::Writer<rapidjson::StringBuffer> twitterWriter(twitterStringBuffer);
	rapidjsonData.twitter.Accept(twitterWriter);
	std::string twitterString = twitterStringBuffer.GetString();

}

TEST(rapidjson, prettify) {
	rapidjson::StringBuffer canadaStringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> canadaWriter(canadaStringBuffer);
	rapidjsonData.canada.Accept(canadaWriter);
	std::string canadaString = canadaStringBuffer.GetString();

	rapidjson::StringBuffer catalogStringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> catalogWriter(catalogStringBuffer);
	rapidjsonData.catalog.Accept(catalogWriter);
	std::string catalogString = catalogStringBuffer.GetString();

	rapidjson::StringBuffer twitterStringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> twitterWriter(twitterStringBuffer);
	rapidjsonData.twitter.Accept(twitterWriter);
	std::string twitterString = twitterStringBuffer.GetString();
}

TEST(rapidjson, writeToFiles) {
	std::ofstream canadaStream("Tests/canada.rapidjson.json", std::ios_base::binary | std::ios_base::out);
	rapidjson::StringBuffer canadaStringBuffer;
	rapidjson::Writer<rapidjson::StringBuffer> canadaWriter(canadaStringBuffer);
	rapidjsonData.canada.Accept(canadaWriter);
	std::string canadaString = canadaStringBuffer.GetString();
	canadaStream << canadaString;

	std::ofstream catalogStream("Tests/citm_catalog.rapidjson.json", std::ios_base::binary | std::ios_base::out);
	rapidjson::StringBuffer catalogStringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> catalogWriter(catalogStringBuffer);
	rapidjsonData.catalog.Accept(catalogWriter);
	std::string catalogString = catalogStringBuffer.GetString();
	catalogStream << catalogString;

	std::ofstream twitterStream("Tests/twitter.rapidjson.json", std::ios_base::binary | std::ios_base::out);
	rapidjson::StringBuffer twitterStringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> twitterWriter(twitterStringBuffer);
	rapidjsonData.twitter.Accept(twitterWriter);
	std::string twitterString = twitterStringBuffer.GetString();
	twitterStream << twitterString;
}
