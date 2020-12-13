#include "../tinystring/tinystring.hpp"
#include <iostream>
#include <cstdio>
#include "tast_util.hpp"

#define DUMP_CHAR(str) DumpChar(str.c_str(), sizeof(str))
void DumpChar(const char* str, size_t len)
{
    for (size_t i = 0; i < len; ++i)
    {
        printf("%X ", str[i]);
    }
    printf("\n");
}

// dump string in tight way, use \ escape after string ended by \0
void DumpStr(const char* str, size_t len)
{
    bool end = false;
    for (size_t i = 0; i < len; ++i)
    {
        if (str[i] == 0)
        {
            end = true;
        }
        if (end)
        {
            printf("\\%d", (unsigned char)str[i]);
        }
        else if (str[i] >= 32 && str[i] <= 127)
        {
            printf("%c", str[i]);
        }
        else
        {
            printf("\\%d", (unsigned char)str[i]);
        }
    }
    printf("\n");
}


void tast_basic_append()
{
    char_16t str16;
    char_32t str32;
    char_64t str64;
    char_128t str128;
    char_256t str256;

    COUT(sizeof(str16));
    COUT(sizeof(str32));
    COUT(sizeof(str64));
    COUT(sizeof(str128));
    COUT(sizeof(str256));

    COUT(str16.capacity());
    COUT(str32.capacity());
    COUT(str64.capacity());
    COUT(str128.capacity());
    COUT(str256.capacity());

    COUT(str16.size());
    COUT(str32.size());
    COUT(str64.size());
    COUT(str128.size());
    COUT(str256.size());

    str16.append("1358042**98");
    str16.cout();
    COUT(str16.capacity());
    COUT(str16.size());
    DUMP_CHAR(str16);

    for (int i = 0; i < 26; ++i)
    {
        str32.append('a' + i);
    }
    str32.cout();
    COUT(str32.size());
    DUMP_CHAR(str32);

    // will overflow?
    for (int i = 0; i < 26; ++i)
    {
        str32.append('A' + i);
    }
    str32.cout();
    COUT(str32.size());
    DUMP_CHAR(str32);
}

void tast_operator()
{
    char_32t* p32 = new char_32t("1234");

    COUT(p32->capacity());
    COUT(p32->size());
    COUT(p32->length());
    p32->cout();

    p32->push_back('$');
    COUT(p32->size());
    p32->cout();

    (*p32) += '$';
    (*p32) += "4321";
    COUT(p32->size());
    p32->cout();

    (*p32) << '#' << '@' << "abcd";
    COUT(p32->size());
    p32->cout();

    char_32t s32 = "abcd";
    COUT(s32.size());
    s32.cout();

    s32 = (*p32);
    COUT(s32.size());
    s32.cout();

    p32->clear();
    COUT(p32->size());
    COUT(p32->empty());
    delete p32;
}

void tast_ustring_conversion()
{
    ustring str;

    COUT(str.capacity());
    char_16t* p16 = str.str16();
    COUT(p16 != nullptr);

    str.enlarge();
    COUT(str.capacity());
    char_32t* p32 = str.str32();
    COUT(p32 != nullptr);

    str.enlarge();
    COUT(str.capacity());
    char_64t* p64 = str.str64();
    COUT(p64 != nullptr);

    str.enlarge();
    COUT(str.capacity());
    char_128t* p128 = str.str128();
    COUT(p128 != nullptr);

    str.enlarge();
    COUT(str.capacity());
    char_256t* p256 = str.str256();
    COUT(p256 != nullptr);

    str.enlarge();
    COUT(str.capacity());
    char_256t* p512 = str.str256();
    COUT(p512 != nullptr);
    COUT(str.small_string());
    COUT(str.middle_string());
    COUT(str.size());
}

void tast_ustring_autolarge()
{
    ustring str;
    COUT(str.capacity());
    COUT(str.size());

    for (int i = 0; i < 5; ++i)
    {
        std::cout << "push a-z times: " << i << std::endl;
        str << "abcdefghijklmnopqrstuvwxyz";
        COUT(str.capacity());
        COUT(str.size());
        str.cout();
        // DumpStr(str.c_str(), str.capacity() + 1);

        str << "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        COUT(str.capacity());
        COUT(str.size());
        str.cout();
        // DumpStr(str.c_str(), str.capacity() + 1);
    }
}

void tast_ustring_autolarge_appendc()
{
    ustring str;
    COUT(str.capacity());
    COUT(str.size());

    char_32t upLetter = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char_32t lowLetter = "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < upLetter.size(); ++j)
        {
            str << upLetter[j];
        }
        for (int j = 0; j < lowLetter.size(); ++j)
        {
            str << lowLetter[j];
        }
        COUT(str.capacity());
        COUT(str.size());
        str.cout();
    }
}

int main(int argc, char* argv[])
{
    TAST(tast_basic_append);
    TAST(tast_operator);
    TAST(tast_ustring_conversion);
    TAST(tast_ustring_autolarge);
    TAST(tast_ustring_autolarge_appendc);
}

// bash: g++ -g -o basic-tiny-string.out basic-tiny-string.cpp
// diff tast.cout tast.cout2 --strip-trailing-cr
// compare the output with diff, may ignore newline format
