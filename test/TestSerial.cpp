#include "benchmark/benchmark.h"
#include "Serial/Json.hpp"
#include "MemoryData.hpp"

struct SerialData {
    explicit SerialData(const MemoryData &data) {
        _canada.parseString<serial::Json>(data._canada);
        _catalog.parseString<serial::Json>(data._catalog);
        _twitter.parseString<serial::Json>(data._twitter);
    }

    serial::Node _canada;
    serial::Node _catalog;
    serial::Node _twitter;
};

static void Serial_ParseInMemory(benchmark::State& state) {
    MemoryData data;
    for (auto _ : state) {
        SerialData serial(data);
    }
}
BENCHMARK(Serial_ParseInMemory);

inline const serial::Node &operator>>(const serial::Node &node, Status &status) {
    node["id"]->get(status._id);
    node["id_str"]->get(status._idStr);
    node["user"]->get(status._user);
    return node;
}

inline const serial::Node &operator>>(const serial::Node &node, Status::User &user) {
    node["id"]->get(user._id);
    node["id_str"]->get(user._idStr);
    
    node["name"]->get(user._name);
    node["screen_name"]->get(user._screenName);
    node["location"]->get(user._description);
    node["followers_count"]->get(user._followersCount);
    node["friends_count"]->get(user._friendsCount);
    node["listed_count"]->get(user._listedCount);
    node["favourites_count"]->get(user._favouritesCount);
    node["statuses_count"]->get(user._statusesCount);

    return node;
}

static void Serial_GetTwitterData(benchmark::State& state) {
    MemoryData data;
    SerialData serial(data);
    for (auto _ : state) {
        auto statuses = serial._twitter["statuses"]->get<std::vector<Status>>();
    }
}
BENCHMARK(Serial_GetTwitterData);

static void Serial_Stringify(benchmark::State& state) {
    MemoryData data;
    SerialData serial(data);
    for (auto _ : state) {
        auto canadaString = serial._canada.writeString<serial::Json>();
        auto catalogString = serial._catalog.writeString<serial::Json>();
        auto twitterString = serial._twitter.writeString<serial::Json>();
    }
}
BENCHMARK(Serial_Stringify);

static void Serial_Prettify(benchmark::State& state) {
    MemoryData data;
    SerialData serial(data);
    for (auto _ : state) {
        auto canadaString = serial._canada.writeString<serial::Json>(serial::NodeFormat::Beautified);
        auto catalogString = serial._catalog.writeString<serial::Json>(serial::NodeFormat::Beautified);
        auto twitterString = serial._twitter.writeString<serial::Json>(serial::NodeFormat::Beautified);
    }
}
BENCHMARK(Serial_Prettify);

static void Serial_WriteToFiles(benchmark::State& state) {
    MemoryData data;
    SerialData serial(data);
    for (auto _ : state) {
        std::ofstream canadaFile("Tests/canada.serial.json", std::ios_base::binary | std::ios_base::out);
        serial._canada.writeStream<serial::Json>(canadaFile);
        std::ofstream catalogFile("Tests/citm_catalog.serial.json", std::ios_base::binary | std::ios_base::out);
        serial._catalog.writeStream<serial::Json>(catalogFile, serial::NodeFormat::Beautified);
        std::ofstream twitterFile("Tests/twitter.serial.json", std::ios_base::binary | std::ios_base::out);
        serial._twitter.writeStream<serial::Json>(twitterFile, serial::NodeFormat::Beautified);
    }
}
BENCHMARK(Serial_WriteToFiles);
