#ifndef __SDK_NATIVE_FUNCTION_BASE_HPP__
#define __SDK_NATIVE_FUNCTION_BASE_HPP__
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace VMPilot::SDK::Segmentator {
/**
 * @brief Base class for native functions in different executable formats.
 *
 * This class is intended to be inherited by derived classes for different
 * executable formats such as ELF, PE, etc.
 */
class NativeFunctionBase {
   protected:
    uint64_t m_addr;
    uint64_t m_size;
    std::string m_name;
    std::vector<uint8_t> m_code;
    std::vector<uint8_t> globalData;

    /// The function symbol that physically contains this region.
    ///
    /// When the compiler inlines a function with VMPilot_Begin/End, the
    /// protected region is duplicated into each call site's enclosing
    /// function.  This field records which symbol the region lives in,
    /// enabling downstream grouping to distinguish the "canonical" copy
    /// (enclosing == source) from inlined copies.
    ///
    /// nullopt when: PE format (no function symbols), stripped binary,
    /// or region falls outside any known symbol range.
    std::optional<std::string> m_enclosing_symbol;

    friend class ArchHandlerStrategy;

   public:
    NativeFunctionBase(uint64_t addr, uint64_t size, std::string name,
                       std::vector<uint8_t> code)
        : m_addr(addr), m_size(size), m_name(std::move(name)),
          m_code(std::move(code)) {}

    NativeFunctionBase(const NativeFunctionBase&) = default;
    NativeFunctionBase(NativeFunctionBase&&) noexcept = default;
    NativeFunctionBase& operator=(const NativeFunctionBase&) = default;
    NativeFunctionBase& operator=(NativeFunctionBase&&) noexcept = default;

    virtual ~NativeFunctionBase() = default;
    uint64_t getAddr() const { return m_addr; }
    uint64_t getSize() const { return m_size; }
    std::string getName() const { return m_name; }
    std::vector<uint8_t> getCode() const { return m_code; }
    std::vector<uint8_t> getGlobalData() const { return globalData; }

    void setEnclosingSymbol(std::string symbol) {
        m_enclosing_symbol = std::move(symbol);
    }
    const std::optional<std::string>& getEnclosingSymbol() const {
        return m_enclosing_symbol;
    }

    /// Check that code.size() == m_size (the fundamental invariant).
    bool isValid() const { return m_code.size() == m_size; }
};
}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_NATIVE_FUNCTION_BASE_HPP__
