/*
 * Unit Test: Motor State Tracking
 *
 */

#ifdef GTEST
#include <gtest/gtest.h>

#include <iostream>

#include "custom_components/cat_feeder_logic/binary_state.h"
#include "custom_components/cat_feeder_logic/motor_state_tracker.h"

// Include some data handling test utilities
#include "data_handling/csv_map.hpp"
#include "data_handling/file_paths.hpp"

using namespace esphome::cat_feeder_logic;
using namespace test_utilities;

static float data[15] = {0.0123, 0.0251, 0.0146, 0.0214, 0.0215,   // NOLINT
                         0.1544, 0.2155, 0.1947, 0.1845, 0.2148,   // NOLINT
                         0.0123, 0.0251, 0.0146, 0.0214, 0.0215};  // NOLINT

TEST(TestMotorTracking, NoLag) {
  MotorStateTracker test_tracker;
  EXPECT_FALSE(test_tracker.IsSetup());
  test_tracker.Initialize(1.5F, 1.5F, 0.1F, 100.0F);
  EXPECT_TRUE(test_tracker.IsSetup());
  EXPECT_FALSE(test_tracker.IsReady());
  EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kUnknown);
  EXPECT_EQ(test_tracker.GetCurrentState(), UpDownState::kUnknown);
  EXPECT_EQ(test_tracker.GetVoltageState(), UpDownState::kUnknown);

  // Start ingesting info
  for (int i = 0; i < 15; i++) {
    test_tracker.IngestReadings(-data[i], data[i]);
    if (i < 2) {
      EXPECT_FALSE(test_tracker.IsReady());
    } else {
      EXPECT_TRUE(test_tracker.IsReady());
    }
    if (i >= 5 && i < 10) {
      EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kOn);
      EXPECT_EQ(test_tracker.GetCurrentState(), UpDownState::kUp);
      EXPECT_EQ(test_tracker.GetVoltageState(), UpDownState::kDown);
    } else if (i >= 10) {
      EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kOff);
      EXPECT_EQ(test_tracker.GetCurrentState(), UpDownState::kDown);
      EXPECT_EQ(test_tracker.GetVoltageState(), UpDownState::kUp);
    }
    std::cout << i << "," << static_cast<int>(test_tracker.GetMotorState()) << "," << test_tracker.IsReady()
              << std::endl;
  }
}

TEST(TestMotorTracking, LaggingCurrent) {
  MotorStateTracker test_tracker;
  EXPECT_FALSE(test_tracker.IsSetup());
  test_tracker.Initialize(1.5F, 1.5F, 0.1F, 100.0F);
  EXPECT_TRUE(test_tracker.IsSetup());
  EXPECT_FALSE(test_tracker.IsReady());
  EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kUnknown);
  EXPECT_EQ(test_tracker.GetCurrentState(), UpDownState::kUnknown);
  EXPECT_EQ(test_tracker.GetVoltageState(), UpDownState::kUnknown);

  // Start ingesting info
  for (int i = 0; i < 15; i++) {
    if (i == 0) {
      test_tracker.IngestReadings(-data[i], data[i]);
    } else {
      test_tracker.IngestReadings(-data[i], data[i - 1]);
    }
    if (i < 2) {
      EXPECT_FALSE(test_tracker.IsReady());
    } else {
      EXPECT_TRUE(test_tracker.IsReady());
    }
    if (i == 5) {
      EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kUnknown);
      EXPECT_EQ(test_tracker.GetCurrentState(), UpDownState::kUnknown);
      EXPECT_EQ(test_tracker.GetVoltageState(), UpDownState::kDown);
    }
    if (i > 5 && i < 10) {
      EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kOn);
      EXPECT_EQ(test_tracker.GetCurrentState(), UpDownState::kUp);
      EXPECT_EQ(test_tracker.GetVoltageState(), UpDownState::kDown);
    } else if (i == 10) {
      EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kOn);
      EXPECT_EQ(test_tracker.GetCurrentState(), UpDownState::kUp);
      EXPECT_EQ(test_tracker.GetVoltageState(), UpDownState::kUp);
    } else if (i > 10) {
      EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kOff);
      EXPECT_EQ(test_tracker.GetCurrentState(), UpDownState::kDown);
      EXPECT_EQ(test_tracker.GetVoltageState(), UpDownState::kUp);
    }
    std::cout << i << "," << static_cast<int>(test_tracker.GetMotorState()) << "," << test_tracker.IsReady()
              << std::endl;
  }
}

TEST(TestMotorTracking, PrefilteredMotorData) {
  /// NOTE: This is data that went through a 0.5 Hz low pass filter
  CSVMap csv_data(GetProjectRootPathString() + "/projects/cat_feeder/unit_tests/no_load_filtered_motor_data.csv");
  std::optional<Eigen::VectorXf> voltage_vec = csv_data.GetEigenLibraryVectorFormat("V");
  std::optional<Eigen::VectorXf> amperage_vec = csv_data.GetEigenLibraryVectorFormat("I");

  // make sure we correctly read in the data and have floating point values to iterate through
  EXPECT_TRUE(voltage_vec.has_value());
  EXPECT_TRUE(amperage_vec.has_value());
  EXPECT_EQ(voltage_vec.value().rows(), 467);
  EXPECT_EQ(voltage_vec.value().cols(), 1);

  // Create the motor tracker
  MotorStateTracker test_tracker;
  /** Our actual params:  V/s,  A/s,  d_sec, cutoff_freq_hz */
  test_tracker.Initialize(0.05F, 0.05F, 0.25F, 1000.0F /** actual Hz = 0.5 */);

  // Loop through data
  float time = 0.0F;
  for (unsigned int i = 0; i < voltage_vec.value().rows(); i++) {
    float voltage = voltage_vec.value()(i, 0);
    float amperage = amperage_vec.value()(i, 0);

    // Call the motor tracker
    test_tracker.IngestReadings(voltage, amperage);

    // Expectations (motor on between 6.5 and 34, then off until 76.5 to 105, then off again)
    if (time > 7.0F && time < 34) {
      EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kOn);
    }
    if (time > 35 && time < 76) {
      EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kOff);
    }
    if (time > 77 && time < 104) {
      EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kOn);
    }
    if (time > 106) {
      EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kOff);
    }
    std::cout << time << "," << static_cast<int>(test_tracker.GetMotorState()) << ","
              << static_cast<int>(test_tracker.GetVoltageState()) << ","
              << static_cast<int>(test_tracker.GetCurrentState()) << std::endl;

    time += 0.25F;
  }
}

#endif
