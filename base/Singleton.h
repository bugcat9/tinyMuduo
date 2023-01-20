#ifndef _SINGLETON_H
#define _SINGLETON_H

#include <boost/noncopyable.hpp>
#include <mutex>
#include <assert.h>

namespace tinyMuduo
{

    template <typename T>
    struct has_no_destroy
    {
        template <typename C>
        static char test(decltype(&C::no_destroy));
        template <typename C>
        static int32_t test(...);
        const static bool value = sizeof(test<T>(0)) == 1;
    };

    template <typename T>
    class Singleton : boost::noncopyable
    {

    public:
        Singleton() = delete;
        ~Singleton() = delete;

        static T &instance()
        {
            std::call_once(ponce_, &Singleton::init);
            assert(value_ != NULL);
            return *value_;
        }

    private:
        static void init()
        {
            value_ = new T();
            if (!has_no_destroy<T>::value)
            {
                ::atexit(destroy);
            }
        }

        static void destroy()
        {
            // 不完全类型检查
            typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
            T_must_be_complete_type dummy;
            (void)dummy;

            delete value_;
            value_ = NULL;
        }

        static std::once_flag ponce_;
        static T *value_;
    };

    template <typename T>
    std::once_flag Singleton<T>::ponce_ ;

    template <typename T>
    T *Singleton<T>::value_ = NULL;

} // namespace tinyMuduo

#endif // _SINGLETON_H