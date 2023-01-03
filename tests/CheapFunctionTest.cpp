#include <losync/cheap_function.h>

#include <gtest/gtest.h>

#include <memory>


using namespace losync;


TEST(CheapFunction, Construct)
{
    int callCount = 0;
    auto functor = [&callCount]() { ++callCount; };
    {
        cheap_function<void()> func(std::move(functor));
    }
    EXPECT_EQ(callCount, 0);
}

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

TEST(CheapFunction, CanHandleMoveOnlyLambda)
{
    auto ptr = std::make_unique<int>(123);
    auto functor = [ptr = std::move(ptr)]() { return *ptr; };
    EXPECT_EQ(functor(), 123);

    cheap_function<int()> func(std::move(functor));
    EXPECT_EQ(func(), 123);

    cheap_function<int()> func2(std::move(func));
    EXPECT_EQ(func2(), 123);
}
