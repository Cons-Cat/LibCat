#include <cat/array>
#include <cat/math>
#include <cat/runtime>
#include <cat/string>
#include <cat/utility>

void meow() {
    cat::align_stack_pointer_32();

    // Initializing a array:
    cat::Array<int4, 5> array_1 = {0, 1, 2, 3, 4};
    // Assigning a array:
    array_1 = {5, 6, 7, 8, 9};
    // Default initializing a array:
    cat::Array<int4, 1> array_2;
    // Move assigning a array:
    cat::Array<int4, 1> array_3;
    array_2 = cat::move(array_3);  // NOLINT
    // Move constructing a array:
    _ = cat::move(array_1);  // NOLINT

    // `const` array.
    cat::Array<int4, 3> const array_const = {0, 1, 2};
    [[maybe_unused]] int4 const_val = array_const.at(1).value();

    // Repeat those tests in a constexpr context.
    auto constant = []() constexpr {
        cat::Array<int4, 1> const_array_1;
        cat::Array<int4, 1> const_array_2 = {1};
        // NOLINTNEXTLINE Just be explicit about the move here.
        _ = cat::move(const_array_1);
        // NOLINTNEXTLINE Just be explicit about the move here.
        _ = cat::move(const_array_2);
    };
    _ = cat::constant_evaluate(constant);

    // Test iterable.
    using It = cat::CollectionTraits<cat::Array<int, 4>>::Iterator;
    static_assert(
        cat::is_same<It,
                     cat::RemoveCvRef<decltype(cat::Array<int, 4>{}.begin())>>);

    ssize count = 0;
    for (int4& a : array_1) {
        Result(a == array_1[count]).or_panic();
        count++;
    }

    for (int4 const& a : cat::AsReverse(array_1)) {
        count--;
        Result(a == array_1[count]).or_panic();
    }

    Result(array_1.front() == 5).or_panic();
    Result(array_1.back() == 9).or_panic();

    count = 0;
    for (int4 const& a : cat::AsConst(array_1)) {
        Result(a == array_1[count]).or_panic();
        count++;
    }
    _ = array_1.cbegin();

    for (int4 const& a : cat::AsConstReverse(array_1)) {
        count--;
        Result(a == array_1[count]).or_panic();
    }

    // Index in and out of bounds.
    Result(array_1.at(0).value() == 5).or_panic();
    Result(!array_1.at(6).has_value()).or_panic();

    // Deducing type.
    cat::Array implicit_array_1 = {0, 1, 2, 3, 4};
    cat::Array implicit_array_2{0, 1, 2, 3, 4};
    cat::Array implicit_array_3(0, 1, 2, 3, 4);
    static_assert(implicit_array_1.size() == 5);
    static_assert(implicit_array_2.size() == 5);
    static_assert(implicit_array_3.size() == 5);

    // Max elements.
    constexpr cat::Array array_4 = {0, 2, 8, 5};
    constexpr int4 max_1 = cat::max(array_4);
    Result(max_1 == 8).or_panic();

    int4 min_1 = cat::min(array_4);
    Result(min_1 == 0).or_panic();

    // TODO: String deduction:
    //     cat::Array implicit_string = "Hi, Conscat!";
    // static_assert(implicit_string.size() ==
    //               cat::string_length("Hi, Conscat!"));

    // TODO: Test `constexpr`.

    // Slicing array.
    [[maybe_unused]] cat::Span span = array_1.first(1);
    _ = array_1.subspan(0, 2);
    _ = array_1.last(2);

    [[maybe_unused]] cat::Span const span_const = array_1.first(1);
    _ = array_const.subspan(0, 2);
    _ = array_const.last(2);

    // Test `IntegerList`.
    cat::Array<int4, 5> int_array = cat::value_list<int4, 0, 5>;
    for (int4& i : int_array) {
        Result(i == 0).or_panic();
    }

    int_array = cat::value_list<int4, 1, 5>;
    for (int4& i : int_array) {
        Result(i == 1).or_panic();
    }

    int_array = cat::integer_sequence<int4, 5>;
    for (int i = 0; i < int_array.size(); i++) {
        Result(int_array[i] == i).or_panic();
    }

    [[maybe_unused]] cat::Array int_array_2 = cat::value_list<int4, 0, 5>;

    cat::exit();
};
