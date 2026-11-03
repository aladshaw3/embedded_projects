/*
 * Unit Test: Simple low pass filter implementation
 *
 */

#ifdef GTEST
#include <gtest/gtest.h>

#include <iostream>

#include "custom_components/cat_feeder_logic/derivatives.h"

using namespace esphome::cat_feeder_logic;

static float data[15] = {0.0123, 0.0251, 0.0146, 0.0214, 0.0215,   // NOLINT
                         0.1544, 0.2155, 0.1947, 0.1845, 0.2148,   // NOLINT
                         0.0123, 0.0251, 0.0146, 0.0214, 0.0215};  // NOLINT

enum TestMethodChoice {
  EULER = 0,
  BDF2 = 1,
  FPS = 2,
};

void rising_falling_helper(TestMethodChoice method_choice, int expected_rise_idx, int expected_fall_idx,
                           float expected_der_at_rise, float expected_der_at_fall) {
  int start_i = -1;
  int idx_rising_edge = -1;
  int idx_falling_edge = -1;
  float max_der = 0;
  float min_der = 0;
  switch (method_choice) {
    case TestMethodChoice::EULER:
      start_i = 1;
      break;
    case TestMethodChoice::BDF2:
      start_i = 2;
      break;
    case TestMethodChoice::FPS:
      start_i = 4;
      break;
  }

  for (int i = start_i; i < 15; i++) {
    float der = 0;
    switch (method_choice) {
      case TestMethodChoice::EULER:
        der = EulerMethod(data[i], data[i - 1], 0.1);
        break;
      case TestMethodChoice::BDF2:
        der = BDF2Method(data[i], data[i - 1], data[i - 2], 0.1);
        break;
      case TestMethodChoice::FPS:
        der = FivePointStencilMethod(data[i], data[i - 1], data[i - 3], data[i - 4], 0.1);
        break;
    }

    if (der > max_der) {
      max_der = der;
      idx_rising_edge = i;
    }
    if (der < min_der) {
      min_der = der;
      idx_falling_edge = i;
    }

    std::cout << "DEBUG: " << i << " , " << der << std::endl;
  }

  EXPECT_EQ(idx_rising_edge, expected_rise_idx);
  EXPECT_EQ(idx_falling_edge, expected_fall_idx);

  EXPECT_NEAR(max_der, expected_der_at_rise, 0.001);
  EXPECT_NEAR(min_der, expected_der_at_fall, 0.001);
}

TEST(TestEulerMethod, FindRisingFallingEdges) { rising_falling_helper(TestMethodChoice::EULER, 5, 10, 1.329, -2.0249); }

TEST(TestBDF2Method, FindRisingFallingEdges) { rising_falling_helper(TestMethodChoice::BDF2, 5, 10, 1.993, -3.189); }

// NOTE: Because the 5 point method is smoother, it may not
// be as appropriate as BDF2 for finding the precise point of an edge.
// In this test case, it misses the rising and falling edges by 2 nodes
TEST(TestFivePointMethod, FindRisingFallingEdges) {
  rising_falling_helper(TestMethodChoice::FPS, 7, 12, 1.14892, -1.12308);
}

#endif
