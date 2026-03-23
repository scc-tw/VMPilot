#ifndef __LOADER_PE_PATCHER_HPP__
#define __LOADER_PE_PATCHER_HPP__
#pragma once

#include <FormatPatcher.hpp>
#include <PEPatcherTraits.hpp>

namespace VMPilot::Loader {

using PEPatcher = FormatPatcher<PEPatcherTraits>;

}  // namespace VMPilot::Loader

#endif  // __LOADER_PE_PATCHER_HPP__
