#include "benchmark/benchmark.h"
#include "SimpleJSON/JSON.h"
#include "MemoryData.hpp"

struct SimpleJSONData {
    explicit SimpleJSONData(const MemoryData &data) {
        canada = JSON::Parse(data._canada.c_str());
        catalog = JSON::Parse(data._catalog.c_str());
        twitter = JSON::Parse(data._twitter.c_str());
    }
    ~SimpleJSONData() {
        delete twitter;
        delete catalog;
        delete canada;
    }

    JSONValue* canada;
    JSONValue* catalog;
    JSONValue* twitter;
};

static void SimpleJSON_ParseInMemory(benchmark::State& state) {
    MemoryData data;
    for (auto _ : state) {
        SimpleJSONData simplejson(data);
    }
}
BENCHMARK(SimpleJSON_ParseInMemory);

static void SimpleJSON_Stringify(benchmark::State& state) {
    MemoryData data;
    SimpleJSONData simplejson(data);
    for (auto _ : state) {
        auto canadaString = JSON::Stringify(simplejson.canada);
        auto catalogString = JSON::Stringify(simplejson.catalog);
        auto twitterString = JSON::Stringify(simplejson.twitter);
    }
}
BENCHMARK(SimpleJSON_Stringify);

static void SimpleJSON_WriteToFiles(benchmark::State& state) {
    MemoryData data;
    SimpleJSONData simplejson(data);
    for (auto _ : state) {
        std::wofstream canadaStream("Tests/canada.simplejson.json", std::ios_base::binary | std::ios_base::out);
        canadaStream << JSON::Stringify(simplejson.canada);
        std::wofstream catalogStream("Tests/citm_catalog.simplejson.json", std::ios_base::binary | std::ios_base::out);
        catalogStream << JSON::Stringify(simplejson.catalog);
        std::wofstream twitterStream("Tests/twitter.simplejson.json", std::ios_base::binary | std::ios_base::out);
        twitterStream << JSON::Stringify(simplejson.twitter);
    }
}
BENCHMARK(SimpleJSON_WriteToFiles);
