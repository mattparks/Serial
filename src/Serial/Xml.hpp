#pragma once

#include <vector>

#include "Node.hpp"

namespace serial {

class Xml : public NodeFormatType<Xml> {
protected:
    friend class Node;
    friend class NodeFormatType<Xml>;

    constexpr static char AttributePrefix = '@';

    static void Load(Node &node, std::string_view string);
    static void Write(const Node &node, std::ostream &stream, Format format = Minified);

private:
    static void AddToken(std::string_view view, std::vector<Token> &tokens);
    static void Convert(Node &current, const std::vector<Token> &tokens, int32_t &k);
    static Node &CreateProperty(Node &current, const std::string &name);

    static void AppendData(const std::string &nodeName, const Node &node, std::ostream &stream, Format format, int32_t indent);
};

}
