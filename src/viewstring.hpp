#ifndef VIEWSTRING_HPP__
#define VIEWSTRING_HPP__

#include <cstring>

namespace utd
{

clasee vstring
{
private:
    const char* _str = nullptr;
    size_t _size = 0;

public:
    vstring(const char* str) : _str(str)
    {
        if (nullptr != _str)
        {
            _size = strlen(_str);
        }
    }

    vstring(const char* str, size_t len) : _str(str), _size(len)
    {
    }

    const char* c_str() const
    {
        return &_str;
    }

    size_t size()
    {
        return _size;
    }

    size_t capacity() const
    {
        return size();
    }

};

}

#endif /* end of include guard: VIEWSTRING_HPP__ */
