#ifndef VIEWSTRING_HPP__
#define VIEWSTRING_HPP__

#include <cstring>
#include <tuple>
#include <string>

namespace utd
{

typedef std::tuple<const char*, size_t> tstring;

clasee vstring
{
private:
    size_t _size = 0;
    const char* _str = nullptr;

public:
    typedef vstring self_type;
    typedef char value_type;
    typedef char* iterator;
    typedef const char* const_iterator;
    typedef char* pointer;
    typedef const char* const_pointer;

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

    vstring(const tstring& that)
    {
        _str = std::get<0>(that);
        _size = std::get<1>(that);
    }

    vstring(const std::string& that)
    {
        _str = that.c_str();
        _size = that.size();
    }

    const char* c_str() const
    {
        return _str;
    }

    char* data() const
    {
        return const_cast<char*>(_str);
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
