#include "benchmark/benchmark.h"
#include "nlohmann/json.hpp"
#include "MemoryData.hpp"

struct NlohmannData {
    explicit NlohmannData(const MemoryData &data) {
        _canada = nlohmann::json::parse(data._canada.begin(), data._canada.end());
        _catalog = nlohmann::json::parse(data._catalog.begin(), data._catalog.end());
        _twitter = nlohmann::json::parse(data._twitter.begin(), data._twitter.end());
    }

    nlohmann::json _canada;
    nlohmann::json _catalog;
    nlohmann::json _twitter;
};

static void Nlohmann_ParseInMemory(benchmark::State& state) {
    MemoryData data;
    for (auto _ : state) {
        NlohmannData nlohmann(data);
    }
}
BENCHMARK(Nlohmann_ParseInMemory);

static void Nlohmann_Stringify(benchmark::State& state) {
    MemoryData data;
    NlohmannData nlohmann(data);
    for (auto _ : state) {
        auto canadaString = nlohmann._canada.dump();
        auto catalogString = nlohmann._catalog.dump();
        auto twitterString = nlohmann._twitter.dump();
    }
}
BENCHMARK(Nlohmann_Stringify);

static void Nlohmann_Prettify(benchmark::State& state) {
    MemoryData data;
    NlohmannData nlohmann(data);
    for (auto _ : state) {
        auto canadaString = nlohmann._canada.dump(2);
        auto catalogString = nlohmann._catalog.dump(2);
        auto twitterString = nlohmann._twitter.dump(2);
    }
}
BENCHMARK(Nlohmann_Prettify);

static void Nlohmann_WriteToFiles(benchmark::State& state) {
    MemoryData data;
    NlohmannData nlohmann(data);
    for (auto _ : state) {
        std::ofstream canadaStream("Tests/canada.nlohmann.json", std::ios_base::binary | std::ios_base::out);
        canadaStream << nlohmann._canada.dump();
        std::ofstream catalogStream("Tests/citm_catalog.nlohmann.json", std::ios_base::binary | std::ios_base::out);
        catalogStream << nlohmann._catalog.dump(2);
        std::ofstream twitterStream("Tests/twitter.nlohmann.json", std::ios_base::binary | std::ios_base::out);
        twitterStream << nlohmann._twitter.dump(2);
    }
}
BENCHMARK(Nlohmann_WriteToFiles);
