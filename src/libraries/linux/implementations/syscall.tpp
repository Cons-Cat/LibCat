// -*- mode: c++ -*-
// vim: set ft=cpp:
#pragma once

#include <linux>

template <typename T, typename... Args>
auto nix::syscall(ssize const call, Args const... parameters)
    -> nix::ScaredyLinux<T>
requires(sizeof...(Args) < 7) {
    static constexpr unsigned length = sizeof...(Args);
    cat::Any arguments[length] = {parameters...};

	// TODO: Make this a `union` of reasonable types.
    ssize result;

    if constexpr (length == 0) {
        result = nix::syscall0(call);
    } else if constexpr (length == 1) {
        result = nix::syscall1(call, arguments[0]);
    } else if constexpr (length == 2) {
        result = nix::syscall2(call, arguments[0], arguments[1]);
    } else if constexpr (length == 3) {
        result = nix::syscall3(call, arguments[0], arguments[1], arguments[2]);
    } else if constexpr (length == 4) {
        result = nix::syscall4(call, arguments[0], arguments[1], arguments[2],
                               arguments[3]);
    } else if constexpr (length == 5) {
        result = nix::syscall5(call, arguments[0], arguments[1], arguments[2],
                               arguments[3], arguments[4]);
    } else if constexpr (length == 6) {
        result = nix::syscall6(call, arguments[0], arguments[1], arguments[2],
                               arguments[3], arguments[4], arguments[5]);
    }

    if (result < 0) {
        return static_cast<LinuxError>(result.c());
    }
	
    if constexpr (!::meta::is_void<T>) {
        return meta::bit_cast<T>(result);
    } else {
        return monostate;
    }
}
