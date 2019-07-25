#include <iostream>

#include <fstream>
#include "Serialized/Node.hpp"
#include "Serialized/Json/Json.hpp"
#include "Serialized/Yaml/Yaml.hpp"
#include <unordered_map>

using namespace acid;

namespace test
{
/*class NonCopyable
{
protected:
	NonCopyable() = default;

	virtual ~NonCopyable() = default;

public:
	NonCopyable(const NonCopyable &) = delete;

	NonCopyable(NonCopyable &&) = default;

	NonCopyable &operator=(const NonCopyable &) = delete;

	NonCopyable &operator=(NonCopyable &&) = default;
};

class ModuleDetail
{
public:
	enum class Stage
	{
		Always, Pre, Normal, Post, Render
	};

	virtual ~ModuleDetail() = default;

	virtual void Update() = 0;
};

template<typename T>
class Module :
	public NonCopyable,
	public ModuleDetail
{
public:
	static T *Get()
	{
		//if (Instance == nullptr)
		//{
		//	Instance = new T;
		//}

		return Instance;
	}

	virtual void Update() = 0;

private:
	friend class ModuleHolder;
	static inline T *Instance = nullptr;
};

class Display :
	public Module<Display>
{
public:
	~Display()
	{
		std::cout << "Deleted Display\n";
	}

	void Update() override
	{
	}

	float w{1080.0f};
	float h{720.0f};
};

class ModuleHolder
{
public:
	ModuleHolder()
	{
		Display::Instance = new Display;
		m_modules[0] = reinterpret_cast<ModuleDetail **>(&Display::Instance);
	}

	~ModuleHolder()
	{
		// TODO: Why no rbegin or rend methods in std::unordered_map?
		for (auto &[id, module] : m_modules)
		{
			delete *module;
			module = nullptr;
		}

		std::cout << "Deleted ModuleHolder\n";
	}

	std::unordered_map<uint32_t, ModuleDetail **> m_modules;
};*/

class Example1
{
public:
	class XML
	{
	public:
		std::vector<std::vector<std::string>> data{{"clunky"}, {"uses more words than necessary"}};
		std::optional<float> optional0{};
		std::optional<std::string> optional1{"Hello optional string!"};

		friend const Node &operator>>(const Node &node, XML &xml)
		{
			node["data"].Get(xml.data);
			node["optional0"].Get(xml.optional0);
			node["optional1"].Get(xml.optional1);
			return node;
		}

		friend Node &operator<<(Node &node, const XML &xml)
		{
			node["data"].Set(xml.data);
			node["optional0"].Set(xml.optional0);
			node["optional1"].Set(xml.optional1);
			return node;
		}
	} xml;

	class Objects
	{
	public:
		std::string key{"value"};
		std::vector<float> values{190.0f, 11.0f, -0.001f};

		friend const Node &operator>>(const Node &node, Objects &objects)
		{
			node["key"].Get(objects.key, "failed to get key");
			node["values"].Get(objects.values);
			return node;
		}

		friend Node &operator<<(Node &node, const Objects &objects)
		{
			node["key"].Set(objects.key);
			node["values"].Set(objects.values);
			return node;
		}
	} objects;

	std::string paragraph{"Lorem ipsum dolor sit amet,\nconsectetur adipiscing elit,\nsed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"};
	std::unique_ptr<std::string> content{std::make_unique<std::string>("Ut enim ad minim veniam,\nquis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.")};
	//std::string google{"\"https://google.com\""};

	std::filesystem::path currentPath{std::filesystem::current_path()};
	std::vector<std::string> json{"rigid", "better for data interchange"};
	std::vector<std::string> yaml{"slim and flexible", "better for configuration", "supports comments"};
	std::map<int32_t, std::string> map{{10, "Hello World"}, {-2, "Negative Keys"}, {400, "Larger Key"}};
	std::map<int32_t, std::vector<std::string>> vectorMap{{-1, {"A", "B", "C"}}, {8, {"1", "2.00", "3.00"}}, {700, {"%", "$", "#", "&", "#"}}};

	friend const Node &operator>>(const Node &node, Example1 &example1)
	{
		node["currentPath"].Get(example1.currentPath);
		node["paragraph"].Get(example1.paragraph);
		//node["google"].Get(example1.google);
		node["content"].Get(example1.content);
		node["xml"].Get(example1.xml);
		node["json"].Get(example1.json);
		node["yaml"].Get(example1.yaml);
		node["map"].Get(example1.map);
		node["vectorMap"].Get(example1.vectorMap);
		node["objects"].Get(example1.objects);
		return node;
	}

	friend Node &operator<<(Node &node, const Example1 &example1)
	{
		node["currentPath"].Set(example1.currentPath);
		node["paragraph"].Set(example1.paragraph);
		//node["google"].Set(example1.google);
		node["content"].Set(example1.content);
		node["xml"].Set(example1.xml);
		node["json"].Set(example1.json);
		node["yaml"].Set(example1.yaml);
		node["map"].Set(example1.map);
		node["vectorMap"].Set(example1.vectorMap);
		node["objects"].Set(example1.objects);
		return node;
	}
};

class User
{
public:
	std::string username;
	std::string fullname;
	std::string description;
	bool employed{};
	std::string birthday;

	friend const Node &operator>>(const Node &node, User &user)
	{
		node["username"].Get(user.username);
		node["fullname"].Get(user.fullname);
		node["description"].Get(user.description);
		node["employed"].Get(user.employed);
		node["birthday"].Get(user.birthday);
		return node;
	}

	friend Node &operator<<(Node &node, const User &user)
	{
		node["username"].Set(user.username);
		node["fullname"].Set(user.fullname);
		node["description"].Set(user.description);
		node["employed"].Set(user.employed);
		node["birthday"].Set(user.birthday);
		return node;
	}
};

/*class Resource
{
	template<typename T>
	friend std::enable_if_t<std::is_base_of_v<Resource, T>, const Node &> operator>>(const Node &node, std::shared_ptr<T> &object)
	{
		object = T::Create(node);
		std::cout << "Created Resource from Node\n";
		return node;
	}
};

class ResourceImage :
	public Resource
{
public:
	static std::shared_ptr<ResourceImage> Create(const std::filesystem::path &path)
	{
		return std::make_shared<ResourceImage>(path);
	}

	static std::shared_ptr<ResourceImage> Create(const Node &node)
	{
		auto result{std::make_shared<ResourceImage>()};
		node >> *result;
		return result;
	}

	ResourceImage() = default;

	ResourceImage(std::filesystem::path path) :
		m_path{std::move(path)}
	{
	}

	friend const Node &operator>>(const Node &node, ResourceImage &image)
	{
		node["path"].Get(image.m_path);
		return node;
	}

	friend Node &operator<<(Node &node, const ResourceImage &image)
	{
		node["path"].Set(image.m_path);
		return node;
	}

	std::filesystem::path m_path;
};*/
}

int main(int argc, char **argv)
{
	/*Json canada;
	Json catalog;
	Json twitter;

	{
		auto start{std::chrono::high_resolution_clock::now()};

		canada.Load(std::ifstream{"canada.json"});
		catalog.Load(std::ifstream{"citm_catalog.json"});
		twitter.Load(std::ifstream{"twitter.json"});

		auto length{std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start)};
		std::cout << "Loaded in " << length.count() << "ms\n";
	}

	{
		auto start{std::chrono::high_resolution_clock::now()};

		canada.Write(std::ofstream{"canada.1.json"});
		catalog.Write(std::ofstream{"citm_catalog.1.json"});
		twitter.Write(std::ofstream{"twitter.1.json"});

		auto length{std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start)};
		std::cout << "Written in " << length.count() << "ms\n";
	}*/

	/*{
		test::ModuleHolder moduleHolder;
		auto display{test::Display::Get()};
	}*/

	test::Example1 example1;
	Node node;
	node = example1;

	// Appends different types into a array.
	node["array1"]->Append("Hello", nullptr, 10, 4.8924f);

	// Creates a array, then appends values to the back of the array.
	node["array2"] = std::vector{1.0f, 10.0f, -5.55f, 9.3456f};
	node["array2"]->Append(64, 32.1f, -2.0);
	//node["array2"].SetName("array2_renamed");
	//auto array2Name{node["array2"].GetName()};
	//auto array2{node["array2"].Get<std::vector<float>>()};

	auto timeNow{node["timeNow"].Get<int64_t>(123456)}; // 123456
	node.RemoveProperty("timeNow");

	auto data00{node["xml"]["data"][0][0].Get<std::string>()}; // "clunky"
	auto data10{node["xml"]["data"][1][0].Get<std::string>()}; // "uses more words than necessary"

	auto mapN2{node["map"]["-2"].Get<std::string>()}; // TODO: Can names be numbers without searching with keys?
	auto map400{node["map"]["400"].Get<std::string>()}; // TODO: Can names be numbers without searching with keys?

	if (auto mapN2{node["map"]["-2"]}; mapN2)
	{
		auto value{mapN2.Get<std::string>()};
	}

	node["users"][0] = test::User{"mattparks", "Matthew Albrecht", "C++ developer", false, "12/07/2000"};
	node["users"][1] = test::User{"nettcod", "Cody Nettesheim", "University student", true, "11/03/1999"};
	node["users"][3] = test::User{"blockhead", "Nick Block", "Website developer", false, "11/03/1996"};
	node["users"][6] = test::User{"aaronphal", "Aaron Phalphouvong", "High school student", true, "11/03/2002"};
	auto users{node["users"].Get<std::vector<std::optional<test::User>>>()};

	Json json1{node};

	Json json2;
	json2.Load(json1.Write(Node::Format::Minified));
	//json2.Write(std::cout, Node::Format::Beautified);
	{
		std::ofstream testStream{"Test.json"};
		json2.Write(testStream);
	}

	/*Yaml yaml2{node};
	//yaml2.Write(std::cout, Node::Format::Beautified);
	yaml2.Write(std::ofstream{"Test.yml"});*/

	/*{
		Node node0;
		auto i0{test::ResourceImage::Create(std::filesystem::current_path() / "Example0")};
		auto i1{test::ResourceImage::Create(std::filesystem::current_path() / "Example1")};
		node0.Append(i0, i1);
		Json json{node0};
		std::ofstream resourceStream{"Resource.json"};
		json.Write(resourceStream);
		auto o0{node0[0].Get<std::shared_ptr<test::ResourceImage>>()};
		std::cout << o0->m_path << '\n';
	}*/

	//std::cout << "\nPress enter to continue...";
	//std::cin.get();
	return 0;
}
