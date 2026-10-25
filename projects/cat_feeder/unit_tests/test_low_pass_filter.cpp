/*
 * Unit Test: Simple low pass filter implementation
 *
 */

#ifdef GTEST
#include <gtest/gtest.h>

#include "custom_components/cat_feeder_logic/low_pass_filter.h"

using namespace esphome::cat_feeder_logic;

TEST(TestLowPassFilter, BasicUsage) {
  LowPassFilter lpf;

  EXPECT_NEAR(lpf.ReadAlpha(), 0.0f, 1e-6);
  EXPECT_NEAR(lpf.ReadPriorFilter(), 0.0f, 1e-6);
  EXPECT_NEAR(lpf.ReadCutoffFreqHz(), 1.0f, 1e-6);

  lpf.SetInitialValues(0.0f, 1.0f);

  float input = 10.0f;
  float result = 0.0f;
  for (uint32_t i = 0; i < 8; i++) {
    result = lpf.Filter(input, 1.0f);
    EXPECT_LT(result, input);
  }
  EXPECT_NEAR(result, input, 0.001);

  input = 0.0f;
  for (uint32_t i = 0; i < 8; i++) {
    result = lpf.Filter(input, 1.0f);
    EXPECT_GT(result, input);
  }
  EXPECT_NEAR(result, input, 0.001);

  input = 1000.0f;
  result = lpf.Filter(input, 10.0f);
  EXPECT_NEAR(result, lpf.ReadPriorFilter(), 0.001);
  EXPECT_GT(lpf.ReadPriorFilter(), 0.0f);

  lpf.Reset();
  EXPECT_NEAR(lpf.ReadPriorFilter(), 0.0f, 0.001);
}

TEST(TestLowPassFilter, BadValues) {
  LowPassFilter lpf;

  lpf.SetInitialValues(0.0f, 1e-9);  /// To small cutoff freq (clipped)
  EXPECT_NEAR(lpf.ReadCutoffFreqHz(), LowPassFilter::kMinCutoffFreq, 1e-6);

  float result = lpf.Filter(100.0f, 1e-9);  /// To small delta time (returns input)
  EXPECT_NEAR(result, 100.0f, 0.001);
}

#endif
