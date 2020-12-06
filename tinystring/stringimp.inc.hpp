// basic string operation based on c_str() capacity() and size() methods
// can copy(include) to concret string class
// cannot put in some base class to avoid define virtual method for them;

// class stringImp {
//  const char* c_str() { return 0; }
//  size_t capacity() { return 0; }
//  size_t size() { return 0; }

public:

char* data() const
{
    return const_cast<char*>(c_str());
}

char* begin() const
{
    return data();
}

char* end() const
{
    return data() + size();
}

const char* cbegin() const
{
    return c_str();
}

const char* cend() const
{
    return c_str() + size();
}

size_t length() const
{
    return size();
}

bool empty() const
{
    return size() == 0;
}

bool full() const
{
    return size() >= capacity();
}

void cout() const
{
    std::cout << c_str() << std::endl;
}

char& at(size_t pos)
{
    return *(data() + pos);
}

const char& at(size_t pos) const
{
    return *(c_str() + pos);
}

char& operator[](size_t pos)
{
    return at(pos);
}

const char& operator[](size_t pos) const
{
    return at(pos);
}

selfType& operator +=(char c)
{
    append(c);
    return *this;
}

selfType& operator +=(const char* str)
{
    append(str);
    return *this;
}

selfType& operator <<(char c)
{
    append(c);
    return *this;
}

selfType& operator <<(const char* str)
{
    append(str);
    return *this;
}

selfType& operator =(const char* str)
{
    clear();
    append(str);
    return *this;
}

void push_back(char c)
{
    append(c);
}

private:

bool _append_local(char c)
{
    if (size() >= capacity())
    {
        return false;
    }
    char* pEnd = end();
    *pEnd++ = c;
    *pEnd = '\0';
    return true;
}

size_t _append_local(const char* str)
{
    if (size() >= capacity())
    {
        return 0;
    }
    
    size_t free = capacity() - size();
    size_t copied = 0;
    char* pDest = end();
    const char* pSrc = str;
    for (; copied < free; ++copied)
    {
        if (*pSrc == '\0')
        {
            break;
        }
        *pDest++ = *pSrc++;
    }
    *pDest = '\0';

    return copied;
}

size_t _append_local(const char* str, size_t len)
{
    if (size() >= capacity())
    {
        return 0;
    }
    size_t free = capacity() - size();
    if (free < len)
    {
        len = free;
    }
    char* pDest = end();
    const char* pSrc = str;
    memcpy(pDest, pSrc, len);
    *(pDest+len) = '\0';

    return len;
}

//};
