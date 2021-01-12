#include "string_allocator.hpp"
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <cstdio>

namespace utd
{

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
template <int SIZE, typename Allocator = tinypool>
class basic_tiny_string
{
public:
    typedef basic_tiny_string<SIZE> self_type;

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
        const char* str = c_str();
        if (nullptr == str || *str == '\0')
        {
            return 0;
        }

        size_t iSize = _read_size();
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
            _append_size(newSize);
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
            _append_size(newSize);
        }
        return copied;
    }

    void clear()
    {
        memset(_str, 0, SIZE);
    }
private:
    size_t _read_size() const
    {
        TFdecodeSize(c_str() + capacity(), static_cast<unsigned char*>(0));
    }
    void _write_size(unsigned char sz)
    {
        TFencodeSize(data() + capacity(), sz);
    }

public:
    void* operator new(size_t sz)
    {
#ifdef DEV_DEBUG
        std::cout << "basic_tiny_string<>::operator new called" << std::endl;
#endif
        Allocator a;
        return a.allocate(sz);
    }

    void operator delete(void* p, size_t sz)
    {
#ifdef DEV_DEBUG
        std::cout << "basic_tiny_string<>::operator delete called" << std::endl;
#endif
        Allocator a;
        return a.deallocate(static_cast<char*>(p), sz);
    }

#include "stringimp.inc.hpp"
};

typedef basic_tiny_string<16> str16_t;
typedef basic_tiny_string<32> str32_t;
typedef basic_tiny_string<64> str64_t;
typedef basic_tiny_string<128> str128_t;
typedef basic_tiny_string<256> str256_t;

template <typename Allocator = tinypool>
class united_tiny_string
{
public:
    typedef united_tiny_string self_type;

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

    template <int SIZE, typename AllocOther>
    united_tiny_string(const basic_tiny_string<SIZE, AllocOther>& str)
    {
        memset(&_umemory, 0, sizeof(_umemory));
        append(str.c_str(), str.size());
    }

    ~united_tiny_string()
    {
        if (false == tiny_string())
        {
            _deallocate(_umemory.str, _full_size());
            _umemory.str = nullptr;
        }
    }

    united_tiny_string(const self_type& that)
    {
        memset(&_umemory, 0, sizeof(_umemory));
        append(that);
    }

    united_tiny_string(self_type&& that) noexcept
    {
        memset(&_umemory, 0, sizeof(_umemory));
        swap(that);
    }

    self_type& operator =(self_type&& that) noexcept
    {
        if (this != &that)
        {
            clear();
            swap(that);
        }
        return *this;
    }

    void swap(self_type& that)
    {
        if (this == &that)
        {
            return;
        }
        char tmp[sizeof(*this)];
        memcpy(tmp, this, sizeof(*this));
        memcpy(this, &that, sizeof(*this));
        memcpy(&that, tmp, sizeof(*this));
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

    str16_t* str16() const
    {
        if (tiny_string())
        {
            return reinterpret_cast<str16_t*>(data());
        }
        return nullptr;
    }

    str32_t* str32() const
    {
        if (small_string() && capacity() == 31)
        {
            return reinterpret_cast<str32_t*>(data());
        }
        return nullptr;
    }

    str64_t* str64() const
    {
        if (small_string() && capacity() == 63)
        {
            return reinterpret_cast<str64_t*>(data());
        }
        return nullptr;
    }

    str128_t* str128() const
    {
        if (small_string() && capacity() == 127)
        {
            return reinterpret_cast<str128_t*>(data());
        }
        return nullptr;
    }

    str256_t* str256() const
    {
        if (small_string() && capacity() == 255)
        {
            return reinterpret_cast<str256_t*>(data());
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
        const char* str = c_str();
        if (nullptr == str || *str == '\0')
        {
            return 0;
        }

        size_t iSize = _read_size();
        if (iSize == 0)
        {
            return capacity();
        }
        else
        {
            return iSize;
        }
    }

    size_t max_size() const
    {
        return 0xFFFFFFFF;
    }

    self_type& append(char c)
    {
        size_t iSize = size();
        bool flag = false;

        iSize++;
        if (iSize <= capacity())
        {
            flag = _append_local(c);
        }
        else if(_enlarge(iSize))
        {
            flag = _append_local(c);
        }

        if (flag)
        {
            _append_size(iSize);
        }
        return *this;
    }

    self_type& append(const char* str, size_t len = 0)
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
            _append_size(iSize);
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
            _deallocate(_umemory.str, _full_size());
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
        const char* pEnd = c_str() + capacity();
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
        size_t oldSize = _full_size();
        char* pOldData = data();
        bool hasHeap = !tiny_string();

        size_t newSize = _enlarge_size(target);
        char* pNewStr = nullptr;
        pNewStr = (char*) _allocate(newSize);
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
            _deallocate(pOldData, oldSize);
        }

        return true;
    }

    void* _allocate(size_t sz)
    {
        Allocator a;
        return a.allocate(sz);
    }

    void _deallocate(char* p, size_t sz)
    {
        Allocator a;
        return a.deallocate(p, sz);
    }

#include "stringimp.inc.hpp"
};

typedef united_tiny_string<> ustring;
}
