#include "spsc.hpp"
#include "gtest/gtest.h"

TEST(spsc, basics)
{
	spsc_queue<int, 0, 2> queue;
	int elem;
	ASSERT_FALSE(queue.pop(elem));

	ASSERT_TRUE(queue.push(1));
	ASSERT_TRUE(queue.pop(elem));
	EXPECT_EQ(elem, 1);
	ASSERT_FALSE(queue.pop(elem));

	ASSERT_TRUE(queue.push(1));
	ASSERT_TRUE(queue.push(2));
	ASSERT_FALSE(queue.push(3));
	ASSERT_TRUE(queue.pop(elem));
	EXPECT_EQ(elem, 1);
	ASSERT_TRUE(queue.pop(elem));
	EXPECT_EQ(elem, 2);
	ASSERT_FALSE(queue.pop(elem));
}


int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
