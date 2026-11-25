/*
 * csv_map.cpp
 *
 */

#include "data_handling/csv_map.hpp"

namespace test_utilities {

CSVMap::CSVMap() { this->csv_contents_.clear(); }

CSVMap::CSVMap(const std::string& file_path_and_name) { this->ReadFile(file_path_and_name); }

const std::map<std::string, std::vector<std::string> >& CSVMap::ReadMap() const { return this->csv_contents_; }

// NOLINTBEGIN
bool CSVMap::IsNumeric(const std::string& token) {
  // Helper flags
  bool have_dot = false;
  bool have_e = false;
  bool have_plus_minus = false;
  bool is_numeric = true;

  // Loop through the string to determine whether or not this is a number
  /**
      This is an extremely ugly function I borrowed from my own thesis
      written almost 10 years ago. It seems to work fine for this case
      since we cannot use std::exceptions
  */
  for (unsigned int i = 0; i < token.length(); i++) {
    if (!std::isdigit(token[i])) {
      if (!(token[i] == '-' || token[i] == '+') && i == 0) {
        is_numeric = false;
        break;
      } else if (token[i] == '.') {
        if (have_dot == true) {
          is_numeric = false;
          break;
        } else {
          have_dot = true;
        }
      } else if ((token[i] == 'E' || token[i] == 'e') && i > 0 && token[i - 1] != ' ') {
        if (have_e == true) {
          is_numeric = false;
          break;
        } else {
          have_e = true;
          if (i == token.length() - 1) {
            is_numeric = false;
            break;
          } else {
            i++;
            if (!std::isdigit(token[i])) {
              if (token[i] == '-' || token[i] == '+') {
                if (have_plus_minus == true) {
                  is_numeric = false;
                  break;
                } else {
                  have_plus_minus = true;
                }
              } else {
                is_numeric = false;
                break;
              }
            } else {
              // continue
            }
          }
        }
      } else if (i != 0 && ((token[i] == '-' || token[i] == '+') || !std::isdigit(token[i]))) {
        is_numeric = false;
        break;
      } else {
        // continue
      }
    } else {
      // continue
    }
  }
  return is_numeric;
}
// NOLINTEND

std::optional<Eigen::VectorXf> CSVMap::GetEigenLibraryVectorFormat(const std::string& column_name) {
  if (!this->csv_contents_.contains(column_name)) {
    return {};
  }

  Eigen::VectorXf tmp;
  tmp.resize(this->csv_contents_.at(column_name).size(), 1);
  for (unsigned int i = 0; i < this->csv_contents_.at(column_name).size(); i++) {
    // If the conversion cannot be made, provide error message and return nothing
    if (CSVMap::IsNumeric(this->csv_contents_.at(column_name)[i])) {
      tmp[i] = std::stof(this->csv_contents_.at(column_name)[i]);
    } else {
      return {};
    }
  }

  return tmp;
}

bool CSVMap::IsThisColumnEqualTo(const std::string& column_name, const CSVMap& other) const {
  if (!this->csv_contents_.contains(column_name) || !other.ReadMap().contains(column_name)) {
    return false;
  }
  if (this->csv_contents_.at(column_name).size() != other.ReadMap().at(column_name).size()) {
    return false;
  }
  for (unsigned int i = 0; i < this->csv_contents_.at(column_name).size(); i++) {
    if (this->csv_contents_.at(column_name)[i] != other.ReadMap().at(column_name)[i]) {
      return false;
    }
  }
  return true;
}

unsigned int CSVMap::NumberOfDifferencesInColumn(const std::string& column_name, const CSVMap& other) const {
  unsigned int num = 0;
  if (!this->csv_contents_.contains(column_name) || !other.ReadMap().contains(column_name)) {
    return this->csv_contents_.begin()->second.size();
  }
  if (this->csv_contents_.at(column_name).size() != other.ReadMap().at(column_name).size()) {
    return this->csv_contents_.at(column_name).size();
  }
  for (unsigned int i = 0; i < this->csv_contents_.at(column_name).size(); i++) {
    if (this->csv_contents_.at(column_name)[i] != other.ReadMap().at(column_name)[i]) {
      num++;
    }
  }
  return num;
}

unsigned int CSVMap::NumberOfRows() const { return this->csv_contents_.begin()->second.size(); }

std::vector<std::string> CSVMap::ListOfColumnNames() const {
  std::vector<std::string> tmp;
  for (auto it = this->csv_contents_.begin(); it != this->csv_contents_.end(); ++it) {
    tmp.push_back(it->first);
  }
  return tmp;
}

unsigned int CSVMap::TotalNumberOfDataPoints() const { return this->NumberOfRows() * this->csv_contents_.size(); }

unsigned int CSVMap::TotalNumberOfDifferences(const CSVMap& other) const {
  unsigned int num = 0;
  for (auto it = this->csv_contents_.begin(); it != this->csv_contents_.end(); ++it) {
    num += this->NumberOfDifferencesInColumn(it->first, other);
  }
  return num;
}

void CSVMap::ReadFile(const std::string& file_path_and_name) {
  csvstream csvin(file_path_and_name);
  std::map<std::string, std::string> row;
  this->csv_contents_.clear();
  while (csvin >> row) {
    for (auto& col : row) {
      this->csv_contents_[col.first].push_back(col.second);
    }
  }
}

}  // namespace test_utilities
