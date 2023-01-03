#pragma once

#include <cstddef>
#include <type_traits>

template <typename T>
class cheap_function;

template <typename Ret, typename... Args>
class cheap_function<Ret(Args...)>
{
public:
    cheap_function() = delete;
    cheap_function(const cheap_function&) = delete;

    template <typename TRef, typename = std::enable_if_t<std::is_invocable<TRef, Args...>::value>>
    explicit cheap_function(TRef&& obj)
    {
        static_assert(std::is_move_constructible<TRef>::value);
        static_assert(!std::is_const<TRef>::value, "Don't pass const object to cheap_function<> constructor");
        static_assert(!std::is_lvalue_reference<TRef>::value,
                      "It seems you forgot to wrap cheap_function<> constructor argument with std::move");

        using T = typename std::remove_reference<TRef>::type;
        using ActualWrapper = Wrapper<T>;

        static_assert(sizeof(T) <= sizeOfWrapperBuffer);
        static_assert(static_cast<WrapperBase*>(static_cast<ActualWrapper*>(nullptr)) == nullptr,
                      "ActualWrapper pointer is expected to be binary equal to its interface pointer");

        ActualWrapper* const wrapper = reinterpret_cast<ActualWrapper*>(&wrapperBuffer);
        new (wrapper) ActualWrapper(std::move(obj));
    }

    template <typename T2>
    explicit cheap_function(cheap_function<T2>&& b)
    {
        // This function prevents wrapping of cheap_function<T> into cheap_function<U> during construction
        // and gives informative error in case of mistake
        static_assert(false, "Cannot convert different specializations of cheap_function template");
    }

    template <>
    explicit cheap_function(cheap_function&& b)
    {
        b.getWrapper()->placement_move_self(getWrapper());
    }

    ~cheap_function()
    {
        getWrapper()->~WrapperBase();
    }

    cheap_function& operator=(const cheap_function&) = delete;
    cheap_function& operator=(cheap_function&& b)
    {
        if (this != &b)
        {
            getWrapper()->~WrapperBase();
            b.getWrapper()->placement_move_self(getWrapper());
        }
        return *this;
    }

    Ret operator()(Args&&... args) const
    {
        return getWrapper()->call(std::forward<Args>(args)...);
    }

private:
    class WrapperBase
    {
    public:
        virtual ~WrapperBase() = default;
        virtual void placement_move_construct_value(WrapperBase* place, void* value) = 0;
        virtual void placement_move_self(WrapperBase* place) = 0;
        virtual Ret call(Args&&... args) const = 0;
    };

    template <typename T>
    class Wrapper : public WrapperBase
    {
    public:
        explicit Wrapper(T&& value) : wrapped_value(std::move(value))
        {
        }

        virtual void placement_move_construct_value(WrapperBase* place, void* value) override
        {
            Wrapper* const placePtr = static_cast<Wrapper*>(place);
            T* const valuePtr = static_cast<T*>(value);
            new (placePtr) T(std::move(*valuePtr));
        }

        virtual void placement_move_self(WrapperBase* place) override
        {
            Wrapper* const placePtr = static_cast<Wrapper*>(place);
            new (placePtr) Wrapper(std::move(wrapped_value));
        }

        virtual Ret call(Args&&... args) const override
        {
            return wrapped_value(std::forward<Args>(args)...);
        }

    private:
        T wrapped_value;
    };

    constexpr WrapperBase* getWrapper()
    {
        return reinterpret_cast<WrapperBase*>(&wrapperBuffer);
    }

    constexpr const WrapperBase* getWrapper() const
    {
        return reinterpret_cast<const WrapperBase*>(&wrapperBuffer);
    }

private:
    constexpr static size_t alignValue = alignof(std::max_align_t);
    constexpr static size_t sizeOfWrapperBuffer = 256;
    static_assert(sizeOfWrapperBuffer % alignValue == 0,
                  "sizeOfBuffer should be greater and a multiple of alignValue to optimize memory usage");

    alignas(alignValue) char wrapperBuffer[sizeOfWrapperBuffer];
};
