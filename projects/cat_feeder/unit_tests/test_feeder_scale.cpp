/*
 * Unit Test: Battery Monitor
 *
 */

#ifdef GTEST
#include <gtest/gtest.h>

#include <iostream>

#include "custom_components/cat_feeder_logic/feeder_scale.h"

using namespace esphome::cat_feeder_logic;

TEST(TestFeederScale, CalibratedScale) {
  FeederScale test_scale;
  EXPECT_FALSE(test_scale.IsScaleReady());
  EXPECT_FALSE(test_scale.IsFeederLogicReady());

  // Set the scale for data ingestion
  test_scale.SetCalibrations(1.0F, 3100.0F);
  test_scale.SetFilterParams(0.25F, 1.0F);
  EXPECT_TRUE(test_scale.IsScaleReady());
  EXPECT_FALSE(test_scale.IsFeederLogicReady());
  // note: full feeder logic requires the weight params to also be set
  // but we can still run the scale as just a normal scale

  // set the number of startup cycles to 3
  test_scale.SetStartupCycles(3);
  // first 3 data points are discarded (no filter, only returns 0)
  test_scale.IngestReadings(1.0F);
  test_scale.IngestReadings(500.0F);
  test_scale.IngestReadings(-500.0F);
  EXPECT_NEAR(test_scale.GetTotalWeight(), 0.0F, 1e-3);

  // next reading sets the filtered value to the initial valid observation
  test_scale.IngestReadings(-3045.0F);  // raw value
  EXPECT_NEAR(test_scale.GetTotalWeight(), (1.0F * -3045.0F + 3100.0F), 1e-3);

  // if we read something invalid, it is discarded
  test_scale.IngestReadings(-293784702983.0F);
  test_scale.IngestReadings(1923468345.0F);
  EXPECT_NEAR(test_scale.GetTotalWeight(), (1.0F * -3045.0F + 3100.0F), 1e-3);  // still the og value

  // next valid reading is weighted heavier in filter due to extended time of bad values
  test_scale.IngestReadings(-2045.0F);
  EXPECT_NEAR(test_scale.GetTotalWeight(), 879.94189, 1e-3);

  // because we have not set the weight parameters, the food calculations are all zeros
  EXPECT_NEAR(test_scale.GetFoodWeight(), 0.0F, 1e-3);
  EXPECT_EQ(test_scale.GetFoodCyclesRemaining(), 0U);
  EXPECT_NEAR(test_scale.GetFoodPercentage(), 0.0F, 1e-3);
}

TEST(TestFeederScale, CalibratedWithFeederWeights) {
  FeederScale test_scale;
  EXPECT_FALSE(test_scale.IsScaleReady());
  EXPECT_FALSE(test_scale.IsFeederLogicReady());

  test_scale.SetCalibrations(1.0F, 3100.0F);
  test_scale.SetFilterParams(0.25F, 1000.0F);  // effectively no filtering
  test_scale.SetFeederWeights(1000.0F, 3000.0F, 50.0F);
  EXPECT_TRUE(test_scale.IsScaleReady());
  EXPECT_TRUE(test_scale.IsFeederLogicReady());

  // first 2 data points are discarded (no filter, only returns 0)
  test_scale.IngestReadings(1.0F);
  test_scale.IngestReadings(500.0F);
  EXPECT_NEAR(test_scale.GetTotalWeight(), 0.0F, 1e-3);

  // next reading sets the filtered value to the initial valid observation
  test_scale.IngestReadings(-2045.0F);                                          // raw value
  EXPECT_NEAR(test_scale.GetTotalWeight(), (1.0F * -2045.0F + 3100.0F), 1e-3);  // ~1055 g
  EXPECT_NEAR(test_scale.GetFoodWeight(), 55.0F, 1e-3);
  EXPECT_NEAR(test_scale.GetFoodPercentage(), (55.0F / (3000.0F - 1000.0F)) * 100.0F, 1e-3);
  EXPECT_EQ(test_scale.GetFoodCyclesRemaining(), 1U);  // becase 50 g per cycle and we have 55 g of food

  // lets say we lose 60 g of weight
  test_scale.IngestReadings(-2045.0F - 60.0F);            // b/c gain == 1
  EXPECT_NEAR(test_scale.GetTotalWeight(), 995.0F, 0.1);  // ~995 g +/- .1 g
  EXPECT_NEAR(test_scale.GetFoodWeight(), 0.0F, 1e-3);    // can't be negative
  EXPECT_NEAR(test_scale.GetFoodPercentage(), 0.0F, 1e-3);
  EXPECT_EQ(test_scale.GetFoodCyclesRemaining(), 0U);

  // lets say we gain 4000 g
  test_scale.IngestReadings(-2045.0F - 60.0F + 4000.0F);
  EXPECT_NEAR(test_scale.GetTotalWeight(), 4992.455F, 0.1);           // over the max fullness
  EXPECT_NEAR(test_scale.GetFoodWeight(), 4992.455F - 1000.0F, 0.1);  // food weight can exceed 100%
  EXPECT_NEAR(test_scale.GetFoodPercentage(), 100.0F, 1e-3);          // but percentage is clipped at 100%
  EXPECT_EQ(test_scale.GetFoodCyclesRemaining(), 79U);
}

#endif
