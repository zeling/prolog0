#pragma once

#include <utility>

template <typename Impl>
class pimpl {
protected:
    Impl *_impl;

    using super_t = pimpl<Impl>;
    using impl_t = Impl;

    Impl *impl() {
        return _impl;
    }

public:

    pimpl() = default;
    pimpl(Impl *impl): _impl(impl) {}
    pimpl(const pimpl &) = delete;
    pimpl(pimpl &&that) : _impl(that._impl) {
        that._impl = nullptr;
    }

    pimpl &operator=(const pimpl &) = delete;
    pimpl &operator=(pimpl &&rhs) {
        std::swap(*this, rhs);
    }

    ~pimpl() {
        if (_impl) {
            delete _impl;
        }
    }
};
