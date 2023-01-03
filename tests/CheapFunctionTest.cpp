#include "losync/cheap_function.h"

#include <gtest/gtest.h>


TEST(CheapFunction, Basic)
{
    int callCount = 0;
    auto functor = [&callCount]() { ++callCount; };

    {
        cheap_function<void()> func(std::move(functor));
        EXPECT_EQ(callCount, 0);
        func();
        EXPECT_EQ(callCount, 1);
    }
    EXPECT_EQ(callCount, 1);
}
