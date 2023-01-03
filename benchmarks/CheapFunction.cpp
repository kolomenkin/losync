#include <losync/cheap_function.h>

#include "BenchCommon.h"

#include <benchmark/benchmark.h>

#include <functional>
#include <memory>


using namespace losync;


int globalValue = 123;

NOINLINE static int StdFuncFastInt(const std::function<int(int)>& func, const int value)
{
    return func(value);
}

NOINLINE static int StdFuncFullInt(std::function<int(int)>&& func, const int value)
{
    std::function<int(int)> func2 = std::move(func);
    return func2(value);
}

NOINLINE static int CheapFuncFastInt(const cheap_function<int(int)>& func, const int value)
{
    return func(value);
}

NOINLINE static int CheapFuncFullInt(cheap_function<int(int)>&& func, const int value)
{
    const cheap_function<int(int)> func2 = std::move(func);
    return func2(value);
}


struct FewInts
{
    int values[16] = {1, 33, 55, 23, 765, 22, 567, 543};
};

struct FewStrings
{
    std::string s1;
    std::string s2;
    std::string s3;
};

struct FewSharedPtrs
{
    std::shared_ptr<int> v1;
    std::shared_ptr<int> v2;
    std::shared_ptr<int> v3;
};

struct VeryBigObj
{
    std::string strs[4];
    std::shared_ptr<int> ptr[4];
    bool bools[8] = {false, true, false, true, true};
};


namespace data_gen
{

NOINLINE static int generateCapturedData(int*)
{
    return 42;
}

NOINLINE static std::string generateCapturedData(std::string*)
{
    using namespace std::string_view_literals;
    return std::string{"abcdefghijklmnopq"sv};
}

NOINLINE static FewInts generateCapturedData(FewInts*)
{
    return {};
}

NOINLINE static FewStrings generateCapturedData(FewStrings*)
{
    using namespace std::string_view_literals;
    return {std::string{"abcdefghijklmnopq"sv}, std::string{"abcdefgh"sv}, std::string{"0123456789ABCDEFGH"sv}};
}

NOINLINE static FewSharedPtrs generateCapturedData(FewSharedPtrs*)
{
    using namespace std::string_view_literals;
    return {std::make_shared<int>(111), std::make_shared<int>(222), std::make_shared<int>(333)};
}

NOINLINE static VeryBigObj generateCapturedData(VeryBigObj*)
{
    using namespace std::string_view_literals;
    VeryBigObj result;
    for (auto& value : result.strs)
        value = "abcdefghijklmnopq"sv;
    for (auto& value : result.ptr)
        value = std::make_shared<int>(55);
    return result;
}

} // namespace data_gen


template <typename T>
class CheapTester
{
public:
    static constexpr int Count = 100 * 1000;

    static void PrepareData(std::vector<T>& data)
    {
        data.clear();
        data.reserve(Count);
        T* ptr = nullptr;

        for (int i = 0; i < Count; ++i)
        {
            data.emplace_back(data_gen::generateCapturedData(ptr));
        }
    }

    static void StdFuncFast(benchmark::State& state)
    {
        const int delta = globalValue;
        std::vector<T> data;
        PrepareData(data);

        for (auto value : state)
        {
            if (data.empty())
            {
                state.PauseTiming();
                PrepareData(data);
                state.ResumeTiming();
            }
            auto lambda = [delta, data = std::move(data.back())](int value) { return value + delta; };
            data.pop_back();
            benchmark::DoNotOptimize(StdFuncFastInt(std::move(lambda), 345));
        }
    }

    static void StdFuncFull(benchmark::State& state)
    {
        const int delta = globalValue;
        std::vector<T> data;
        PrepareData(data);
        for (auto value : state)
        {
            if (data.empty())
            {
                state.PauseTiming();
                PrepareData(data);
                state.ResumeTiming();
            }
            auto lambda = [delta, data = std::move(data.back())](int value) { return value + delta; };
            data.pop_back();
            benchmark::DoNotOptimize(StdFuncFullInt(std::move(lambda), 345));
        }
    }

    static void CheapFuncFast(benchmark::State& state)
    {
        const int delta = globalValue;
        std::vector<T> data;
        PrepareData(data);
        for (auto value : state)
        {
            if (data.empty())
            {
                state.PauseTiming();
                PrepareData(data);
                state.ResumeTiming();
            }
            auto lambda = [delta, data = std::move(data.back())](int value) { return value + delta; };
            data.pop_back();
            benchmark::DoNotOptimize(CheapFuncFastInt(std::move(lambda), 345));
        }
    }

    static void CheapFuncFull(benchmark::State& state)
    {
        const int delta = globalValue;
        std::vector<T> data;
        PrepareData(data);
        for (auto value : state)
        {
            if (data.empty())
            {
                state.PauseTiming();
                PrepareData(data);
                state.ResumeTiming();
            }
            auto lambda = [delta, data = std::move(data.back())](int value) { return value + delta; };
            data.pop_back();
            benchmark::DoNotOptimize(CheapFuncFullInt(std::move(lambda), 345));
        }
    }
};


template <typename T>
using Bench = CheapTester<T>;

BENCHMARK(*Bench<int>::StdFuncFast);
BENCHMARK(*Bench<int>::CheapFuncFast);
BENCHMARK(*Bench<int>::StdFuncFull);
BENCHMARK(*Bench<int>::CheapFuncFull);

BENCHMARK(*Bench<FewInts>::StdFuncFast);
BENCHMARK(*Bench<FewInts>::CheapFuncFast);
BENCHMARK(*Bench<FewInts>::StdFuncFull);
BENCHMARK(*Bench<FewInts>::CheapFuncFull);

BENCHMARK(*Bench<std::string>::StdFuncFast);
BENCHMARK(*Bench<std::string>::CheapFuncFast);
BENCHMARK(*Bench<std::string>::StdFuncFull);
BENCHMARK(*Bench<std::string>::CheapFuncFull);

BENCHMARK(*Bench<FewStrings>::StdFuncFast);
BENCHMARK(*Bench<FewStrings>::CheapFuncFast);
BENCHMARK(*Bench<FewStrings>::StdFuncFull);
BENCHMARK(*Bench<FewStrings>::CheapFuncFull);

BENCHMARK(*Bench<FewSharedPtrs>::StdFuncFast);
BENCHMARK(*Bench<FewSharedPtrs>::CheapFuncFast);
BENCHMARK(*Bench<FewSharedPtrs>::StdFuncFull);
BENCHMARK(*Bench<FewSharedPtrs>::CheapFuncFull);

BENCHMARK(*Bench<VeryBigObj>::StdFuncFast);
BENCHMARK(*Bench<VeryBigObj>::CheapFuncFast);
BENCHMARK(*Bench<VeryBigObj>::StdFuncFull);
BENCHMARK(*Bench<VeryBigObj>::CheapFuncFull);
