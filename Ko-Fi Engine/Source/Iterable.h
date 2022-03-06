#pragma once

template<class T, class U>
struct Iterable
{
    T _begin;
    U _end;

    Iterable(T begin, U end)
        : _begin(begin), _end(end)
    {}

    T begin()
    {
        return _begin;
    }

    U end()
    {
        return _end;
    }
};

template<class T, class U>
Iterable<T, U> make_iterable(T t, U u)
{
    return Iterable<T, U>(t, u);
}