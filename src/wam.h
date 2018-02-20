#pragma once

#include <memory>

struct mmap_deleter {
    size_t size;
    void operator()(void *p);
};

using mmap_area = std::unique_ptr<char, mmap_deleter>;

struct mmap_protected_area {
    mmap_area _protected;
    char *real;
    mmap_protected_area(unsigned int pagenum);
};

class wam {
    struct impl {
        mmap_protected_area heap;
        impl(unsigned int heap_pagenum = 1) : heap(heap_pagenum) {}
    };

    std::unique_ptr<impl> _impl;

public:
    wam(): _impl(std::make_unique<impl>()) {}
    wam(const wam &) = delete;
    wam(wam &&) = default;
    wam &operator=(const wam &) = delete;
    wam &operator=(wam &&) = default;

    char * real_heap_base() {
        return _impl->heap.real;
    }
};
