#ifndef __LOADER_PE_PATCHER_HPP__
#define __LOADER_PE_PATCHER_HPP__
#pragma once

#include <FormatPatcher.hpp>
#include <PEEditor.hpp>

namespace VMPilot::Loader {
using PEPatcher = FormatPatcher<PEEditor>;
}

#endif  // __LOADER_PE_PATCHER_HPP__
