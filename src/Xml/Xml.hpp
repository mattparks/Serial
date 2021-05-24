#pragma once

#include <vector>

#include "Node.hpp"

namespace serial {
template<typename T, std::enable_if_t<is_container_v<T>, int> = 1>
class XmlVector {
public:
	friend const Node &operator>>(const Node &node, XmlVector &xmlVector) {
		if (node.GetType() == NodeType::Array)
			return node >> xmlVector.value;
		return node >> xmlVector.value.emplace_back();
	}

	T *get() noexcept { return &value; }
	T operator*() noexcept { return std::move(value); }
	operator T () noexcept { return std::move(value); }

private:
	T value;
};

// TODO: XmlVectorRef for Node::Get(T &)

class Xml : public NodeFormatType<Xml> {
public:
	// Do not call Load and Write directly, use Node::ParseString<Xml> and Node::WriteStream<Xml>.
	static void Load(Node &node, std::string_view string);
	static void Write(const Node &node, std::ostream &stream, Format format = Minified);

private:
	static void AddToken(std::string_view view, std::vector<Token> &tokens);
	static void Convert(Node &current, const std::vector<Token> &tokens, int32_t &k);
	static Node &CreateProperty(Node &current, const std::string &name);
	
	static void AppendData(const NodeKey &nodeKey, const Node &node, std::ostream &stream, Format format, int32_t indent);
};
}
