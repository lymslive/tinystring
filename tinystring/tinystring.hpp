#include "allocator.h"
#include <cstring>
#include <iostream>
#include <cstdlib>

// SizeT maybe one of [(unsgined) char, short, int]
template <typename SizeT>
inline 
size_t TFdecodeSize(const char* ptr, const SizeT*)
{
    return (nullptr == ptr) ? 0 : *(reinterpret_cast<const SizeT*>(ptr));
}
template <typename SizeT>
inline 
void TFencodeSize(char* ptr, SizeT size)
{
    if (nullptr == ptr)
    {
        return;
    }
    *(reinterpret_cast<SizeT*>(ptr)) = size;
}

// SIZE <= 256; then use the tail char byte to store string length
template <int SIZE>
class basic_tiny_string
{
public:
    typedef basic_tiny_string<SIZE> selfType;

private:
    char _str[SIZE];

public:
    basic_tiny_string()
    {
        memset(_str, 0, SIZE);
    }

    basic_tiny_string(const char* str, size_t len = 0)
    {
        memset(_str, 0, SIZE);
        append(str, len);
    }

    const char* c_str() const
    {
        return &_str[0];
    }

    size_t capacity() const
    {
        return SIZE - 1;
    }

    size_t size() const
    {
        if (_str[0] == '\0')
        {
            return 0;
        }

        unsigned char iSize = _readSize();
        if (iSize == 0)
        {
            return capacity();
        }
        else
        {
            return iSize;
        }
    }

    bool append(char c)
    {
        size_t preSize = size();
        if (_append_local(c))
        {
            unsigned char newSize = preSize + 1;
            if (preSize == 0 || _str[SIZE-1] != '\0')
            {
                _writeSize(newSize);
            }
        }
    }

    size_t append(const char* str, size_t len = 0)
    {
        size_t preSize = size();
        size_t copied = 0;
        if (len == 0)
        {
            copied = _append_local(str);
        }
        else
        {
            copied = _append_local(str, len);
        }

        if (copied > 0)
        {
            unsigned char newSize = preSize + copied;
            if (preSize == 0 || _str[SIZE-1] != '\0')
            {
                _writeSize(newSize);
            }
        }
        return copied;
    }

    void clear()
    {
        memset(_str, 0, SIZE);
    }
private:
    unsigned char _readSize() const
    {
        TFdecodeSize(c_str() + SIZE - 1, static_cast<unsigned char*>(0));
    }
    void _writeSize(unsigned char sz)
    {
        TFencodeSize(data() + SIZE - 1, sz);
    }

#include "stringimp.inc.hpp"
};

typedef basic_tiny_string<16> char_16t;
typedef basic_tiny_string<32> char_32t;
typedef basic_tiny_string<64> char_64t;
typedef basic_tiny_string<128> char_128t;
typedef basic_tiny_string<256> char_256t;

class united_tiny_string
{
public:
    typedef united_tiny_string selfType;

private:
    struct SHeapMeta
    {
        char pointerPadding[8];
        unsigned int   largeCapacity; // can store max 0xFFFFFFFF length
        unsigned short middleCapacity;// can store max 0xFFFF length
        unsigned char  smallCapacity; // can store max 0xFF length
        unsigned char  tinyLength; // stack tiny string max 0x0F(15) length
        // capacity store in stack, length store at the end of pointed heap
    };
    union ULayoutRep
    {
        char* str;
        char byte[16];
        SHeapMeta meta;
    };

    ULayoutRep _umemory;

    enum EndMarker
    {
        TINY_STRING = 0x0F, // 15 local stack
        SMALL_STRING, // char length
        MIDDLE_STRING, // short length
        LARGE_STRING, // int length
    };

public:
    united_tiny_string()
    {
        memset(&_umemory, 0, sizeof(_umemory));
    }

    united_tiny_string(const char* str, size_t len = 0)
    {
        memset(&_umemory, 0, sizeof(_umemory));
        append(str, len);
    }

    ~united_tiny_string()
    {
        if (false == tiny_string())
        {
            free(_umemory.str);
            _umemory.str = nullptr;
        }
    }

    bool tiny_string() const
    {
        return _umemory.meta.tinyLength <= TINY_STRING; // < is ok
    }
    bool small_string() const
    {
        return _umemory.meta.tinyLength == SMALL_STRING;
    }
    bool middle_string() const
    {
        return _umemory.meta.tinyLength == MIDDLE_STRING;
    }
    bool large_string() const
    {
        return _umemory.meta.tinyLength == LARGE_STRING;
    }

    char_16t* str16() const
    {
        if (tiny_string())
        {
            return reinterpret_cast<char_16t*>(data());
        }
        return nullptr;
    }

    char_32t* str32() const
    {
        if (small_string() && capacity() == 31)
        {
            return reinterpret_cast<char_32t*>(data());
        }
        return nullptr;
    }

    char_64t* str64() const
    {
        if (small_string() && capacity() == 63)
        {
            return reinterpret_cast<char_64t*>(data());
        }
        return nullptr;
    }

    char_128t* str128() const
    {
        if (small_string() && capacity() == 127)
        {
            return reinterpret_cast<char_128t*>(data());
        }
        return nullptr;
    }

    char_256t* str256() const
    {
        if (small_string() && capacity() == 255)
        {
            return reinterpret_cast<char_256t*>(data());
        }
        return nullptr;
    }

    const char* c_str() const
    {
        if (tiny_string())
        {
            return &_umemory.byte[0];
        }

        return _umemory.str;
    }

    size_t capacity() const
    {
        return _read_capacity();
    }

    size_t size() const
    {
        return _read_size();
    }

    size_t max_size() const
    {
        return 0xFFFFFFFF;
    }

    selfType& append(char c)
    {
        size_t iSize = size();
        bool flag = false;

        if (size() < capacity())
        {
            flag = _append_local(c);
        }
        else if(_enlarge(iSize + 1))
        {
            flag = _append_local(c);
        }

        if (flag)
        {
            iSize++;
            if (iSize == capacity())
            {
                _write_size(0);
            }
            else
            {
                _write_size(iSize);
            }
        }
        return *this;
    }

    selfType& append(const char* str, size_t len = 0)
    {
        if (len == 0)
        {
            len = strlen(str);
        }

        size_t iSize = size();
        size_t copied = 0;

        iSize += len;
        if (iSize <= capacity())
        {
            copied = _append_local(str, len);
        }
        else if(_enlarge(iSize))
        {
            copied = _append_local(str, len);
        }

        if (copied > 0)
        {
            if (iSize == capacity())
            {
                _write_size(0);
            }
            else
            {
                _write_size(iSize);
            }
        }
        return *this;
    }

    // reserve double space
    void enlarge()
    {
        _enlarge();
    }

    void reserve(size_t newCap)
    {
        _enlarge(newCap);
    }

    void clear()
    {
        if (!tiny_string())
        {
            delete _umemory.str;
        }
        memset(&_umemory, 0, sizeof(_umemory));
    }

private:
    size_t _read_capacity() const
    {
        if (tiny_string())
        {
            return sizeof(_umemory) - 1;
        }
        else if (small_string())
        {
            return _umemory.meta.smallCapacity;
        }
        else if (middle_string())
        {
            return _umemory.meta.middleCapacity;
        }
        else if (large_string())
        {
            return _umemory.meta.largeCapacity;
        }
        return (size_t)-1;
    }

    size_t _read_size() const
    {
        const char* pBegin = c_str();
        if (nullptr == pBegin || *pBegin == '\0')
        {
            return 0;
        }

        const char* pEnd = pBegin + capacity();
    size_t iSize = 0;
        if (tiny_string())
        {
            iSize = _umemory.meta.tinyLength;
        }
        else if (small_string())
        {
            iSize = TFdecodeSize(pEnd, &_umemory.meta.smallCapacity);
        }
        else if (middle_string())
        {
            iSize = TFdecodeSize(pEnd, &_umemory.meta.middleCapacity);
        }
        else if (large_string())
        {
            iSize = TFdecodeSize(pEnd, &_umemory.meta.largeCapacity);
        }

        // no end length, overwirte by '\0', full string
        if (iSize == 0)
        {
            return capacity();
        }
        else
        {
            return iSize;
        }
    }

    void _write_size(size_t sz)
    {
        char* pEnd = data() + capacity();
        if (tiny_string())
        {
            _umemory.meta.tinyLength = (unsigned char)sz;
        }
        else if (small_string())
        {
            TFencodeSize(pEnd, (unsigned char)sz);
        }
        else if (middle_string())
        {
            TFencodeSize(pEnd, (unsigned short)sz);
        }
        else if (large_string())
        {
            TFencodeSize(pEnd, (unsigned int)sz);
        }
    }

    // full size is string capacity adding tail length bytes
    size_t _full_size()
    {
        if (tiny_string() || small_string())
        {
            return capacity() + sizeof(_umemory.meta.smallCapacity);
        }
        else if (middle_string())
        {
            return capacity() + sizeof(_umemory.meta.middleCapacity);
        }
        else if (large_string())
        {
            return capacity() + sizeof(_umemory.meta.largeCapacity);
        }
        return (size_t)-1;
    }

    size_t _enlarge_size(size_t target)
    {
        size_t oldSize = _full_size();
        size_t newSize = 0;
        if (target == 0)
        {
            newSize = oldSize * 2;
        }
        else if (oldSize < 0xFF && target <= 0xFF)
        {
            newSize = oldSize * 2;
            while (newSize < target + 1)
            {
                newSize *= 2;
            }
        }
        else
        {
            size_t diff = target - capacity();
            size_t extra = 2 * diff;
            if (extra < oldSize / 2)
            {
                extra = oldSize / 2;
            }
            newSize = oldSize + extra;
        }
        return newSize;
    }

    bool _enlarge(size_t target = 0)
    {
        if (target > 0 && target < capacity())
        {
            return false;
        }

        size_t preSize = size();
        char* pOldData = data();
        bool hasHeap = !tiny_string();

        size_t newSize = _enlarge_size(target);
        char* pNewStr = nullptr;
        pNewStr = (char*) malloc(newSize);
        if (nullptr == pNewStr)
        {
            return false;
        }

        memset(pNewStr, 0, newSize);
        memcpy(pNewStr, data(), size());

        memset(&_umemory, 0, sizeof(_umemory));
        if (newSize <= 0xFF + 1)
        {
            _umemory.meta.tinyLength = SMALL_STRING;
            _umemory.meta.smallCapacity = newSize - 1;
        }
        else if(newSize <= 0xFFFF + 2)
        {
            _umemory.meta.tinyLength = MIDDLE_STRING;
            _umemory.meta.middleCapacity = newSize - 2;
        }
        else
        {
            _umemory.meta.tinyLength = LARGE_STRING;
            _umemory.meta.largeCapacity = newSize - 4;
        }

        _umemory.str = pNewStr;
        _write_size(preSize);

        if (hasHeap)
        {
            free(pOldData);
        }

        return true;
    }

#include "stringimp.inc.hpp"
};

typedef united_tiny_string ustring;