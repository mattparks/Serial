#pragma once

#include "Node.hpp"

namespace serial {
template<typename T>
void NodeView::set(const T &value) {
    object()->set<T>(value);
}

template<typename T>
void NodeView::set(T &&value) {
    object()->set<std::remove_reference_t<T>>(std::move(value));
}

template<typename T>
Node &NodeView::operator=(const T &rhs) {
    return *object() = rhs;
}

template<typename T>
Node &NodeView::operator=(T &&rhs) {
    return *object() = std::move(rhs);
}
}
