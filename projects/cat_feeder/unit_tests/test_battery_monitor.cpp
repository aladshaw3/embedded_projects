/*
 * Unit Test: Battery Monitor
 *
 */

#ifdef GTEST
#include <gtest/gtest.h>

#include <iostream>

#include "custom_components/cat_feeder_logic/battery_monitor.h"

// Include some data handling test utilities
#include "data_handling/csv_map.hpp"
#include "data_handling/file_paths.hpp"

using namespace esphome::cat_feeder_logic;
using namespace test_utilities;

TEST(TestBatteryMonitor, BasicMonitoring) {
  BatteryMonitor test_monitor;
  EXPECT_FALSE(test_monitor.IsSetup());
  test_monitor.Initialize(0.5F, 6.0F, 5.0F, 4.0F, 0.25F, 1.0F);
  EXPECT_TRUE(test_monitor.IsSetup());
  EXPECT_EQ(test_monitor.GetBatteryStatus(), BatteryStatus::kUnknown);

  // first reading should be unfiltered
  test_monitor.IngestReadings(2.95F);
  EXPECT_NEAR(test_monitor.GetBatteryVoltage(), 2.95F * 2.0F, 1e-3);
  EXPECT_EQ(test_monitor.GetBatteryStatus(), BatteryStatus::kOk);
  EXPECT_FALSE(test_monitor.IsNewlyAlarmed());

  // sudden drop gets filtered, so it does not immediately alarm
  test_monitor.IngestReadings(2.4F);
  EXPECT_GT(test_monitor.GetBatteryVoltage(), 2.4F * 2.0F);
  EXPECT_EQ(test_monitor.GetBatteryStatus(), BatteryStatus::kOk);
  EXPECT_FALSE(test_monitor.IsNewlyAlarmed());

  // continued low readings will cause the alarm
  test_monitor.IngestReadings(2.4F);
  EXPECT_GE(test_monitor.GetBatteryVoltage(), 2.4F * 2.0F);  // now close enough
  EXPECT_EQ(test_monitor.GetBatteryStatus(), BatteryStatus::kAlarmed);
  EXPECT_TRUE(test_monitor.IsNewlyAlarmed());  // new alarm triggers

  // Once more, we are still alarmed, but it is no longer newly alarmed
  test_monitor.IngestReadings(2.4F);
  EXPECT_EQ(test_monitor.GetBatteryStatus(), BatteryStatus::kAlarmed);
  EXPECT_FALSE(test_monitor.IsNewlyAlarmed());  // alarm is not new anymore

  // Battery percentage is now less than 50%
  EXPECT_LT(test_monitor.GetBatteryPercentage(), 50.0F);

  // Battery charges back up
  test_monitor.IngestReadings(3.1F);
  EXPECT_FALSE(test_monitor.IsNewlyAlarmed());
  test_monitor.IngestReadings(3.1F);
  EXPECT_FALSE(test_monitor.IsNewlyAlarmed());
  test_monitor.IngestReadings(3.1F);
  EXPECT_FALSE(test_monitor.IsNewlyAlarmed());
  EXPECT_EQ(test_monitor.GetBatteryStatus(), BatteryStatus::kOk);
  EXPECT_FALSE(test_monitor.IsNewlyAlarmed());

  // Alarms again
  test_monitor.IngestReadings(0.0F);
  EXPECT_TRUE(test_monitor.IsNewlyAlarmed());
  EXPECT_EQ(test_monitor.GetBatteryStatus(), BatteryStatus::kAlarmed);

  // No new trigger, but still alarmed
  test_monitor.IngestReadings(0.0F);
  EXPECT_FALSE(test_monitor.IsNewlyAlarmed());
  EXPECT_EQ(test_monitor.GetBatteryStatus(), BatteryStatus::kAlarmed);
}

TEST(TestBatteryMonitor, PercentageClipping) {
  BatteryMonitor test_monitor;
  test_monitor.Initialize(0.5F, 6.0F, 5.0F, 4.0F, 0.25F, 1000.0F);  // effectively no filtering

  // first ready above nominal
  test_monitor.IngestReadings(3.1F);
  EXPECT_NEAR(test_monitor.GetBatteryPercentage(), 100.0F, 1e-3);

  // below dead
  test_monitor.IngestReadings(1.0F);
  EXPECT_NEAR(test_monitor.GetBatteryPercentage(), 0.0F, 1e-3);
}

#endif
