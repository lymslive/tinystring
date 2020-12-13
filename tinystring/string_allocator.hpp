#ifndef STRING_ALLOCATOR_HPP__
#define STRING_ALLOCATOR_HPP__

#include <new>
#include <cstdlib>

#ifdef _DEV_DEBUG
#include <cstdio>
#define DDEBUG(msg, ...) printf(msg, ## __VA_ARGS__); printf("\n")
#else
#define DDEBUG(msg, ...)
#endif

template <bool THREAD = false>
class string_allocator
{
public:
    typedef string_allocator self_type;
    typedef char value_type;
    typedef char* pointer;
    typedef const char* const_pointer;
    typedef char& reference;
    typedef const char& const_reference;
    typedef size_t size_type;

public:
    static const size_type MAX_SIZE = 256;
    static const size_type EACH_CHUNK = 8;

    static const size_type BLOCK_VARIANT = 4;
    enum EBlockIndex
    {
        BLOCK_32 = 0,
        BLOCK_64,
        BLOCK_128,
        BLOCK_256,
        BLOCK_TOO_LARGE,
    };

    struct SBlockLink
    {
        SBlockLink* next;
    };

    struct SPoolStatistic
    {
        size_type allocateTimes = 0;
        size_type allocateSpace = 0;
        size_type deallocateTimes = 0;
        size_type deallocateSpace = 0;
        size_type freeSpace = 0;
        size_type freeBlock[BLOCK_VARIANT] = {0, 0, 0, 0};
        size_type totalTimes = 0;
        size_type total = 0;
        size_type blance = 0;
        size_type beyondAllocTimes = 0;
        size_type beyondFreeTimes = 0;
    };

private:

    struct SPoolData
    {
        SBlockLink* freeList[BLOCK_VARIANT] = {nullptr, nullptr, nullptr, nullptr};
        char* freeBegin = nullptr;
        char* freeEnd = nullptr;
        SPoolStatistic report;
    };

    static SPoolData _pool;

public:

    const SPoolStatistic& get_pool_report()
    {
        _report_statistic();
        return _pool.report;
    }

    size_type max_size() const
    {
        return MAX_SIZE;
    }

    void* allocate(size_type nRequest, void* pHint = nullptr)
    {
        size_type index = _check_block(nRequest);
        if (index == BLOCK_TOO_LARGE)
        {
            DDEBUG(">> new space beyond pool");
            _pool.report.beyondAllocTimes++;
            return ::operator new(nRequest);
        }
        return _allocate(nRequest, index, pHint);
    }

    void deallocate(void* pdead, size_type nReturn)
    {
        size_type index = _check_block(nReturn);
        if (index == BLOCK_TOO_LARGE)
        {
            DDEBUG(">> delete space beyond pool");
            _pool.report.beyondFreeTimes++;
            return ::operator delete(pdead, nReturn);
        }
        return _deallocate(pdead, nReturn, index);
    }

private:

    size_type _check_block(size_type& n)
    {
        if (n <= 32)
        {
            n = 32;
            return BLOCK_32;
        }
        else if (n <= 64)
        {
            n = 64;
            return BLOCK_64;
        }
        else if (n <= 128)
        {
            n = 128;
            return BLOCK_128;
        }
        else if (n <= 256)
        {
            n = 256;
            return BLOCK_256;
        }
        else
        {
            return BLOCK_TOO_LARGE;
        }
    }

    void* _allocate(size_type n, size_type index, void* pHint = nullptr)
    {
        SBlockLink*& free = _pool.freeList[index];
        if (nullptr != free)
        {
            // case 1: already have free block of size n.
            DDEBUG(">> Return free bock: %zd[%zd]", n, index);
            void* result = free;
            free = free->next;
            _pool.report.allocateTimes++;
            _pool.report.allocateSpace += n;
            return result;
        }
        else if(nullptr != _pool.freeBegin && _pool.freeBegin + n <= _pool.freeEnd)
        {
            // case 2: still have unlinked free space
            DDEBUG(">> Split free bock: %zd[%zd]", n, index);
            SBlockLink* pBlock = reinterpret_cast<SBlockLink*>(_pool.freeBegin);
            free = pBlock;
            _pool.freeBegin += n;
            for (size_type i = 0; i < EACH_CHUNK - 1 && _pool.freeBegin < _pool.freeEnd; ++i)
            {
                pBlock->next = reinterpret_cast<SBlockLink*>(_pool.freeBegin);
                pBlock = pBlock->next;
                _pool.freeBegin += n;
            }
            pBlock->next = nullptr;
            if (_pool.freeBegin == _pool.freeEnd)
            {
                _pool.freeBegin = nullptr;
                _pool.freeEnd = nullptr;
            }
            else if (_pool.freeBegin > _pool.freeEnd)
            {
                DDEBUG("Error!! imposible happend, freeBein > freeEnd");
            }
            return _allocate(n, index, pHint);
            // recall case 1
        }
        else
        {
            // case 3: no free space, malloc more
            if (nullptr != _pool.freeBegin)
            {
                // memory fragment? should not happen?
                DDEBUG("Error!! Impossible happened, memory fragment");
            }
            size_type chunck = n * EACH_CHUNK * 2;
            void* pMem = ::operator new(chunck);
            if (nullptr == pMem)
            {
                DDEBUG("Error!! fail to allocate size[%zd]", chunck);
                throw std::bad_alloc();
            }
            _pool.freeBegin = static_cast<char*>(pMem);
            _pool.freeEnd = _pool.freeBegin + chunck;
            _pool.report.totalTimes++;
            _pool.report.total += chunck;
            DDEBUG(">> malloc free space: %zd/%zd", chunck, n);
            return _allocate(n, index, pHint);
            // recall case 2
        }
    }

    void _deallocate(void* pdead, size_type nReturn, size_type index)
    {
        SBlockLink*& free = _pool.freeList[index];
        SBlockLink* pBlock = static_cast<SBlockLink*>(pdead);
        pBlock->next = free;
        free = pBlock;
        _pool.report.deallocateTimes++;
        _pool.report.deallocateSpace += nReturn;
    }

    void _report_statistic()
    {
        // unlinked free space
        if (nullptr != _pool.freeBegin && _pool.freeEnd > _pool.freeBegin)
        {
            _pool.report.freeSpace = _pool.freeEnd - _pool.freeBegin;
        }
        else if(nullptr == _pool.freeBegin && nullptr == _pool.freeEnd)
        {
            _pool.report.freeSpace = 0;
        }
        else
        {
            DDEBUG("Error!! freeBgin and freeEnd should match in both null or non-null");
        }

        // linked free blocks
        size_type sumFreeBlock = 0;
        size_type unitBlock = 32;
        for (size_type i = 0; i < BLOCK_VARIANT; ++i)
        {
            SBlockLink* free = _pool.freeList[i];
            size_type num = 0;
            while (nullptr != free)
            {
                // DDEBUG(">> free pointer[%zd]: %p", i, free);
                free = free->next;
                ++num;
            }
            _pool.report.freeBlock[i] = num;
            sumFreeBlock += num * unitBlock;
            unitBlock *= 2;
        }

        // check space blance
        size_type usedSpace = (_pool.report.allocateSpace - _pool.report.deallocateSpace);
        _pool.report.blance = (_pool.report.freeSpace + sumFreeBlock + usedSpace) - _pool.report.total;
        if (_pool.report.blance != 0)
        {
            DDEBUG("Error!! pool memory not balanced at current time");
        }
    }
};

template<bool thread>
typename string_allocator<thread>::SPoolData string_allocator<thread>::_pool;

template<bool thread>
inline
bool operator==(const string_allocator<thread>& lhs, const string_allocator<thread>& rhs)
{
    return true;
}

typedef string_allocator<> tinypool;

#endif /* end of include guard: STRING_ALLOCATOR_HPP__ */
