#ifndef __LOADER_ELF_PATCHER_HPP__
#define __LOADER_ELF_PATCHER_HPP__
#pragma once

#include <ELFEditor.hpp>
#include <FormatPatcher.hpp>

namespace VMPilot::Loader {
using ELFPatcher = FormatPatcher<ELFEditor>;
}

#endif  // __LOADER_ELF_PATCHER_HPP__
