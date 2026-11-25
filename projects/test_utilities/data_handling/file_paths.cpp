/*
 * file_paths.cpp
 *
 */

#include "data_handling/file_paths.hpp"

namespace test_utilities {

std::string GetProjectRootPathString() {
  return std::filesystem::current_path().parent_path().parent_path().parent_path().parent_path().string();
}

}  // namespace test_utilities
