#include <cat/bit>
#include <cat/linux>
#include <cat/page_allocator>
#include <cat/runtime>
#include <cat/string>
#include <cat/thread>

#include "../unit_tests.hpp"

// TODO: Test this without I/O.

void function(void*) {
    for (int4 i = 0; i < 15; ++i) {
        // auto _ = cat::println("Moo?");
    }
}

TEST(test_thread) {
    cat::thread thread;
    cat::page_allocator allocator;
    thread.create(allocator, 2_uki, function, nullptr)
    thread.create(allocator, 2_uki, function)
        .or_exit("Failed to make thread!");
    for (int4 i = 0; i < 10; ++i) {
        // auto _ = cat::println("Boo!");
    }
    thread.join().or_exit("Failed to join thread!");
    // auto _ = cat::println("Finished!");
}
