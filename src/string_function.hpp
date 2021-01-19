#ifndef FUNCTION_HPP__
#define FUNCTION_HPP__

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <cctype>
#include <vector>
#include <iterator>

namespace utd
{
    typedef std::tuple<const char*, size_t> tstring;
}

inline
std::ostream& operator<<(std::ostream& os, const utd::tstring& str)
{
    os.write(std::get<0>(str), std::get<1>(str));
    return os;
}

namespace utd { namespace str {

typedef char  value_type;
typedef char* iterator;
typedef char* pointer;
typedef char& reference;
typedef const char* const_iterator;
typedef const char* const_pointer;
typedef const char& const_reference;
typedef std::reverse_iterator<iterator> reverse_iterator;
typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

template <typename strT>
struct trait
{
    // string is end with zero null characher
    static const bool zend = true;
    // string size can grow, support append += etc
    static const bool grow = true;
    // string can alloc space, capacity can grow
    static const bool alloc = true;
};

// special for raw const char* string and tstring tuple
inline
const char* c_str(const char* self)
{
    return self;
}

inline
size_t size(const char* self)
{
    return std::strlen(self);
}

inline
const char* c_str(const tstring& self)
{
    return std::get<0>(self);
}

inline
size_t size(const tstring& self)
{
    return std::get<1>(self);
}

// read-only query functions
// data() return pointer, c_str() return const pointer
template <typename strT>
const char* c_str(const strT& self)
{
    return self.c_str();
}

template <typename strT>
char* data(strT& self)
{
    return const_cast<char*>(c_str(self));
}

template <typename strT>
char* buffer(strT& self)
{
    return data(self);
}

template <typename strT>
size_t size(const strT& self)
{
    return self.size();
}

template <typename strT>
size_t length(const strT& self)
{
    return size(self);
}

template <typename strT>
size_t capacity(const strT& self)
{
    return self.capacity();
}

template <typename strT>
size_t max_size(const strT& self)
{
    return self.max_size();
}

template <typename strT>
bool empty(const strT& self)
{
    return size(self) == 0;
}

// iterators

template <typename strT>
const_iterator cbegin(const strT& self)
{
    return c_str(self);
}

template <typename strT>
const_iterator cend(const strT& self)
{
    return c_str(self) + size(self);
}

template <typename strT>
iterator begin(strT& self)
{
    return const_cast<iterator>(utd::str::cbegin(self));
}

template <typename strT>
iterator end(strT& self)
{
    return const_cast<iterator>(utd::str::cend(self));
}

template <typename strT>
reverse_iterator rbegin(strT& self)
{
    return std::reverse_iterator<iterator>(utd::str::end(self));
}

template <typename strT>
reverse_iterator rend(strT& self)
{
    return std::reverse_iterator<iterator>(utd::str::begin(self));
}

template <typename strT>
const_reverse_iterator crbegin(const strT& self)
{
    return std::reverse_iterator<const_iterator>(utd::str::cend(self));
}

template <typename strT>
const_reverse_iterator crend(const strT& self)
{
    return std::reverse_iterator<const_iterator>(utd::str::cbegin(self));
}

// output
template <typename strT>
std::ostream& output(std::ostream& os, const strT& str)
{
    for (auto it = utd::str::cbegin(str); it != utd::str::cend(str); ++it)
    {
        os << *it;
    }
    return os;
}

template <typename strT>
char head(const strT& self)
{
    auto it = utd::str::cbegin(self);
    return *it;
}

template <typename strT>
char tail(const strT& self)
{
    auto it = utd::str::cend(self);
    return *(--it);
}

template <typename strT>
char at(const strT& self, size_t n)
{
    if (n >= size(self))
    {
        throw std::out_of_range("utd::str::at require index beyond string");
    }
    auto it = utd::str::cbegin(self);
    return *(it + n);
}

// support -1 index
template <typename strT>
char at(const strT& self, int n)
{
    if (n >= 0)
    {
        return at(self, size_t(n));
    }
    else
    {
        return at(self, size_t(n + size(self)));
    }
}

template <typename strT>
size_t find(const strT& self, char c)
{
    auto it = utd::str::cbegin(self);
    auto itend = utd::str::cend(self);
    for (size_t i = 0; it != itend; ++i, ++it)
    {
        if (*it == c)
        {
            return i;
        }
    }
    return size_t(-1);
}

template <typename strT, typename strU>
size_t find(const strT& self, const strU& that)
{
    size_t nThis = size(self);
    size_t nThat = size(that);
    if (nThis < nThat)
    {
        return size_t(-1);
    }
    for (size_t pos = 0; pos <= nThis - nThat; ++pos)
    {
        auto it = utd::str::cbegin(self);
        it += pos;
        auto itThat = utd::str::cbegin(that), itEnd = utd::str::cend(that);
        for (; itThat != itEnd; ++itThat, ++it)
        {
            if (*it != *itThat)
            {
                break;
            }
        }
        if (itThat == itEnd)
        {
            return pos;
        }
    }
    return size_t(-1);
}

template <typename strT, typename strU>
bool has(const strT& self, const strU& that)
{
    return find(self, that) != size_t(-1);
}

// find any character in that
// some like std::string.find_any_of()
template <typename strT, typename strU>
size_t findin(const strT& self, const strU& that)
{
    auto it = utd::str::cbegin(self);
    size_t nSelf = size(self);
    for (size_t pos = 0; pos < nSelf; ++pos, ++it)
    {
        for (auto itThat = utd::str::cbegin(that), itEnd = utd::str::cend(that);
                itThat != itEnd; ++itThat)
        {
            if (*it == *itThat)
            {
                return pos;
            }
        }
    }
    return size_t(-1);
}

template <typename strT, typename strU>
bool hasin(const strT& self, const strU& that)
{
    return findin(self, that) != size_t(-1);
}

template <typename strT, typename strU>
bool has_prefix(const strT& self, const strU& that)
{
    return find(self, that) == 0;
}

template <typename strT, typename strU>
bool has_suffix(const strT& self, const strU& that)
{
    if (empty(that)) return true;
    return size(self) >= size(that) &&
        find(self, that) == size(self) - size(that);
}

template <typename strT, typename strU>
bool equal(const strT& self, const strU& that)
{
    size_t nThis = size(self);
    size_t nThat = size(that);
    if (nThis != nThat)
    {
        return false;
    }
    for (auto it = utd::str::cbegin(self), itend = utd::str::cend(self), it2 = utd::str::cbegin(that);
            it != itend; ++it, ++it2)
    {
        if (*it != *it2)
        {
            return false;
        }
    }
    return true;
}

template <typename strT, typename strU>
bool less(const strT& self, const strU& that)
{
    auto it = utd::str::cbegin(self), itend = utd::str::cend(self),
         it2 = utd::str::cbegin(that), itend2 = utd::str::cend(that);
    for (; it != itend && it2 != itend2; ++it, ++it2)
    {
        if (*it < *it2)
        {
            return true;
        }
        else if (*it > *it2)
        {
            return false;
        }
    }
    return it == itend && it2 != itend2;
}

template <typename strT, typename strU>
bool greater(const strT& self, const strU& that)
{
    return less(that, self);
}

// extract substring, return tstring, a tuple containing char* pointer and size
template <typename strT>
tstring head(const strT& self, size_t len)
{
    if (len > size(self))
    {
        throw std::out_of_range("utd::str::head require index beyond string");
    }
    return std::make_tuple(const_cast<const char*>(data(self)), len);
}

template <typename strT>
tstring tail(const strT& self, size_t len)
{
    if (len > size(self))
    {
        throw std::out_of_range("utd::str::tail require index beyond string");
    }
    return std::make_tuple(const_cast<const char*>(data(self)) + size(self) - len, len);
}

template <typename strT>
tstring left(const strT& self, size_t len)
{
    return head(self, len);
}

template <typename strT>
tstring right(const strT& self, size_t len)
{
    return tail(self, len);
}

template <typename strT>
tstring substr(const strT& self, size_t pos, size_t len)
{
    if (pos + len > size(self))
    {
        throw std::out_of_range("utd::str::substr require index beyond string");
    }
    return std::make_tuple(const_cast<const char*>(data(self) + pos), len);
}

template <typename strT>
tstring mid(const strT& self, size_t pos, size_t len)
{
    return substr(self, pos, len);
}

template <typename strT>
std::vector<tstring> split(const strT& self, const std::vector<size_t>& vecPos, bool keepEmpty)
{
    const char* buffer = utd::str::cbegin(self);
    const char* pSlice = nullptr;
    size_t nSlice = 0;
    size_t lastPos = size_t(-1);
    std::vector<tstring> result;
    for (size_t pos : vecPos)
    {
        if (pos == 0)
        {
            if (keepEmpty)
            {
                result.push_back(std::make_tuple(buffer, 0));
            }
        }
        else
        {
            ++lastPos;
            pSlice = buffer + lastPos;
            nSlice = pos - lastPos;
            if (nSlice > 0 || keepEmpty)
            {
                result.push_back(std::make_tuple(pSlice, nSlice));
            }
        }

        lastPos = pos;
    }

    size_t nSize = size(self);
    if (lastPos + 1 == nSize && keepEmpty)
    {
        result.push_back(std::make_tuple(buffer + nSize, 0));
    }

    return result;
}

template <typename strT>
std::vector<tstring> split(const strT& self, char c, bool keepEmpty = false)
{
    std::vector<size_t> vecPos;
    const char* ptr = utd::str::cbegin(self);
    size_t nSize = size(self);
    for (size_t i = 0; i < nSize; ++i, ++ptr)
    {
        if (*ptr == c)
        {
            vecPos.push_back(i);
        }
    }

    return split(self, vecPos, keepEmpty);
}

template <typename strT, typename strU>
std::vector<tstring> split(const strT& self, const strU& that, bool keepEmpty = false)
{
    std::vector<size_t> vecPos;
    const char* ptr = utd::str::cbegin(self);
    size_t nSize = size(self);
    for (size_t i = 0; i < nSize; ++i, ++ptr)
    {
        if (has(that, *ptr))
        {
            vecPos.push_back(i);
        }
    }

    return split(self, vecPos, keepEmpty);
}

// limited modifing string without change it size, in-palce modify.
#define STRING_MODIFY_INPLACE
#ifdef STRING_MODIFY_INPLACE
template <typename strT>
void upper(strT& self)
{
    for (auto it = utd::str::begin(self), itend = utd::str::end(self); it != itend; ++it)
    {
        *it = std::toupper(*it);
    }
}

template <typename strT>
void lower(strT& self)
{
    for (auto it = utd::str::begin(self), itend = utd::str::end(self); it != itend; ++it)
    {
        *it = std::tolower(*it);
    }
}

// replace all char of src to dest, default a space
template <typename strT>
size_t replace(strT& self, char src, char dest = ' ')
{
    size_t count = 0;
    for (auto it = utd::str::begin(self), itend = utd::str::end(self); it != itend; ++it)
    {
        if (*it == src)
        {
            *it = dest;
            ++count;
        }
    }
    return count;
}

// replace all char in src to dest, default a space
template <typename strT, typename strU>
size_t replace(strT& self, const strU& src, char dest = ' ')
{
    size_t count = 0;
    for (auto it = utd::str::begin(self), itend = utd::str::end(self); it != itend; ++it)
    {
        if (has(src, *it))
        {
            *it = dest;
            ++count;
        }
    }
    return count;
}

// replace the all occurence of substring src to string dest
// todo: optimize perfermance for repeated replace
template <typename strT, typename strU>
size_t replace(strT& self, const strU& src, const strU& dest)
{
    if (size(src) != size(dest))
    {
        return 0;
    }
    size_t count;
    size_t pos = find(self, src);
    if (pos == size_t(-1))
    {
        return 0;
    }
    auto it = utd::str::begin(self);
    it += pos;
    for (auto itd = utd::str::cbegin(dest), ite = utd::str::cend(dest); itd != ite; ++it, ++itd)
    {
        *it = *itd;
    }
    return size(dest) + replace(self, src, dest);
}

template <typename strT, typename strU, typename strV, typename... varT>
size_t replace(strT& self, const strU& src, const strV& dest, varT... varLeft)
{
    size_t count = replace(self, src, dest);
    count += replace(self, varLeft...);
    return count;
}

template <typename strT>
void reverse(strT& self)
{
    char tmp;
    auto first = utd::str::begin(self), last = utd::str::end(self);
    while ((first != last) && (first != --last))
    {
        tmp = *first;
        *first = *last;
        *last = tmp;
        ++first;
    }
}
#endif

#define STRING_MODIFY_REDUCE
#ifdef STRING_MODIFY_REDUCE
// limited modifing string without alloc, but may reduce size
// these functions return the reduced diff size, the count of removed char.

// erase the last (one or count) character
template <typename strT>
size_t chop(strT& self, size_t count = 1)
{
    if (count > size(self))
    {
        throw std::out_of_range("utd::str::chop require index beyond string");
    }
    auto it = utd::str::end(self);
    it -= count;
    *it = '\0';
    return count;
}

// erase the last newline character
template <typename strT>
size_t chomp(strT& self)
{
    size_t count;
    size_t nSize = size(self);
    auto it = utd::str::end(self);
    for (; nSize > 0; --nSize)
    {
        --it;
        if(*it == '\n' || *it == '\r')
        {
            *it = '\0';
            ++count;
        }
        else
        {
            break;
        }
    }
    return count;
}

// erase the last specific character
template <typename strT>
size_t chomp(strT& self, char c)
{
    size_t count;
    size_t nSize = size(self);
    auto it = utd::str::end(self);
    for (; nSize > 0; --nSize)
    {
        --it;
        if(*it == c)
        {
            *it = '\0';
            ++count;
        }
        else
        {
            break;
        }
    }
    return count;
}

// erase the last character in strU
template <typename strT, typename strU>
size_t chomp(strT& self, const strU& that)
{
    size_t count = 0;
    size_t nSize = size(self);
    auto it = utd::str::end(self);
    for (; nSize > 0; --nSize)
    {
        --it;
        if(has(that, *it))
        {
            *it = '\0';
            ++count;
        }
        else
        {
            break;
        }
    }
    return count;
}

// cutoff at pos, keep the right hand part, need move ahead
template <typename strT>
size_t shift(strT& self, size_t pos)
{
    size_t nSize = size(self);
    if (pos == 0 || nSize = 0)
    {
        return 0;
    }
    else if (pos == nSize)
    {
        *(utd::str::begin(self)) = '\0';
        return nSize;
    }
    else if (pos > nSize)
    {
        return 0;
        // throw std::out_of_range("utd::str::shift require index beyond string");
    }
    auto it = utd::str::begin(self), itend = utd::str::end(self);
    auto dest = it;
    it += pos;
    for (; it != itend && *it != '\0'; ++it, ++dest)
    {
        *dest = *it;
    }
    if (it != itend)
    {
        *dest = '\0';
    }
    return pos;
}

// cutoff at pos, keep the left hand part
template <typename strT>
size_t cutoff(strT& self, size_t pos)
{
    if (pos >= size(self))
    {
        return 0;
    }
    auto it = utd::str::begin(self);
    it += pos;
    *it = '\0';
    return size(self) - pos;
}

// trim space
template <typename strT>
size_t trim_tail(strT& self)
{
    size_t nSize = size(self);
    if (nSize == 0)
    {
        return 0;
    }
    size_t pos = nSize;
    auto it = utd::str::end(self);
    for (; pos > 0; --pos)
    {
        --it;
        if(!std::isspace(*it))
        {
            break;
        }
    }
    return cutoff(self, pos);
}

template <typename strT>
size_t trim_head(strT& self)
{
    size_t pos = 0;
    auto it = utd::str::begin(self), itend = utd::str::end(self);
    for (; it != itend; ++it, ++pos)
    {
        if(!std::isspace(*it))
        {
            break;
        }
    }
    return shift(self, pos);
}

template <typename strT>
size_t trim(strT& self)
{
    size_t count = trim_tail(self);
    count += trim_head(self);
    return count;
}

template <typename strT, typename strU>
size_t trim_tail(strT& self, const strU& that)
{
    if (has_suffix(self, that))
    {
        return cutoff(size(self) - size(that));
    }
}

template <typename strT, typename strU>
size_t trim_head(strT& self, const strU& that)
{
    if (has_prefix(self, that))
    {
        return shift(self, size(that));
    }
}

template <typename strT, typename strU>
size_t trim(strT& self, const strU& that)
{
    size_t count = trim_tail(self, that);
    count += trim_head(self, that);
    return count;
}

// trim any character existed in strU at both end
template <typename strT, typename strU>
size_t trim_any(strT& self, const strU& that)
{
    size_t count = chomp(self, that);
    size_t pos = 0;
    auto it = utd::str::begin(self), itend = utd::str::end(self);
    for (; it != itend > 0; ++it, ++pos)
    {
        if(!has(that, *it))
        {
            break;
        }
    }
    if (pos > 0)
    {
        count += shift(self, pos);
    }
    return count;
}
#endif

#undef STRING_MODIFY_INPLACE
#undef STRING_MODIFY_REDUCE
}}

#endif /* end of include guard: FUNCTION_HPP__ */
