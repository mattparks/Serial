#pragma once

#include "Node.hpp"

namespace serial {
template<typename T>
T NodeConstView::get() const {
    if (!hasObject())
        return {};

    return _object->get<T>();
}

template<typename T>
T NodeConstView::getWithFallback(const T &fallback) const {
    if (!hasObject())
        return fallback;

    return _object->getWithFallback<T>(fallback);
}

template<typename T>
bool NodeConstView::get(T &dest) const {
    if (!hasObject())
        return false;

    return _object->get<T>(dest);
}

template<typename T, typename K>
bool NodeConstView::getWithFallback(T &dest, const K &fallback) const {
    if (!hasObject()) {
        dest = fallback;
        return false;
    }

    return _object->getWithFallback<T>(dest, fallback);
}

template<typename T>
bool NodeConstView::get(T &&dest) const {
    if (!hasObject())
        return false;

    return _object->get<std::remove_reference_t<T>>(std::move(dest));
}

template<typename T, typename K>
bool NodeConstView::getWithFallback(T &&dest, const K &fallback) const {
    if (!hasObject()) {
        dest = fallback;
        return false;
    }

    return _object->getWithFallback<std::remove_reference_t<T>>(std::move(dest), fallback);
}
}
