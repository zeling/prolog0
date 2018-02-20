#include <sys/mman.h>
#include <assert.h>
#include <unistd.h>

#include "wam.h"

void mmap_deleter::operator()(void *p) {
    assert(::munmap(p, size) == 0);
}

mmap_protected_area::mmap_protected_area(unsigned int pagenum) {
    size_t pagesize = getpagesize();
    char *base = static_cast<char *>(::mmap(0, (pagenum + 2) * pagesize, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, 0, 0));
    assert(base != MAP_FAILED);
    assert(reinterpret_cast<uintptr_t>(base) % pagesize == 0); // should align;
    assert(::mprotect(base, pagesize, PROT_NONE) == 0);
    assert(::mprotect(base + (pagenum + 1) * pagesize, pagesize, PROT_NONE) == 0);
    real = base + pagesize;
}


