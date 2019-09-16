#include <fstream>

#include <gtest/gtest.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "rapidjson/prettywriter.h"

#include "MemoryData.hpp"

static class RapidJsonData {
public:
	rapidjson::Document canada;
	rapidjson::Document catalog;
	rapidjson::Document twitter;
} rapidjsonData;

TEST(rapidjson, parseInMemory) {
	rapidjsonData.canada.Parse(memoryData.canadaString.c_str());
	rapidjsonData.catalog.Parse(memoryData.catalogString.c_str());
	rapidjsonData.twitter.Parse(memoryData.twitterString.c_str());
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
