/*
 * csv_map.hpp
 *
 */

#pragma once

#include <cctype>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "Eigen/Dense"
#include "csvstream.hpp"

namespace test_utilities {

/// Class to read in a CSV and store its info into a map
/**
    This class will attempt to read a CSV file and put
    its contents into a map object. Keys in the map will
    be the column names and the values will be vector
    objects of strings. The vectors will be ordered by
    row numbers (starting from row 0 being the first
    row of data).
*/
class CSVMap {
 public:
  /// Default constructor (create empty map to read later)
  CSVMap();

  /// Explicit constructor for map
  /**
      The constructor for the map requires the user provide
      the file to read in.

      \note file name MUST include path and file extention.
      E.g., "path/to/file.csv"

      \param file_path_and_name string name of the file to read
  */
  explicit CSVMap(const std::string& file_path_and_name);

  /// Overator overload for ==
  /**
      This function will take 2 instances of CSVMap and determine
      if their csv_contents_ are equivalent to each other

      \param lhs The left-hand-side CSVMap object
      \param rhs The right-hand-side CSVMap object

      \return true if the contents of each map are the same
  */
  friend bool operator==(const CSVMap& lhs, const CSVMap& rhs) {
    // Use built in == operator for map objects
    // we only care if the contents are equal, no other properties
    return (lhs.csv_contents_ == rhs.csv_contents_);
  }

  /// Overator overload for !=
  /**
      This function will take 2 instances of CSVMap and determine
      if their csv_contents_ are NOT equivalent to each other

      \param lhs The left-hand-side CSVMap object
      \param rhs The right-hand-side CSVMap object

      \return true if the contents of each map are NOT the same
  */
  friend bool operator!=(const CSVMap& lhs, const CSVMap& rhs) { return !(lhs == rhs); }

  /// \return read-only reference to the map contents
  const std::map<std::string, std::vector<std::string> >& ReadMap() const;

  /// Function to return a vector of floats from the given column name
  /**
      This function will take a given column in the CSV map and attempt
      to convert the vector of strings to a vector of floats for numeric
      comparisons later.

      \note We are taking advantage of the `Eigen::VectorXf` objects here
      because the intent of this function is to be used in calculating
      norms between vectors.

      \param column_name The name of the column you want converted into a vector of floats

      \return optional vector of floats (if cannot be converted, return is empty)
  */
  std::optional<Eigen::VectorXf> GetEigenLibraryVectorFormat(const std::string& column_name);

  /// Function to exactly compare 2 columns of this and another map
  /**
      Function will take in a column name and another CSV Map and
      check to see if those columns exist in each and if they have
      the exact same data.

      \param column_name The name of the columns in each map to compare
      \param other Constant reference to another CSVMap object

      \return true if the columns are the same
  */
  bool IsThisColumnEqualTo(const std::string& column_name, const CSVMap& other) const;

  /// Function to return number of rows for which a column is different between 2 objects
  /**
      \param column_name The name of the columns in each map to compare
      \param other Constant reference to another CSVMap object

      \return number of differences between the 2 columns
  */
  unsigned int NumberOfDifferencesInColumn(const std::string& column_name, const CSVMap& other) const;

  /// \return the number of rows of data
  unsigned int NumberOfRows() const;

  /// \return vector of string names of the columns
  std::vector<std::string> ListOfColumnNames() const;

  /// \return total number of data points
  unsigned int TotalNumberOfDataPoints() const;

  /// Function to return total number of errors between this object and another
  /**
      \param other Constant reference to another CSVMap object

      \return number of differences between the 2 objects in all columns
  */
  unsigned int TotalNumberOfDifferences(const CSVMap& other) const;

  /// Function to read given file into map
  /**
      \note file name MUST include path and file extention.
      E.g., "path/to/file.csv"

      \param file_path_and_name string name of the file to read
  */
  void ReadFile(const std::string& file_path_and_name);

 private:
  /// Map of CSV contents
  /**
      The map is organized as:
        key = column name
        value = vector of strings
          value[i] = string result at ith row
  */
  std::map<std::string, std::vector<std::string> > csv_contents_;

  /// Private helper function to determine if string is numeric
  /**
      This function uses regular expressions to determine if a
      given string is numeric.

      \param token

      \return true if string is numeric
  */
  static bool IsNumeric(const std::string& token);
};

}  // namespace test_utilities
