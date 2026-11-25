/*
 * file_paths.hpp
 *
 */

#pragma once

#include <filesystem>  // NOLINT
#include <string>

namespace test_utilities {

/// \return string name of path to embedded_projects root directory (i.e., repo directory)
std::string GetProjectRootPathString();

}  // namespace test_utilities
