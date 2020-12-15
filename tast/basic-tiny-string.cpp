#include "../tinystring/tinystring.hpp"
#include <iostream>
#include <cstdio>
#include "tast_util.hpp"

using namespace utd;

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

/* ******************************* */

#define UPCASE_LETTERS "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define LOWCASE_LETTERS "abcdefghijklmnopqrstuvwxyz"
#define DIGITS_DEC "0123456789"
#define DIGITS_HEX "0123456789ABCDEF"

void tast_basic_tiny()
{
    str16_t str16;
    str32_t str32;
    str64_t str64;
    str128_t str128;
    str256_t str256;

    DESC("sizeof typical tiny string");
    COUT(sizeof(str16), 16);
    COUT(sizeof(str32), 32);
    COUT(sizeof(str64), 64);
    COUT(sizeof(str128), 128);
    COUT(sizeof(str256), 256);

    DESC("and capaicty, should = sizeof - 1");
    COUT(str16.capacity(), 15);
    COUT(str32.capacity(), 31);
    COUT(str64.capacity(), 63);
    COUT(str128.capacity(), 127);
    COUT(str256.capacity(), 255);

    DESC("default empy zero length string");
    COUT(str16.size(), 0);
    COUT(str32.size(), 0);
    COUT(str64.size(), 0);
    COUT(str128.size(), 0);
    COUT(str256.size(), 0);

    DESC("normally grow string with append() method");
    str16.append("1358042**98");
    str16.cout();
    COUT(str16.capacity(), 15);
    COUT(str16.size(), 11);
    DUMP_CHAR(str16);

    DESC("str16 can store dec digits 0-9");
    str16 = DIGITS_DEC;
    str16.cout();
    COUT(str16.capacity(), 15);
    COUT(str16.size(), 10);
    DUMP_CHAR(str16);

    DESC(".. but cannot store hex digits, as must end with '\\0'");
    str16 = DIGITS_HEX;
    str16.cout();
    COUT(str16.capacity(), 15);
    COUT(str16.size(), 15);
    DUMP_CHAR(str16);

    DESC("str32 is enough to store all a-z 26 letters");
    for (int i = 0; i < 26; ++i)
    {
        str32.append('a' + i);
    }
    str32.cout();
    COUT(str32.size(), 26);
    DUMP_CHAR(str32);

    DESC("tiny string is fixed capacity, guard not overflow");
    for (int i = 0; i < 26; ++i)
    {
        str32.append('A' + i);
    }
    str32.cout();
    COUT(str32.size(), 31);
    DUMP_CHAR(str32);
}

void tast_operator()
{
    DESC("new tiny string in heap");
    str32_t* p32 = new str32_t("1234");

    COUT(p32->capacity(), 31);
    COUT(p32->size(), 4);
    COUT(p32->length(), 4);
    p32->cout();

    DESC("push_back(c)");
    p32->push_back('$');
    COUT(p32->size(), 5);
    p32->cout();

    DESC("operator += char or string, based append");
    (*p32) += '$';
    (*p32) += "4321";
    COUT(p32->size(), 10);
    p32->cout();

    DESC("operator << chains");
    (*p32) << '#' << '@' << "abcd";
    COUT(p32->size(), 16);
    p32->cout();

    DESC("define tiny string in stack");
    str32_t s32 = "abcd";
    COUT(s32.size(), 4);
    s32.cout();

    DESC("default operator= for tiny string, memory copy");
    s32 = (*p32);
    COUT(s32.size(), 16);
    s32.cout();
    COUT(s32 == *p32, true);

    p32->clear();
    COUT(p32->size(), 0);
    COUT(p32->empty(), true);
    delete p32;
}

void tast_ustring_conversion()
{
    ustring str;

    DESC("convert ustring to basic_tiny_string of different size");
    COUT(str.capacity(), 15);
    str16_t* p16 = str.str16();
    COUT(p16 != nullptr, true);

    str.enlarge();
    COUT(str.capacity(), 31);
    str32_t* p32 = str.str32();
    COUT(p32 != nullptr, true);

    str.enlarge();
    COUT(str.capacity(), 63);
    str64_t* p64 = str.str64();
    COUT(p64 != nullptr, true);

    str.enlarge();
    COUT(str.capacity(), 127);
    str128_t* p128 = str.str128();
    COUT(p128 != nullptr, true);

    str.enlarge();
    COUT(str.capacity(), 255);
    str256_t* p256 = str.str256();
    COUT(p256 != nullptr, true);

    str.enlarge();
    COUT(str.capacity());
    str256_t* p512 = str.str256();
    COUT(p512 != nullptr, false);
    COUT(str.small_string(), false);
    COUT(str.middle_string(), true);
    COUT(str.size());

    DESC("convert basic_tiny_string to usting with operator=");
    str32_t s32 = LOWCASE_LETTERS;
    str = s32;
    p32 = str.str32();
    COUT(p32 != nullptr, true);

    DESC("convert basic_tiny_string to usting with constructor");
    ustring str2(s32);
    p32 = str2.str32();
    COUT(p32 != nullptr, true);
}

void tast_ustring_autolarge()
{
    ustring str;
    COUT(str.capacity());
    COUT(str.size());

    for (int i = 0; i < 5; ++i)
    {
        std::cout << "push a-z times: " << i << std::endl;
        str << LOWCASE_LETTERS;
        COUT(str.capacity());
        COUT(str.size(), 26*(2*i+1));
        str.cout();
        // DumpStr(str.c_str(), str.capacity() + 1);

        str << UPCASE_LETTERS;
        COUT(str.capacity());
        COUT(str.size(), 26*(2*i+2));
        str.cout();
        // DumpStr(str.c_str(), str.capacity() + 1);
    }
}

void tast_ustring_autolarge_appendc()
{
    ustring str;
    COUT(str.capacity());
    COUT(str.size());

    str32_t upLetter = UPCASE_LETTERS;
    str32_t lowLetter = LOWCASE_LETTERS;

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
        COUT(str.size(), 26*2*(i+1));
        str.cout();
    }
}

void tast_ustring_compare()
{
    DESC("prepare lowStr and upStr:");
    ustring lowStr = LOWCASE_LETTERS;
    ustring  upStr =  UPCASE_LETTERS;
    lowStr.cout();
    upStr.cout();

    DESC("compre them with operators:");
    COUT(lowStr == upStr, false);
    COUT(lowStr != upStr, true);
    COUT(lowStr <= upStr, false);
    COUT(lowStr >= upStr, true);
    COUT(lowStr < upStr, false);
    COUT(lowStr > upStr, true);

    DESC("make a copy of lowStr");
    ustring lowCopy(lowStr);
    COUT(lowStr == lowCopy, true);
    COUT(lowStr != lowCopy, false);

    DESC("make double copy of lowStr");
    ustring doubleCopy = lowStr; // also copy construtor, not operator=
    doubleCopy << lowStr;
    doubleCopy.cout();
    COUT(lowStr < doubleCopy, true);
    COUT(lowStr > doubleCopy, false);

    DESC("move operator:");
    const char* preMove = upStr.c_str();
    ustring upMoveTo = std::move(upStr);
    COUT(upStr == upMoveTo, false);
    COUT(upStr != upMoveTo, true);
    COUT(upStr.empty(), true);
    const char* postMove = upMoveTo.c_str();
    COUT(preMove == postMove, true);
}

int main(int argc, char* argv[])
{
    TAST(tast_basic_tiny);
    TAST(tast_operator);
    TAST(tast_ustring_conversion);
    TAST(tast_ustring_autolarge);
    TAST(tast_ustring_autolarge_appendc);
    TAST(tast_ustring_compare);

    return TAST_RESULT;
}

// bash: g++ -g -o basic-tiny-string.out basic-tiny-string.cpp
// diff tast.cout tast.cout2 --strip-trailing-cr
// compare the output with diff, may ignore newline format
