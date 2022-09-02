#include <iostream>
#include <fstream>

#include "Serial/Node.hpp"
#include "Serial/Json.hpp"
#include "Serial/Xml.hpp"
#include "Serial/XmlContainer.hpp"

using namespace serial;

namespace test {
struct Example1 {
    struct XML {
        friend const Node &operator>>(const Node &node, XML &xml) {
            node["data"].get(xml._data);
            node["optional0"].get(xml._optional0);
            node["optional1"].get(xml._optional1);
            return node;
        }

        friend Node &operator<<(Node &node, const XML &xml) {
            node["data"].set(xml._data);
            node["optional0"].set(xml._optional0);
            node["optional1"].set(xml._optional1);
            return node;
        }

        std::vector<std::vector<std::string>> _data = {{"clunky"}, {"uses more words than necessary"}};
        std::optional<float> _optional0;
        std::optional<std::string> _optional1 = "Hello optional string!";
    };

    struct Objects {
        friend const Node &operator>>(const Node &node, Objects &objects) {
            node["url"].get(objects._url);
            node["key"].getWithFallback(objects._key, "failed to get key");
            node["values"].get(objects._values);
            return node;
        }

        friend Node &operator<<(Node &node, const Objects &objects) {
            node["url"].set(objects._url);
            node["key"].set(objects._key);
            node["values"].set(objects._values);
            return node;
        }
        
        std::string _url = R"(<a href=\"http://twitter.com/download/iphone\" rel=\"nofollow\">Twitter for iPhone</a>)";
        std::string _key = "value";
        std::vector<float> _values = {190.0f, 11.0f, -0.001f};
    };

    friend const Node &operator>>(const Node &node, Example1 &example1) {
        node["currentPath"].get(example1._currentPath);
        node["paragraph"].get(example1._paragraph);
        //node["google"].get(example1._google);
        node["content"].get(example1._content);
        node["xml"].get(example1._xml);
        node["json"].get(example1._json);
        node["yaml"].get(example1._yaml);
        node["map"].get(example1._map);
        node["vectorMap"].get(example1._vectorMap);
        node["objects"].get(example1._objects);
        return node;
    }

    friend Node &operator<<(Node &node, const Example1 &example1) {
        node["currentPath"].set(example1._currentPath);
        node["paragraph"].set(example1._paragraph);
        //node["google"].set(example1._google);
        node["content"].set(example1._content);
        node["xml"].set(example1._xml);
        node["json"].set(example1._json);
        node["yaml"].set(example1._yaml);
        node["map"].set(example1._map);
        node["vectorMap"].set(example1._vectorMap);
        node["objects"].set(example1._objects);
        return node;
    }

    XML _xml;
    Objects _objects;
    std::string _paragraph = "\tLorem ipsum dolor sit amet,\nconsectetur adipiscing elit,\nsed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n";
    std::unique_ptr<std::string> _content = std::make_unique<std::string>("Ut enim ad minim veniam,\nquis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.");
    //std::string google = "\"https://google.com\"";

    std::filesystem::path _currentPath = std::filesystem::current_path();
    std::vector<std::string> _json = {"rigid", "better for data interchange"};
    std::vector<std::string> _yaml = {"slim and flexible", "better for configuration", "supports comments"};
    std::map<int32_t, std::string> _map = {{10, "Hello World"}, {-2, "Negative Keys"}, {400, "Larger Key"}};
    std::map<int32_t, std::vector<std::string>> _vectorMap = {{-1, {"A", "B", "C"}}, {8, {"1", "2.00", "3.00"}}, {700, {"%", "$", "#", "&", "#"}}};
};

struct User {
    friend const Node &operator>>(const Node &node, User &user) {
        node["username"].get(user._username);
        node["fullname"].get(user._fullname);
        node["description"].get(user._description);
        node["employed"].get(user._employed);
        node["birthday"].get(user._birthday);
        return node;
    }

    friend Node &operator<<(Node &node, const User &user) {
        node["username"].set(user._username);
        node["fullname"].set(user._fullname);
        node["description"].set(user._description);
        node["employed"].set(user._employed);
        node["birthday"].set(user._birthday);
        return node;
    }

    std::string _username;
    std::string _fullname;
    std::string _description;
    bool _employed = false;
    std::string _birthday;
};
}

int main(int argc, char **argv) {
    test::Example1 example1;
    Node node;
    node = example1;

    // Appends different types into a array.
    node["array1"]->append("Hello", nullptr, 10, 4.8924f);

    // Creates a array, then appends values to the back of the array.
    node["array2"] = std::vector{1.0f, 10.0f, -5.55f, 9.3456f};
    node["array2"]->append(64, 32.1f, -2.0);
    //node["array2"].name("array2_renamed");
    //auto array2Name{node["array2"].name()};
    //auto array2{node["array2"].get<std::vector<float>>()};

    auto timeNow = node["timeNow"].get<int64_t>(123456); // 123456
    node.remove("timeNow");

    auto data00 = node["xml"]["data"][0][0].get<std::string>(); // "clunky"
    auto data10 = node["xml"]["data"][1][0].get<std::string>(); // "uses more words than necessary"

    auto mapN2 = node["map"]["-2"].get<std::string>(); // TODO: Can names be numbers without searching with keys?
    auto map400 = node["map"]["400"].get<std::string>(); // TODO: Can names be numbers without searching with keys?

    if (auto mapN2_ = node["map"]["-2"]) {
        auto value = mapN2_.get<std::string>();
    }

    node["users"][0] = test::User{"user1", "User One", "C++ developer", false, "10/07/2000"};
    node["users"][1] = test::User{"user2", "User Two", "University student", true, "11/05/1999"};
    node["users"][3] = test::User{"user3", "User Three", "Website developer", false, "2/03/1996"};
    Node user4;
    user4 = test::User{"user4", "User Four", "High school student", true, "30/04/2002"};
    node["users"][6] = std::move(user4);
    //node["users"][7] = test::User{"k�ln", "'Etat de S�o Paulo", R"(\"Hello World\")", true, "01/00/2000"};

    // TODO: Resizing a vector changes the internal addressed of each element. 
    // NodeView holds a pointer and will not be aware that the value it holds has been reallocated.
    // In this instance the left side of the assignment will invalidate the right without a resize before assignment.
    //node["users"]->Add(8); 
    //node["users"][8] = node["users"][6];

    Node numbers;
    numbers["a"] = std::vector{1, 2, 3, 4};
    numbers["b"] = std::vector{5, 6, 7, 8};
    numbers["c"] = std::vector{9, 10, 11, 12};
    //numbers["d"] = numbers["a"];
    node["numbers"] = std::move(numbers);
    
    auto users = node["users"].get<std::vector<std::optional<test::User>>>();

    std::filesystem::create_directory("Example");

    {
        // Make a copy of the node.
        Node json1 = node;
        //json1.WriteStream<Json>(std::cout, NodeFormat::Beautified);
    
        // Test Json writer.
        std::ofstream outStream1("Example/Test1.json");
        json1.writeStream<Json>(outStream1, NodeFormat::Beautified);
        outStream1.close();

        // Test Json reader.
        std::ifstream inStream1("Example/Test1.json");
        Node json2;
        json2.parseStream<Json>(inStream1);
        inStream1.close();
        
        // Ensure Test1.json and Test2.json values are the same (ignore order changes).
        std::ofstream outStream2("Example/Test2.json");
        json2.writeStream<Json>(outStream2, NodeFormat::Beautified);
        outStream2.close();
    }
    {
        Node xml1;
        // Make a copy of the node.
        xml1["node"] = node;

        // Test Xml writer.
        std::ofstream outStream1("Example/Xml1.xml");
        xml1.writeStream<Xml>(outStream1, NodeFormat::Beautified);
        outStream1.close();

        /*// Test Xml reader.
        std::ifstream inStream1("Example/Xml1.xml");
        Node xml2;
        xml2.parseStream<Xml>(inStream1);
        inStream1.close();

        // Ensure Test1.json and Test2.json values are the same (ignore order changes).
        std::ofstream outStream2("Example/Xml1.json");
        xml2.writeStream<Xml>(outStream2, NodeFormat::Beautified);
        outStream2.close();*/
    }
    /*{
        // Make a copy of the node.
        Node yaml1 = node;
        //yaml1.writeStream<Yaml>(std::cout, NodeFormat::Beautified);

        // Test Yaml writer.
        std::ofstream outStream1("Example/Yaml1.yaml");
        node.writeStream<Yaml>(outStream1);
    }*/
    /*{
        Node xml1;

        // Test Xml reader.
        std::ifstream inStream1("41, 41-1048, Day 1, 2 Hours Post-Dose, 20Feb2020, 12.40.xml");
        xml1.parseStream<Xml>(inStream1);
        inStream1.close();

        auto components = *xml1["AnnotatedECG"]["component"]["series"]["component"].get<XmlContainer<std::vector<Node>>>();

        std::vector<Node> components1;
        xml1["AnnotatedECG"]["component"]["series"]["component"].getWithFallback(XmlContainerRef(components1), std::vector<Node>{});

        // Test Json writer.
        std::ofstream outStream1("41, 41-1048, Day 1, 2 Hours Post-Dose, 20Feb2020, 12.40.json");
        xml1.writeStream<Json>(outStream1, NodeFormat::Beautified);
        outStream1.close();
    }*/
    
    //std::cout << "\nPress enter to continue...";
    //std::cin.get();
    return 0;
}
