// File just to test cmake and googletest build
#ifdef GTEST
#include <gtest/gtest.h>

TEST(BuildTest, DoesItBuild) { EXPECT_TRUE(true); }

#endif
