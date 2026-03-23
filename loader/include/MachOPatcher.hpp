#ifndef __LOADER_MACHO_PATCHER_HPP__
#define __LOADER_MACHO_PATCHER_HPP__
#pragma once

#include <FormatPatcher.hpp>
#include <MachOPatcherTraits.hpp>

namespace VMPilot::Loader {

using MachOPatcher = FormatPatcher<MachOPatcherTraits>;

}  // namespace VMPilot::Loader

#endif  // __LOADER_MACHO_PATCHER_HPP__
