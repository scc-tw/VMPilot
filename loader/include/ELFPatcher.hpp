#ifndef __LOADER_ELF_PATCHER_HPP__
#define __LOADER_ELF_PATCHER_HPP__
#pragma once

#include <ELFPatcherTraits.hpp>
#include <FormatPatcher.hpp>

namespace VMPilot::Loader {

/// ELF binary patcher — FormatPatcher instantiated with ELF traits.
using ELFPatcher = FormatPatcher<ELFPatcherTraits>;

}  // namespace VMPilot::Loader

#endif  // __LOADER_ELF_PATCHER_HPP__
