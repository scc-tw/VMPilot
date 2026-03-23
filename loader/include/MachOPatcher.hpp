#ifndef __LOADER_MACHO_PATCHER_HPP__
#define __LOADER_MACHO_PATCHER_HPP__
#pragma once

#include <FormatPatcher.hpp>
#include <MachOEditor.hpp>

namespace VMPilot::Loader {
using MachOPatcher = FormatPatcher<MachOEditor>;
}

#endif  // __LOADER_MACHO_PATCHER_HPP__
