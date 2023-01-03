#pragma once

#include <cstddef>
#include <type_traits>


template<typename T>
class cheap_function;

template<typename Ret, typename... Args>
class cheap_function<Ret(Args...)>
{
public:
    cheap_function() = delete;
    cheap_function(const cheap_function&) = delete;

    template<typename TRef, typename = std::enable_if_t<std::is_invocable<TRef, Args...>::value>>
    explicit cheap_function(TRef&& obj)
    {
        static_assert(!std::is_const<TRef>::value, "Don't pass const object to cheap_function<> constructor");
        static_assert(!std::is_lvalue_reference<TRef>::value, "It seems you forgot to wrap cheap_function<> constructor argument with std::move");
        using T = typename std::remove_reference<TRef>::type;

        class Controller : public IController
        {
        public:
            virtual void move_construct(void* that, void* b) override
            {
                T* const thatPtr = static_cast<T*>(that);
                T* const thatB = static_cast<T*>(b);
                new (thatPtr) T(std::move(*thatB));
            }
            virtual void destroy_object(void* that) override
            {
                T* const thatPtr = static_cast<T*>(that);
                thatPtr->~T();
            }
            virtual Ret call(const void* that, Args&&...args) const override
            {
                const T* const thatPtr = static_cast<const T*>(that);
                return (*thatPtr)(std::forward<Args>(args)...);
            }
        };

        static_assert(sizeof(Controller) == sizeof(controllerBuffer), "The only expected data in Destroyer is the pointer to a virtual table");
        static_assert(static_cast<IController*>(static_cast<Controller*>(nullptr)) == nullptr,
            "Interface pointer is expected to be binary equal to the object pointer when we have only one inheritance");
        Controller* const controller = reinterpret_cast<Controller*>(&controllerBuffer);
        new (controller) Controller;

        static_assert(sizeof(T) <= sizeOfBuffer);
        controller->move_construct(&buffer[0], &obj);
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
        controllerBuffer = b.controllerBuffer;
        b.getController()->move_construct(&buffer[0], &b.buffer[0]);
        b.controllerBuffer = nullptr;
    }

    ~cheap_function()
    {
        if (controllerBuffer)
        {
            getController()->destroy_object(&buffer[0]);
            controllerBuffer = nullptr; // this is optional
        }
    }

    cheap_function& operator=(const cheap_function&) = delete;
    cheap_function& operator=(cheap_function&& b)
    {
        if (this != &b)
        {
            getController()->destroy_object(&buffer[0]);
            controllerBuffer = b.controllerBuffer;
            b.getController()->move_construct(&buffer[0], &b.buffer[0]);
            b.controllerBuffer = nullptr;
        }
        return *this;
    }

    Ret operator()(Args&&...args) const
    {
        return getController()->call(&buffer[0], std::forward<Args>(args)...);
    }

private:
    class IController
    {
    public:
        virtual void move_construct(void* that, void* b) = 0;
        virtual void destroy_object(void* that) = 0;
        virtual Ret call(const void* that, Args&&...args) const = 0;
    };

    IController* getController()
    {
        return reinterpret_cast<IController*>(&controllerBuffer);
    }

    const IController* getController() const
    {
        return reinterpret_cast<const IController*>(&controllerBuffer);
    }

private:
    constexpr static size_t alignValue = alignof(std::max_align_t);
    constexpr static size_t sizeOfBuffer = 64;
    static_assert(sizeOfBuffer% alignValue == 0, "sizeOfBuffer should be greater and a multiple of alignValue");

    void* controllerBuffer; // virtual table pointer

    alignas(alignValue)
        char buffer[sizeOfBuffer];
};
