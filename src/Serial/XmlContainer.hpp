#pragma once

#include "Node.hpp"

namespace serial {
/**
 * @brief Class that allows {@link Node::get()} to handle returning XML container types (std::vector, std::array)
 * where the XML array may contain 1 or n objects.
 */
template<typename T, typename = std::enable_if_t<utils::is_container_v<std::remove_reference_t<T>>>>
class XmlContainer {
public:
    constexpr XmlContainer(const T &value = {}) : _value(value) {}
    template<typename T1 = T, typename = std::enable_if_t<!std::is_reference_v<T1>>>
    constexpr XmlContainer(T1 &&value) : _value(std::move(value)) {}

    friend const Node &operator>>(const Node &node, XmlContainer<T> &object) {
        if (node.type() == NodeType::Array)
            return node >> object.value;
        return node >> object.value.emplace_back();
    }

    friend Node &operator<<(Node &node, const XmlContainer<T> &object) {
        return node << object.value;
    }

    T operator*() noexcept { return std::move(_value); }
    operator T () noexcept { return std::move(_value); }

    template<typename K = std::remove_reference_t<T>>
    constexpr XmlContainer &operator=(const K &value) noexcept {
        _value = value;
        return *this;
    }

    template<typename K = std::remove_reference_t<T>>
    constexpr XmlContainer &operator=(K &&value) noexcept {
        _value = std::move(value);
        return *this;
    }

protected:
    T _value;
};

template<typename T>
class XmlContainerRef : public XmlContainer<T &> {
public:
    constexpr XmlContainerRef(T &value) : XmlContainer<T &>(value) {}

    operator const T &() const { return this->_value; }
    const T &operator*() const { return this->_value; }

    template<typename K>
    constexpr XmlContainerRef &operator=(const K &value) noexcept {
        this->_value = value;
        return *this;
    }

    template<typename K>
    constexpr XmlContainerRef &operator=(K &&value) noexcept {
        this->_value = std::move(value);
        return *this;
    }
};

}
