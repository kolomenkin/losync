#include <losync/cheap_function.h>

#include "BenchCommon.h"

#include <benchmark/benchmark.h>

#include <atomic>
#include <cassert>
#include <cstdint>
#include <new>
#include <thread>


class NaiveCounter
{
public:
    static void Benchmark(benchmark::State& state)
    {
        if (state.thread_index() == 0)
        {
            counter = 0;
        }
        std::int64_t processed = 0;
        for (auto _ : state)
        {
            // 10 operations per round
            processed += 10;
            ++counter;
            ++counter;
            ++counter;
            ++counter;
            ++counter;
            ++counter;
            ++counter;
            ++counter;
            ++counter;
            ++counter;
        }
        state.SetItemsProcessed(processed);
    }

private:
    inline static std::atomic<unsigned> counter = 0;
};


class CounterBase
{
protected:
    inline static const thread_local size_t threadUniqueId = std::hash<std::thread::id>{}(std::this_thread::get_id());

    struct CACHE_ALIGNED Counter
    {
        std::atomic<unsigned> cnt = 0;
    };

    static constexpr size_t ShardSize = 256 * 32;
    inline static Counter counters[ShardSize];
    inline static const size_t ShardActualSize = std::thread::hardware_concurrency() * 16 + 1;
};


class IdealShardedCounter : private CounterBase
{
public:
    static void Benchmark(benchmark::State& state)
    {
        Counter& counter = counters[state.thread_index() % ShardSize];
        counter.cnt = 0;

        std::int64_t processed = 0;
        for (auto _ : state)
        {
            // 10 operations per round
            processed += 10;
            ++counter.cnt;
            ++counter.cnt;
            ++counter.cnt;
            ++counter.cnt;
            ++counter.cnt;
            ++counter.cnt;
            ++counter.cnt;
            ++counter.cnt;
            ++counter.cnt;
            ++counter.cnt;
        }
        state.SetItemsProcessed(processed);
    }
};


class SimplifiedShardedCounter : private CounterBase
{
public:
    static void Benchmark(benchmark::State& state)
    {
        assert(ShardActualSize <= ShardSize);
        const auto threadIdHash = std::hash<std::thread::id>{}(std::this_thread::get_id());
        const size_t shardIndex = threadIdHash % ShardActualSize;
        Counter& counter = counters[shardIndex];
        counter.cnt = 0;

        std::int64_t processed = 0;
        for (auto _ : state)
        {
            // 10 operations per round
            processed += 10;
            ++counter.cnt;
            ++counter.cnt;
            ++counter.cnt;
            ++counter.cnt;
            ++counter.cnt;
            ++counter.cnt;
            ++counter.cnt;
            ++counter.cnt;
            ++counter.cnt;
            ++counter.cnt;
        }
        state.SetItemsProcessed(processed);
    }
};


class ShardedCounter : private CounterBase
{
public:
    NOINLINE static void Inc()
    {
        ++counters[threadUniqueId % ShardActualSize].cnt;
    }

    static void Benchmark(benchmark::State& state)
    {
        assert(ShardActualSize <= ShardSize);
        counters[threadUniqueId % ShardActualSize].cnt = 0;

        std::int64_t processed = 0;
        for (auto _ : state)
        {
            // 10 operations per round
            processed += 10;
            Inc();
            Inc();
            Inc();
            Inc();
            Inc();
            Inc();
            Inc();
            Inc();
            Inc();
            Inc();
        }
        state.SetItemsProcessed(processed);
    }
};


BENCHMARK(ShardedCounter::Benchmark)->ThreadRange(1, 4 * std::thread::hardware_concurrency());
BENCHMARK(SimplifiedShardedCounter::Benchmark)->ThreadRange(1, 4 * std::thread::hardware_concurrency());
BENCHMARK(IdealShardedCounter::Benchmark)->ThreadRange(1, 4 * std::thread::hardware_concurrency());
BENCHMARK(NaiveCounter::Benchmark)->ThreadRange(1, 4 * std::thread::hardware_concurrency());

#if 0
#ifdef WIN32
#include <windows.h>

const bool affinity = []()
{
    return !!SetProcessAffinityMask(GetCurrentProcess(), 0x0FFFF);
}();
#endif
#endif
