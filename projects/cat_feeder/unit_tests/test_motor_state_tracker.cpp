/*
 * Unit Test: Motor State Tracking
 *
 */

#ifdef GTEST
#include <gtest/gtest.h>

#include <iostream>

#include "custom_components/cat_feeder_logic/binary_state.h"
#include "custom_components/cat_feeder_logic/motor_state_tracker.h"

using namespace esphome::cat_feeder_logic;

static float data[15] = {0.0123, 0.0251, 0.0146, 0.0214, 0.0215,   // NOLINT
                         0.1544, 0.2155, 0.1947, 0.1845, 0.2148,   // NOLINT
                         0.0123, 0.0251, 0.0146, 0.0214, 0.0215};  // NOLINT

TEST(TestMotorTracking, NoLag) {
  MotorStateTracker test_tracker;
  EXPECT_FALSE(test_tracker.IsSetup());
  test_tracker.SetThresholds(1.5F, 1.5F, 0.1F);
  EXPECT_TRUE(test_tracker.IsSetup());
  EXPECT_FALSE(test_tracker.IsReady());
  EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kUnknown);
  EXPECT_EQ(test_tracker.GetCurrentState(), UpDownState::kUnknown);
  EXPECT_EQ(test_tracker.GetVoltageState(), UpDownState::kUnknown);

  // Start ingesting info
  for (int i = 0; i < 15; i++) {
    test_tracker.IngestReadings(data[i], data[i]);
    if (i < 2) {
      EXPECT_FALSE(test_tracker.IsReady());
    } else {
      EXPECT_TRUE(test_tracker.IsReady());
    }
    if (i >= 5 && i < 10) {
      EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kOn);
      EXPECT_EQ(test_tracker.GetCurrentState(), UpDownState::kUp);
      EXPECT_EQ(test_tracker.GetVoltageState(), UpDownState::kUp);
    } else if (i >= 10) {
      EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kOff);
      EXPECT_EQ(test_tracker.GetCurrentState(), UpDownState::kDown);
      EXPECT_EQ(test_tracker.GetVoltageState(), UpDownState::kDown);
    }
    std::cout << i << "," << static_cast<int>(test_tracker.GetMotorState()) << "," << test_tracker.IsReady()
              << std::endl;
  }
}

TEST(TestMotorTracking, NoLaggingCurrent) {
  MotorStateTracker test_tracker;
  EXPECT_FALSE(test_tracker.IsSetup());
  test_tracker.SetThresholds(1.5F, 1.5F, 0.1F);
  EXPECT_TRUE(test_tracker.IsSetup());
  EXPECT_FALSE(test_tracker.IsReady());
  EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kUnknown);
  EXPECT_EQ(test_tracker.GetCurrentState(), UpDownState::kUnknown);
  EXPECT_EQ(test_tracker.GetVoltageState(), UpDownState::kUnknown);

  // Start ingesting info
  for (int i = 0; i < 15; i++) {
    if (i == 0) {
      test_tracker.IngestReadings(data[i], data[i]);
    } else {
      test_tracker.IngestReadings(data[i], data[i - 1]);
    }
    if (i < 2) {
      EXPECT_FALSE(test_tracker.IsReady());
    } else {
      EXPECT_TRUE(test_tracker.IsReady());
    }
    if (i == 5) {
      EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kUnknown);
      EXPECT_EQ(test_tracker.GetCurrentState(), UpDownState::kUnknown);
      EXPECT_EQ(test_tracker.GetVoltageState(), UpDownState::kUp);
    }
    if (i > 5 && i < 10) {
      EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kOn);
      EXPECT_EQ(test_tracker.GetCurrentState(), UpDownState::kUp);
      EXPECT_EQ(test_tracker.GetVoltageState(), UpDownState::kUp);
    } else if (i == 10) {
      EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kOn);
      EXPECT_EQ(test_tracker.GetCurrentState(), UpDownState::kUp);
      EXPECT_EQ(test_tracker.GetVoltageState(), UpDownState::kDown);
    } else if (i > 10) {
      EXPECT_EQ(test_tracker.GetMotorState(), OnOffState::kOff);
      EXPECT_EQ(test_tracker.GetCurrentState(), UpDownState::kDown);
      EXPECT_EQ(test_tracker.GetVoltageState(), UpDownState::kDown);
    }
    std::cout << i << "," << static_cast<int>(test_tracker.GetMotorState()) << "," << test_tracker.IsReady()
              << std::endl;
  }
}

#endif
