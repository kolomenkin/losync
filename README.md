# Losync: Generic C++ Synchronization Library

## cheap_function

`#include <losync/cheap_function.h>`

`cheap_function` is a template class for lightweight replacement of `std::function`.
Its features:

1. The implementation does not allocate dynamic memory.
   But the class is limited by the maximum size of the functor.
2. The class does not support copying. It only supports move semantics.
   So it can wrap the lambda that captured the `std::unique_ptr`.
