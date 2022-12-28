#include <cat/memory>
#include <cat/page_allocator>

#include "../unit_tests.hpp"

TEST(test_set_memory) {
    cat::page_allocator allocator;
    uint1* p_page = allocator.alloc<uint1>().or_exit();

    // TODO: Make a `cat::compare_memory()` or something for this.

    cat::set_memory(p_page, 1u1, 4_ki);
    cat::verify(p_page[1000] == 1u1);

    // Test that unaligned memory still sets correctly.
    cat::set_memory(p_page + 3, 2u1, 2_ki - 6);
    cat::verify(p_page[0] == 1u1);
    cat::verify(p_page[2] == 1u1);
    cat::verify(p_page[3] == 2u1);
    // TODO: Why did this stop working?
    // cat::verify(p_page[(2_ki - 4).raw] == 2u1);
    cat::verify(p_page[(2_ki - 3).raw] == 1u1);

    // Test zeroing out memory.
    cat::zero_memory(p_page, 4_ki);
    cat::verify(p_page[0] == 0u1);
    cat::verify(p_page[(4_ki).raw - 1] == 0u1);

    // Test setting values larger than 1 byte.
    cat::set_memory(p_page, 1i2, 2_ki);
    cat::verify(static_cast<int2*>(static_cast<void*>(p_page))[10] == 1i2);
    // The next byte after this should be 0.
    cat::verify(static_cast<int1*>(static_cast<void*>(p_page))[21] == 0);

    cat::set_memory(p_page, 1i4, 1_ki);
    cat::verify(static_cast<int4*>(static_cast<void*>(p_page))[10] == 1i4);

    cat::set_memory(p_page, 1i8, 0.5_ki);
    cat::verify(static_cast<int4*>(static_cast<void*>(p_page))[10] == 1i4);

    // Test scalar `set_memory()`.
    cat::set_memory_scalar(p_page, 1u1, 4_ki);
    cat::verify(p_page[1001] == 1u1);

    // Test scalar `zero_memory()`.
    cat::zero_memory_scalar(p_page, 4_ki);
    cat::verify(p_page[1001] == 0u1);
};
