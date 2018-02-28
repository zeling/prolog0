#pragma once

#include <type_traits>

template <typename T, typename U, typename = std::enable_if_t<std::is_base_of<U, T>::value>>
bool isa(U *u) {
    return !u || T::classof(u);
};

template <typename T, typename U, typename = std::enable_if_t<std::is_base_of<U, T>::value>>
T *dyn_cast(U *u) {
    if (isa<T>(u)) {
        return static_cast<T *>(u);
    } else {
        return nullptr;
    }
};
