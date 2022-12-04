#include <cat/maybe>
#include <cat/memory>
#include <cat/unique>
#include <cat/utility>

#include "../unit_tests.hpp"

struct Movable {
    Movable() = default;
    Movable(Movable&&) = default;
    auto operator=(Movable&&){};
};

int4 maybe_counter = 0;

struct OptNonTrivial {
    OptNonTrivial() {
        ++maybe_counter;
    }
    OptNonTrivial(OptNonTrivial const&) {
        ++maybe_counter;
    }
    OptNonTrivial(OptNonTrivial&&) {
        ++maybe_counter;
    }
    ~OptNonTrivial() {
        ++maybe_counter;
    }
    OptNonTrivial(int, int, char) {
    }
};

struct OptConstNonTrivial {
    constexpr OptConstNonTrivial() {  // NOLINT
    }
    constexpr OptConstNonTrivial(OptConstNonTrivial const&) {  // NOLINT
    }
    constexpr OptConstNonTrivial(OptConstNonTrivial&&) {
    }
};

auto maybe_try_success() -> cat::Maybe<int> {
    cat::Maybe<int> error{0};
    int boo = TRY(error);
    return boo;
}

auto maybe_try_fail() -> cat::Maybe<int> {
    cat::Maybe<int> error{nullopt};
    int boo = TRY(error);
    return boo;
}

TEST(test_maybe) {
    // Initialize empty.
    cat::Maybe<int4> foo{nullopt};
    cat::verify(!foo.has_value());

    cat::Maybe<int4> inplace_1{};
    cat::verify(!inplace_1.has_value());

    // `int4` default-initializes to 0.
    // cat::Maybe<int4> inplace_2{in_place};
    // cat::verify(inplace_2.value() == 0);

    // Assign a value.
    foo = 1;
    cat::verify(foo.has_value());

    // Remove a value.
    foo = nullopt;
    cat::verify(!foo.has_value());

    // Unwrap a value.
    cat::Maybe<int4> moo = 1;
    moo = 2;
    cat::verify(moo.value() == 2);

    moo = nullopt;
    cat::verify(moo.value_or(100) == 100);

    // `Maybe` reference.
    cat::Maybe<int4&> ref(nullopt);
    cat::Maybe<int4&> ref_2 = nullopt;

    cat::verify(!ref.has_value());
    cat::verify(!ref_2.has_value());

    // Rebind.
    int4 goo = 1;
    cat::Maybe<int4&> boo = goo;
    ref = boo;
    boo = nullopt;

    // Because `boo` was rebinded when assigned `nullopt`, `ref` should still
    // hold a value.
    cat::verify(ref.has_value());

    cat::verify(ref.value() == 1);
    goo = 2;
    cat::verify(ref.value() == 2);

    int4 goo_2 = 3;
    // `ref` is rebinded to `goo_2`, instead of `3` assigning through into
    // `goo`.
    ref = goo_2;
    cat::verify(goo == 2);
    goo = 0;
    cat::verify(ref.value() == 3);

    ref = nullopt;
    cat::verify(!ref.has_value());

    ref_2 = goo;
    cat::verify(ref_2.has_value());
    cat::verify(ref_2.value() == goo);

    // `Maybe` with a predicate.
    cat::Maybe<cat::Compact<int4,
                            [](int4 input) -> bool {
                                return input >= 0;
                            },
                            -1>>
        positive(nullopt);
    cat::verify(!positive.has_value());

    positive = -10;
    cat::verify(!positive.has_value());

    positive = 0;
    cat::verify(positive.has_value());
    _ = positive.or_exit();

    positive = 10;
    cat::verify(positive.has_value());

    positive = nullopt;
    cat::verify(!positive.has_value());

    // `Maybe<void>` with a predicate.
    cat::Maybe<cat::Compact<cat::MonostateStorage<int, 0>,
                            [](int input) -> bool {
                                return input >= 0;
                            },
                            -1>>
        predicate_void(nullopt);
    cat::verify(!predicate_void.has_value());
    predicate_void = monostate;
    cat::verify(predicate_void.has_value());
    _ = predicate_void.or_exit();

    // Test the sentinel predicate.
    cat::Maybe<cat::Sentinel<int4, 0>> nonzero = nullopt;
    cat::verify(!nonzero.has_value());

    nonzero = 1;
    cat::verify(nonzero.has_value());

    nonzero = 0;
    cat::verify(!nonzero.has_value());

    // Test `MaybePtr`.
    int4 get_addr = 0;
    cat::MaybePtr<int4> opt_ptr = &get_addr;
    cat::verify(opt_ptr.has_value());
    cat::verify(opt_ptr.value() == &get_addr);
    cat::verify(*opt_ptr.value() == 0);
    cat::verify(opt_ptr.p_value() == &get_addr);

    opt_ptr = nullopt;
    cat::verify(!opt_ptr.has_value());
    opt_ptr = nullptr;
    cat::verify(!opt_ptr.has_value());

    // Converting assignments. `foo` is `int4`.
    foo = int{1};
    foo = short{2};

    // Monadic methods.
    moo = 2;

    // Type converting transform.
    cat::verify(moo.transform([](int4 input) -> uint8 {
                       return static_cast<uint8>(input * 2);
                   })
                    .value() == 4u);

    moo.or_else([] {
        cat::exit(1);
    });

    moo = nullopt;
    cat::verify(!moo.transform([](int4 input) {
                        return input * 2;
                    })
                     .has_value());

    _ = moo.and_then([](int4 input) -> cat::Maybe<int4> {
        cat::exit(1);
        return input;
    });

    cat::verify(!moo.transform([](int4 input) {
                        return input * 2;
                    })
                     .and_then([](int4 input) -> cat::Maybe<int4> {
                         return input;
                     })
                     .has_value());

    positive = nullopt;
    cat::verify(!positive
                     .transform([](int4 input) -> int4 {
                         return input * 2;
                     })
                     .has_value());

    cat::verify(!positive
                     .transform([](int4 input) {
                         return input * 2;
                     })
                     .and_then([](int4 input) -> cat::Maybe<int4> {
                         return input;
                     })
                     .has_value());

    decltype(positive) default_predicate_1{};
    cat::verify(!default_predicate_1.has_value());

    // Test function calls.
    auto return_int = [](int4 input) -> int4 {
        return input + 1;
    };
    auto return_none = [](int4) -> cat::Maybe<int4> {
        return nullopt;
    };
    auto return_opt = [](int4 input) -> cat::Maybe<int4> {
        return input;
    };
    auto return_void = [](int4) -> void {
    };
    auto return_opt_void = [](int4) -> cat::Maybe<void> {
        return monostate;
    };
    auto nothing = []() -> void {
    };
    auto maybe_nothing = []() -> cat::Maybe<void> {
        return nullopt;
    };

    foo.transform(return_int).and_then(return_opt_void).or_else(nothing);

    _ = foo.transform(return_int)
            .and_then(return_opt_void)
            .or_else(maybe_nothing);

    cat::Maybe<int4> monadic_int;
    monadic_int = return_none(0).and_then(return_opt);
    cat::verify(!monadic_int.has_value());

    monadic_int = return_opt(1).transform(return_int);
    cat::verify(monadic_int.has_value());
    cat::verify(monadic_int.value() == 2);

    cat::Maybe<void> monadic_void =
        return_opt(1).transform(return_int).transform(return_void);
    cat::verify(monadic_void.has_value());

    // Test monadic methods on reference types.
    int4 monadic_int_ref = 1;
    cat::Maybe<void> monadic_void_ref =
        cat::Maybe(monadic_int_ref).and_then(return_opt_void);
    // Be sure that this did not assign through.
    cat::verify(monadic_void_ref.has_value());

    // The default value of `int4` is `0`.
    decltype(positive) default_predicate_2{in_place};
    cat::verify(default_predicate_2.value() == 0);

    // Test monadic methods on move-only types.
    cat::Maybe<cat::Unique<int4>> monadic_move = 1;
    monadic_move = return_none(0).and_then(return_opt);
    cat::verify(!monadic_move.has_value());

    monadic_move = return_opt(1).transform(return_int);
    cat::verify(monadic_move.has_value());
    cat::verify(monadic_move.value().borrow() == 2);

    // Test copying `Maybe`s into other `Maybe`s.
    cat::Maybe<int4> opt_original = 10;
    cat::Maybe<int4> opt_copy_1 = cat::Maybe(opt_original);
    cat::Maybe<int4> opt_copy_2 = opt_original;
    cat::verify(opt_copy_1.value() == 10);
    cat::verify(opt_copy_2.value() == 10);

    // Getting pointers.
    foo = 1;
    int4 const& ref_foo = foo.value();
    cat::verify(&ref_foo == &foo.value());
    cat::verify(foo.p_value() == &foo.value());
    cat::verify(foo.p_value() == addressof(foo.value()));

    // Test non-trivial reference.
    OptNonTrivial nontrivial_val;
    cat::Maybe<OptNonTrivial&> nontrivial_ref_default;
    nontrivial_ref_default = nontrivial_val;
    [[maybe_unused]] cat::Maybe<OptNonTrivial&> nontrivial_ref = nontrivial_val;

    OptNonTrivial const const_nontrivial_val;
    [[maybe_unused]] cat::Maybe<OptNonTrivial&> const
        mut_const_nontrivial_ref_default;
    [[maybe_unused]] cat::Maybe<OptNonTrivial&> const mut_const_nontrivial_ref =
        nontrivial_val;

    [[maybe_unused]] cat::Maybe<OptNonTrivial const&>
        const_mut_nontrivial_ref_default;
    [[maybe_unused]] cat::Maybe<OptNonTrivial const&> const_mut_nontrivial_ref =
        nontrivial_val;
    [[maybe_unused]] cat::Maybe<OptNonTrivial const&>
        const_mut_nontrivial_ref_2 = const_nontrivial_val;
    const_mut_nontrivial_ref = const_nontrivial_val;

    [[maybe_unused]] cat::Maybe<OptNonTrivial const&> const
        const_const_nontrivial_ref_default;
    [[maybe_unused]] cat::Maybe<OptNonTrivial const&> const
        const_const_nontrivial_ref = nontrivial_val;
    [[maybe_unused]] cat::Maybe<OptNonTrivial const&> const
        const_const_nontrivial_ref_2 = const_nontrivial_val;

    // `Maybe const`
    cat::Maybe<int4> const constant_val = 1;
    [[maybe_unused]] cat::Maybe<int4> const constant_null = nullopt;
    [[maybe_unused]] auto con = constant_val.value();

    // Test constant references.
    int4 nonconstant_int = 10;
    int4 const constant_int = 9;
    cat::Maybe<int4 const&> constant_ref = constant_int;
    cat::verify(constant_ref.value() == 9);
    constant_ref = nonconstant_int;
    cat::verify(constant_ref.value() == 10);

    // Test move-only types.
    Movable mov;
    cat::Maybe<Movable> maybe_movs(cat::move(mov));

    // Non-trivial constructor and destructor.
    cat::Maybe<OptNonTrivial> nontrivial = OptNonTrivial();

    // `Maybe<void>` default-initializes empty:
    cat::Maybe<void> optvoid;
    cat::verify(!optvoid.has_value());
    // `monostate` initializes a value:
    cat::Maybe<void> optvoid_2{monostate};
    cat::verify(optvoid_2.has_value());

    // `in_place` initializes a value:
    cat::Maybe<void> optvoid_4{in_place};
    cat::verify(optvoid_4.has_value());
    // `nullopt` initializes empty:
    cat::Maybe<void> optvoid_5{nullopt};
    cat::verify(!optvoid_5.has_value());

    cat::Maybe<OptNonTrivial> in_place_nontrivial_1{in_place};
    _ = in_place_nontrivial_1.verify();

    cat::Maybe<OptNonTrivial> in_place_nontrivial_2{in_place, 1, 2, 'a'};
    _ = in_place_nontrivial_2.verify();

    // Test `Maybe` in a `constexpr` context.
    auto constant = []() constexpr {
        [[maybe_unused]] constexpr cat::Maybe<int> const_int_default;

        constexpr cat::Maybe<OptConstNonTrivial> const_nontrivial_default;
        cat::verify(!const_nontrivial_default.has_value());

        constexpr cat::Maybe<OptConstNonTrivial> const_nontrivial =
            OptConstNonTrivial();
        cat::verify(const_nontrivial.has_value());

        constexpr cat::Maybe<OptConstNonTrivial> const_nontrivial_in_place = {
            in_place, OptConstNonTrivial()};
        cat::verify(const_nontrivial_in_place.has_value());

        // Test `Maybe<Compact<T>>`.
        constexpr cat::MaybePtr<void> const_optptr = nullptr;
        cat::MaybePtr<void> optptr = nullptr;
        optptr = nullptr;
        optptr = const_optptr;
        [[maybe_unused]] cat::MaybePtr<void> optptr2 = optptr;
        [[maybe_unused]] cat::MaybePtr<void> optptr3 = const_optptr;
        [[maybe_unused]] cat::MaybePtr<void> optptr4;

        [[maybe_unused]] constexpr cat::MaybePtr<OptNonTrivial>
            const_nontrivial_optptr = nullptr;
        [[maybe_unused]] constexpr cat::MaybePtr<OptNonTrivial>
            const_nontrivial_default_optptr;
        [[maybe_unused]] cat::MaybePtr<OptNonTrivial> nontrivial_optptr =
            nullptr;
        [[maybe_unused]] cat::MaybePtr<OptNonTrivial> nontrivial_default_optptr;
    };
    _ = cat::constant_evaluate(constant);

    // Assign value:
    optvoid = monostate;
    cat::verify(optvoid.has_value());
    // Remove value:
    optvoid = nullopt;
    cat::verify(!optvoid.has_value());

    // Test `.is()`;
    cat::Maybe<int4> opt_is = 1;
    cat::verify(opt_is.is<int4>());
    cat::verify(!opt_is.is<uint8>());

    opt_is = nullopt;
    cat::verify(!opt_is.is<int4>());
    cat::verify(!opt_is.is<uint8>());

    // Test `match()`.
    cat::Maybe<int4> opt_match = 1;

    bool matched = false;
    cat::match(opt_match)(is_a(1).then([&]() {
        matched = true;
    }));
    cat::match(opt_match)(is_a(2).then([&]() {
        matched = false;
    }));
    cat::verify(matched);

    matched = false;
    cat::match(opt_match)(is_a<int4>().then([&]() {
        matched = true;
    }));
    cat::match(opt_match)(is_a<uint8>().then([&]() {
        matched = false;
    }));
    cat::verify(matched);

    // Test matching against `nullopt` when this holds a value.
    matched = true;
    cat::match(opt_match)(is_a(nullopt).then([&]() {
        matched = false;
    }));
    cat::verify(matched);

    // Test matching against `nullopt` when this does not hold a value.
    matched = false;
    opt_match = nullopt;
    cat::match(opt_match)(is_a(nullopt).then([&]() {
        matched = true;
    }));
    cat::verify(matched);

    // Test member access pattern matching syntax.
    matched = false;
    opt_match.match(is_a(nullopt).then([&]() {
        matched = true;
    }));
    cat::verify(matched);

    // Test traits.
    static_assert(cat::is_maybe<cat::Maybe<int>>);
    static_assert(cat::is_maybe<decltype(opt_original)>);

    static_assert(!cat::is_scaredy<cat::Maybe<int>>);
    static_assert(!cat::is_scaredy<decltype(opt_original)>);

    // Test `TRY` macro.
    _ = maybe_try_success().verify();
    cat::Maybe fail = maybe_try_fail();
    cat::verify(!fail.has_value());
}
