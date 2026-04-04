#ifndef __LOADER_FALLBACK_CHAIN_HPP__
#define __LOADER_FALLBACK_CHAIN_HPP__
#pragma once

/// @file fallback_chain.hpp
/// @brief Composable multi-layer fallback chain for loader operations.
///
/// FallbackChain<Strategies...> tries each strategy in order.  The first
/// strategy to succeed short-circuits.  Failures are logged to the
/// DiagnosticCollector and the next strategy is attempted.
///
/// Each strategy is a stateless struct with:
///   static constexpr const char* name;
///   using result_type = tl::expected<T, DiagnosticCode>;
///   static result_type try_execute(DiagCollector& diag, Args... args);
///
/// Design:
///   - Independently testable: each strategy can be tested in isolation.
///   - Composable: chains can be defined as type aliases and reused.
///   - Type-safe: all strategies must share the same result_type.
///   - Zero runtime overhead: the chain is a compile-time construct,
///     each try_execute is a direct call (no virtual dispatch).
///
/// Example:
///   using DepInjector = FallbackChain<StealDtNull, GrowDynamic>;
///   auto result = DepInjector::execute(diag, elf, lib_name);

#include <diagnostic_collector.hpp>
#include <diagnostic.hpp>

#include <tl/expected.hpp>

#include <cstddef>
#include <string>
#include <utility>

namespace VMPilot::Loader {

namespace detail {

/// Recursive implementation: try Strategy S, then Rest...
template<std::size_t I, typename S, typename... Rest>
struct FallbackImpl {
    template<typename... Args>
    static auto execute(Common::DiagnosticCollector& diag, Args&&... args)
        -> typename S::result_type
    {
        auto result = S::try_execute(diag, std::forward<Args>(args)...);
        if (result.has_value()) {
            return result;
        }

        // Log failure and try next strategy
        diag.note("FallbackChain",
                  result.error(),
                  std::string("Layer ") + std::to_string(I + 1)
                      + " (" + S::name + ") failed, trying next");

        if constexpr (sizeof...(Rest) > 0) {
            return FallbackImpl<I + 1, Rest...>::execute(
                diag, std::forward<Args>(args)...);
        } else {
            // Last strategy's error propagates
            return result;
        }
    }
};

}  // namespace detail

/// Composable multi-layer fallback chain.
///
/// Tries each Strategy in order.  First success wins.
/// All failures are accumulated in the DiagnosticCollector.
///
/// @tparam Strategies...  Ordered list of strategy types to try
template<typename... Strategies>
struct FallbackChain {
    static_assert(sizeof...(Strategies) >= 1,
                  "FallbackChain requires at least one strategy");

    /// Execute the chain.  Returns the result of the first successful
    /// strategy, or the error from the last strategy if all fail.
    template<typename... Args>
    static auto execute(Common::DiagnosticCollector& diag, Args&&... args)
        -> typename detail::FallbackImpl<0, Strategies...>::template
           execute<Args...> // return type deduced from first strategy
    {
        return detail::FallbackImpl<0, Strategies...>::execute(
            diag, std::forward<Args>(args)...);
    }
};

// Simpler alias: just call the impl directly (avoids return-type deduction issue)
template<typename... Strategies>
struct Chain {
    static_assert(sizeof...(Strategies) >= 1,
                  "Chain requires at least one strategy");

    template<typename... Args>
    static auto execute(Common::DiagnosticCollector& diag, Args&&... args) {
        return detail::FallbackImpl<0, Strategies...>::execute(
            diag, std::forward<Args>(args)...);
    }
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_FALLBACK_CHAIN_HPP__
