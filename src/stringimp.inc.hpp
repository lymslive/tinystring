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

void cout(std::ostream& os, bool endl = true) const
{
    os << c_str();
    if (endl)
    {
        os << std::endl;
    }
}

void cout(FILE* file, bool endl = true) const
{
    fprintf(file, "%s", c_str());
    if (endl)
    {
        fprintf(file, "\n");
    }
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

self_type& append(const self_type& that)
{
    append(that.c_str(), that.size());
    return *this;
}

self_type& operator +=(char c)
{
    append(c);
    return *this;
}

self_type& operator +=(const char* str)
{
    append(str);
    return *this;
}

self_type& operator +=(const self_type& that)
{
    return append(that);
}

self_type& operator <<(char c)
{
    append(c);
    return *this;
}

self_type& operator <<(const char* str)
{
    append(str);
    return *this;
}

self_type& operator <<(const self_type& that)
{
    return append(that);
}

self_type& operator =(const char* str)
{
    clear();
    append(str);
    return *this;
}

self_type& operator =(const self_type& that)
{
    if (this != &that)
    {
        clear();
        append(that);
    }
    return *this;
}

void push_back(char c)
{
    append(c);
}

int compare(const self_type& that) const
{
    return strcmp(this->c_str(), that.c_str());
}

bool operator ==(const self_type& that) const
{
    return compare(that) == 0;
}

bool operator !=(const self_type& that) const
{
    return compare(that) != 0;
}

bool operator <=(const self_type& that) const
{
    return compare(that) <= 0;
}

bool operator >=(const self_type& that) const
{
    return compare(that) >= 0;
}

bool operator <(const self_type& that) const
{
    return compare(that) < 0;
}

bool operator >(const self_type& that) const
{
    return compare(that) > 0;
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

void _append_size(size_t len)
{
    if (len == capacity())
    {
        _write_size(0);
    }
    else
    {
        _write_size(len);
    }
}

//};
