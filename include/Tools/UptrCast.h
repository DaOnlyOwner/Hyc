#pragma once
#include <memory>

//From: https://stackoverflow.com/questions/26377430/how-to-perform-a-dynamic-cast-with-a-unique-ptr
template <typename To, typename From, typename Deleter>
std::unique_ptr<To, Deleter> dynamic_unique_cast(std::unique_ptr<From, Deleter>&& p) {
    if (To* cast = dynamic_cast<To*>(p.get()))
    {
        std::unique_ptr<To, Deleter> result(cast, std::move(p.get_deleter()));
        p.release();
        return result;
    }
    return std::unique_ptr<To, Deleter>(nullptr); // or throw std::bad_cast() if you prefer
}

template<typename To, typename From>
std::unique_ptr<To> dynamic_uptr_cast_no_deleter(std::unique_ptr<From>&& p)
{
    if (To* cast = dynamic_cast<To*>(p.get()))
    {
        std::unique_ptr<To> result(cast);
        p.release();
        return result;
    }
    return nullptr;
}



