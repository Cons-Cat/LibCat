// -*- mode: c++ -*-
// vim: set ft=cpp:
#pragma once

#include <any.h>
#include <concepts.h>
#include <type_traits.h>
#include <utility.h>

/* This file contains a general-purpose error-handling tool. Result<> is a
 * templated struct that either contains some data (unless it holds void) or an
 * error-code. The error-code is an 8-byte value that can be either an integer
 * or a pointer to a string that may be printed. To extract a value out of a
 * result, call one of its member functions.
 *
 * An error code or string pointer must be wrapped in a Failure, such as:
 ** return Failure(1);
 *
 * Otherwise, it is presumed that a Result holds a value rather than an error.
 * This is done to prevent monomorphizing the Result on its error types like
 * Rust, which provides type-safety at the cost of pessimized compiler-times and
 * runtime overhead. This Result is a zero-overhead abstraction over C-style
 * error handling.
 *
 * A Failure is implicitly convertible into a Result, and a T is implicitly
 * convertible into a Result<T>.
 *
 * Result types are marked [[nodiscard]]. With confidence that an invocation of
 * a function returning a Result will never fail, you may call .unsafe_value(),
 * which will skip error-handling when compiler optimizations are enabled. */

template <typename T = void>
struct Result;

struct [[nodiscard]] Failure {
    isize code;

    // TODO: Rethink Error and Result constructors.
    Failure() = default;

    // This must be explicit to disambiguate returning an integer to a Result.
    explicit Failure(isize in_code) {
        code = static_cast<isize>(in_code);
    }

    template <typename T>
    operator Result<T>() const {
        return Result<T>(code);
    }

    template <typename T>
    constexpr operator T() const
        // TODO: Make is_bool<> traits.
        requires(meta::is_same_v<T, isize>() || meta::is_same_v<T, bool>) {
        return code;
    }

    auto operator==(Failure operand) const -> bool {
        return this->code == operand.code;
    }
};

namespace std::detail {

struct ok {};

}  // namespace std::detail

// `Okay` may be returned in a function that returns Result<void>.
constexpr std::detail::ok okay{};

template <typename T>
struct [[nodiscard("To skip error-handling, call .discard_result()")]] Result {
    Failure error_code;  // Error is a 8-byte value.
    /* char should be a relatively unintrusive dummy data for when this holds
     * void. Reflection TS in future C++ will provide conditional-members, which
     * would be a better solution. */
    using ValueType = meta::conditional_t<meta::is_void_v<T>, char, T>;
    static constexpr bool is_void = meta::is_same_v<T, void>;
    /* It is unsafe to access value. You may prefer or_panic_debug() which
     * provides failure-handling in debug builds, and skips them in optimized
     * builds. */
    ValueType const value;
    bool const is_okay;

    Result(Failure in_code) : error_code(in_code), value(), is_okay(false) {
    }
    Result(ValueType in_value) requires(!is_void)
        : error_code(), value(in_value), is_okay(true) {
    }
    // Special-case consuming data when holding `void`. This includes `okay`.
    Result(auto) requires(is_void) : error_code(), value(), is_okay(true) {
    }
    // TODO: Concept for bool1, bool2, and bool4 as well.
    Result(bool&& expression) requires(is_void) : value(), is_okay(expression) {
    }
    /* Attempt to cast an inputted Result to this. This is useful for
     * `Result<Any>`. */
    template <typename U>
    Result(Result<U> && in_result) requires(!is_void)
        : value(meta::bit_cast<T>(in_result.value)),
          is_okay(in_result.is_okay) {
    }

    // Return a specified value when is_okay is false.
    auto or_its(ValueType const& in_value)->ValueType {
        if (!is_okay) {
            return in_value;
        }
        return this->value;
    }
    // nullptr must be special-cased.
    auto or_it_is(decltype(nullptr) in_value)
        ->ValueType /* requires(meta::is_pointer_v<ValueType>) */ {
        if (!is_okay) {
            return static_cast<ValueType>(in_value);
        }
        return this->value;
    }

    // TODO: Add an invokable concept
    auto or_do(auto callback)->T {
        if (!is_okay) {
            return callback();
        }
        if constexpr (!is_void) {
            return this->value;
        }
    }

    /* If this object does not hold a value, the return of this function is
     * undefined. When building -O0, this function panics if a value is not
     * held. When optimizations are enabled, that safety check is elided. */
    auto unsafe_value()->T {
#ifndef __OPTIMIZE__
        if (is_okay) {
            return value;
        }
        // TODO: Error message.
        exit(1);
#endif
        return value;
    }

    void discard_result(){};

    // TODO: Pass in the exit code and error message with overloads.
    auto or_panic()->T {
        if (is_okay) {
            if constexpr (!is_void) {
                return this->value;
            } else if constexpr (is_void) {
                return;
            }
        }
        exit(1);
    }

    auto or_panic(char8_t const* /*error_message*/)->T {
        if (is_okay) {
            if constexpr (!is_void) {
                return this->data;
            } else if constexpr (is_void) {
                return;
            }
        }
        // TODO: print(error_message);
        exit(1);
    }

    /* Because the error code is 8-bytes, it could contain a non-null pointer to
     * an error string. This prints that error message. */
    auto or_print_panic()->T {
        if (is_okay) {
            if constexpr (!is_void) {
                return this->value;
            } else if constexpr (is_void) {
                return;
            }
        }
        // TODO: print(*error_code);
        exit(1);
    }

    /* It may be desirable to skip the costs of printing error messages in
     * release builds of an application. These following functions only print an
     * error message when building -O0. */
    auto or_panic_debug(char8_t const* error_message = u8"")->T {
        if (is_okay) {
            if constexpr (!is_void) {
                return this->data;
            } else if constexpr (is_void) {
                return;
            }
        }
#ifndef __OPTIMIZE__
        // TODO: print(error_message);
#endif
        exit(1);
    }

    /* Because the error code is 8-bytes, it could contain a non-null pointer to
     * an error string. This prints that error message. */
    auto or_print_panic_debug()->T {
        if (is_okay) {
            if constexpr (!is_void) {
                return this->value;
            } else if constexpr (is_void) {
                return;
            }
        }
#ifndef __OPTIMIZE__
        // TODO: print(error_code);
#endif
        exit(1);
    }
};
