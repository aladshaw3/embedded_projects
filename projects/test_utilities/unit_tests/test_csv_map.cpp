/*
 * Unit Test: CSV Map utility
 *
 */

#ifdef GTEST
#include <gtest/gtest.h>

#include <iostream>

#include "data_handling/csv_map.hpp"
#include "data_handling/file_paths.hpp"

using namespace test_utilities;

TEST(TestCSVMap, ReadAndCheckCSV) {
  CSVMap csv_data(GetProjectRootPathString() + "/projects/test_utilities/unit_tests/test_data/example.csv");

  EXPECT_EQ(csv_data.ReadMap().at("name")[0], "Fergie");
  EXPECT_EQ(csv_data.ReadMap().at("name")[1], "Myrtle II");
  EXPECT_EQ(csv_data.ReadMap().at("name")[2], "Oscar");

  EXPECT_EQ(csv_data.ReadMap().at("animal")[0], "horse");
  EXPECT_EQ(csv_data.ReadMap().at("animal")[1], "chicken");
  EXPECT_EQ(csv_data.ReadMap().at("animal")[2], "cat");

  EXPECT_EQ(csv_data.ReadMap().at("age")[0], "5");
  EXPECT_EQ(csv_data.ReadMap().at("age")[1], "10");
  EXPECT_EQ(csv_data.ReadMap().at("age")[2], "12");

  EXPECT_EQ(csv_data.TotalNumberOfDataPoints(), 9U);
}

TEST(TestCSVMap, ExactCompareCSVs) {
  CSVMap csv_data1(GetProjectRootPathString() + "/projects/test_utilities/unit_tests/test_data/example.csv");
  CSVMap csv_data2(GetProjectRootPathString() + "/projects/test_utilities/unit_tests/test_data/example.csv");
  CSVMap csv_data3(GetProjectRootPathString() +
                   "/projects/test_utilities/unit_tests/test_data/example_missing_column.csv");
  CSVMap csv_data4(GetProjectRootPathString() + "/projects/test_utilities/unit_tests/test_data/example_off_by_one.csv");
  EXPECT_TRUE(csv_data1 == csv_data2);
  EXPECT_TRUE(csv_data1 != csv_data3);
  EXPECT_TRUE(csv_data2 != csv_data4);

  EXPECT_TRUE(csv_data1.IsThisColumnEqualTo("name", csv_data4));
  EXPECT_FALSE(csv_data1.IsThisColumnEqualTo("age", csv_data4));
  EXPECT_EQ(csv_data1.NumberOfDifferencesInColumn("age", csv_data4), 1U);
  EXPECT_EQ(csv_data1.TotalNumberOfDifferences(csv_data4), 1U);
  EXPECT_EQ(csv_data1.NumberOfRows(), 3U);
  EXPECT_EQ(csv_data1.ListOfColumnNames().size(), 3U);
  EXPECT_EQ(csv_data1.ListOfColumnNames()[0], "age");
}

TEST(TestCSVMap, NumericVectors) {
  CSVMap csv_data1(GetProjectRootPathString() + "/projects/test_utilities/unit_tests/test_data/example.csv");
  CSVMap csv_data2(GetProjectRootPathString() + "/projects/test_utilities/unit_tests/test_data/example.csv");

  auto res = csv_data1.GetEigenLibraryVectorFormat("age");
  EXPECT_TRUE(res.has_value());  // Make sure we get a result because this column is numeric

  auto bad_res = csv_data1.GetEigenLibraryVectorFormat("name");
  EXPECT_FALSE(bad_res.has_value());  // Should not have a value saved

  // Show the equivalence of the 2 numeric vectors
  Eigen::Vector3f expected_vector(5, 10, 12);
  EXPECT_FLOAT_EQ((res.value() - expected_vector).norm(), 0);

  CSVMap numbers(GetProjectRootPathString() + "/projects/test_utilities/unit_tests/test_data/example_numeric.csv");

  auto int_res = numbers.GetEigenLibraryVectorFormat("int");
  auto uint_res = numbers.GetEigenLibraryVectorFormat("uint");
  auto float_res = numbers.GetEigenLibraryVectorFormat("float");
  auto invalid_res = numbers.GetEigenLibraryVectorFormat("invalid");
  EXPECT_TRUE(int_res.has_value());
  EXPECT_TRUE(uint_res.has_value());
  EXPECT_TRUE(float_res.has_value());
  EXPECT_FALSE(invalid_res.has_value());

  Eigen::Vector3f exp_uint(1, 0, 5);
  Eigen::Vector3f exp_int(-1, 10, -255);
  Eigen::Vector3f exp_float(5.23, -2.3, -1e-7);

  EXPECT_FLOAT_EQ((int_res.value() - exp_int).norm(), 0);
  EXPECT_FLOAT_EQ((uint_res.value() - exp_uint).norm(), 0);
  EXPECT_FLOAT_EQ((float_res.value() - exp_float).norm(), 0);
}

#endif
