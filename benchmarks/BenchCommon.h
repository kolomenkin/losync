#pragma once

#ifdef _MSC_VER
#define NOINLINE __declspec(noinline)
#else
#define NOINLINE [[gnu::noinline]]
#endif
