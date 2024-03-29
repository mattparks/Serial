#include "benchmark/benchmark.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "MemoryData.hpp"

struct RapidJsonData {
    explicit RapidJsonData(const MemoryData &data) {
        _canada.Parse(data._canada.c_str());
        _catalog.Parse(data._catalog.c_str());
        _twitter.Parse(data._twitter.c_str());
    }

    rapidjson::Document _canada;
    rapidjson::Document _catalog;
    rapidjson::Document _twitter;
};

static void RapidJson_ParseInMemory(benchmark::State& state) {
    MemoryData data;
    for (auto _ : state) {
        RapidJsonData rapidjson(data);
    }
}
BENCHMARK(RapidJson_ParseInMemory);

static void RapidJson_GetTwitterData(benchmark::State& state) {
    MemoryData data;
    RapidJsonData rapidjson(data);
    for (auto _ : state) {
        auto ok = rapidjson._twitter.GetParseError();
        if (ok == rapidjson::kParseErrorNone) {
            std::vector<Status> statuses;
            rapidjson::GenericValue<rapidjson::UTF8<>> &s = rapidjson._twitter["statuses"];
            for (rapidjson::Value::ConstValueIterator iter = s.Begin(); iter != s.End(); ++iter) {
                Status status;
                status._id = (*iter)["id"].GetInt();
                status._idStr = (*iter)["id_str"].GetString();

                Status::User user;
                user._id = (*iter)["user"]["id"].GetInt();
                user._idStr = (*iter)["user"]["id_str"].GetString();

                user._name = (*iter)["user"]["name"].GetString();
                user._screenName = (*iter)["user"]["screen_name"].GetString();
                user._location = (*iter)["user"]["location"].GetString();
                user._description = (*iter)["user"]["description"].GetString();
                user._followersCount = (*iter)["user"]["followers_count"].GetInt();
                user._friendsCount = (*iter)["user"]["friends_count"].GetInt();
                user._listedCount = (*iter)["user"]["listed_count"].GetInt();
                user._favouritesCount = (*iter)["user"]["favourites_count"].GetInt();
                user._statusesCount = (*iter)["user"]["statuses_count"].GetInt();
                status._user = user;

                statuses.emplace_back(status);
            }
        }
    }
}
BENCHMARK(RapidJson_GetTwitterData);

static void RapidJson_Stringify(benchmark::State& state) {
    MemoryData data;
    RapidJsonData rapidjson(data);
    for (auto _ : state) {
        rapidjson::StringBuffer canadaStringBuffer;
        rapidjson::Writer<rapidjson::StringBuffer> canadaWriter(canadaStringBuffer);
        rapidjson._canada.Accept(canadaWriter);
        std::string canadaString = canadaStringBuffer.GetString();

        rapidjson::StringBuffer catalogStringBuffer;
        rapidjson::Writer<rapidjson::StringBuffer> catalogWriter(catalogStringBuffer);
        rapidjson._catalog.Accept(catalogWriter);
        std::string catalogString = catalogStringBuffer.GetString();

        rapidjson::StringBuffer twitterStringBuffer;
        rapidjson::Writer<rapidjson::StringBuffer> twitterWriter(twitterStringBuffer);
        rapidjson._twitter.Accept(twitterWriter);
        std::string twitterString = twitterStringBuffer.GetString();
    }
}
BENCHMARK(RapidJson_Stringify);

static void RapidJson_Prettify(benchmark::State& state) {
    MemoryData data;
    RapidJsonData rapidjson(data);
    for (auto _ : state) {
        rapidjson::StringBuffer canadaStringBuffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> canadaWriter(canadaStringBuffer);
        rapidjson._canada.Accept(canadaWriter);
        std::string canadaString = canadaStringBuffer.GetString();

        rapidjson::StringBuffer catalogStringBuffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> catalogWriter(catalogStringBuffer);
        rapidjson._catalog.Accept(catalogWriter);
        std::string catalogString = catalogStringBuffer.GetString();

        rapidjson::StringBuffer twitterStringBuffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> twitterWriter(twitterStringBuffer);
        rapidjson._twitter.Accept(twitterWriter);
        std::string twitterString = twitterStringBuffer.GetString();
    }
}
BENCHMARK(RapidJson_Prettify);

static void RapidJson_WriteToFiles(benchmark::State& state) {
    MemoryData data;
    RapidJsonData rapidjson(data);
    for (auto _ : state) {
        std::ofstream canadaStream("Tests/canada.rapidjson.json", std::ios_base::binary | std::ios_base::out);
        rapidjson::StringBuffer canadaStringBuffer;
        rapidjson::Writer<rapidjson::StringBuffer> canadaWriter(canadaStringBuffer);
        rapidjson._canada.Accept(canadaWriter);
        std::string canadaString = canadaStringBuffer.GetString();
        canadaStream << canadaString;

        std::ofstream catalogStream("Tests/citm_catalog.rapidjson.json", std::ios_base::binary | std::ios_base::out);
        rapidjson::StringBuffer catalogStringBuffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> catalogWriter(catalogStringBuffer);
        rapidjson._catalog.Accept(catalogWriter);
        std::string catalogString = catalogStringBuffer.GetString();
        catalogStream << catalogString;

        std::ofstream twitterStream("Tests/twitter.rapidjson.json", std::ios_base::binary | std::ios_base::out);
        rapidjson::StringBuffer twitterStringBuffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> twitterWriter(twitterStringBuffer);
        rapidjson._twitter.Accept(twitterWriter);
        std::string twitterString = twitterStringBuffer.GetString();
        twitterStream << twitterString;
    }
}
BENCHMARK(RapidJson_WriteToFiles);
