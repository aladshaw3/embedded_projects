/*
 * Unit Test: Simple low pass filter implementation
 *
 */

#ifdef GTEST
#include <gtest/gtest.h>

#include "custom_components/cat_feeder_logic/circular_buffer.h"

using namespace esphome::cat_feeder_logic;

TEST(TestCircularBuffer, BasicUsage) {
  CircularBuffer<int, 3> cb_data;

  EXPECT_EQ(cb_data.Size(), 0);
  EXPECT_EQ(cb_data.GetHead(), -1);

  cb_data.Push(0);
  EXPECT_EQ(cb_data.GetHead(), 0);
  EXPECT_EQ(cb_data.Size(), 1);
  EXPECT_EQ(cb_data[0], 0);
  EXPECT_EQ(cb_data[1], 0);  // circles around
  EXPECT_EQ(cb_data[2], 0);  // circles around again

  cb_data.Push(1);
  EXPECT_EQ(cb_data.GetHead(), 1);
  EXPECT_EQ(cb_data[0], 1);
  EXPECT_EQ(cb_data[1], 0);
  EXPECT_EQ(cb_data[2], 1);  // circles around

  cb_data.Push(2);
  EXPECT_EQ(cb_data.GetHead(), 2);
  EXPECT_EQ(cb_data[0], 2);
  EXPECT_EQ(cb_data[1], 1);
  EXPECT_EQ(cb_data[2], 0);
  EXPECT_EQ(cb_data[3], 2);  // circles around

  cb_data.Push(3);
  EXPECT_EQ(cb_data.GetHead(), 0);
  EXPECT_EQ(cb_data.Size(), 3);
  EXPECT_EQ(cb_data[0], 3);
  EXPECT_EQ(cb_data[1], 2);
  EXPECT_EQ(cb_data[2], 1);
  EXPECT_EQ(cb_data.GetArrayIndex(0), 0);
  EXPECT_EQ(cb_data.GetArrayIndex(1), 2);
  EXPECT_EQ(cb_data.GetArrayIndex(2), 1);

  cb_data.Push(4);
  EXPECT_EQ(cb_data.GetHead(), 1);
  EXPECT_EQ(cb_data.Size(), 3);
  EXPECT_EQ(cb_data[0], 4);
  EXPECT_EQ(cb_data[1], 3);
  EXPECT_EQ(cb_data[2], 2);

  EXPECT_EQ(cb_data[-1], 2);  // pythonic!
  EXPECT_EQ(cb_data[-2], 3);  // yahoo!
  EXPECT_EQ(cb_data[-3], 4);  // mathmatical!
}

#endif
