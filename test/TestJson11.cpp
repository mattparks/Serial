#include "benchmark/benchmark.h"
#include "json11/json11.hpp"
#include "MemoryData.hpp"

struct Json11Data {
    explicit Json11Data(const MemoryData &data) {
        std::string err;
        _canada = json11::Json::parse(data._canada.data(), err);
        _catalog = json11::Json::parse(data._catalog.data(), err);
        _twitter = json11::Json::parse(data._twitter.data(), err);
    }

    json11::Json _canada;
    json11::Json _catalog;
    json11::Json _twitter;
};

static void Json11_ParseInMemory(benchmark::State& state) {
    MemoryData data;
    for (auto _ : state) {
        Json11Data json11(data);
    }
}
BENCHMARK(Json11_ParseInMemory);

static void Json11_Stringify(benchmark::State& state) {
    MemoryData data;
    Json11Data json11(data);
    for (auto _ : state) {
        std::string canadaString = json11._canada.dump();
        std::string catalogString = json11._catalog.dump();
        std::string twitterString = json11._twitter.dump();
    }
}
BENCHMARK(Json11_Stringify);

static void Json11_WriteToFiles(benchmark::State& state) {
    MemoryData data;
    Json11Data json11(data);
    for (auto _ : state) {
        std::ofstream canadaStream("Tests/canada.json11.json", std::ios_base::binary | std::ios_base::out);
        canadaStream << json11._canada.dump();
        std::ofstream catalogStream("Tests/citm_catalog.json11.json", std::ios_base::binary | std::ios_base::out);
        catalogStream << json11._catalog.dump();
        std::ofstream twitterStream("Tests/twitter.json11.json", std::ios_base::binary | std::ios_base::out);
        twitterStream << json11._twitter.dump();
    }
}
BENCHMARK(Json11_WriteToFiles);
