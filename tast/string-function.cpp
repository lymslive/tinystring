#include "tast_util.hpp"
#include "../src/string_function.hpp"
#include "../src/tinystring.hpp"
#include <iostream>
#include <string>

void tast_rawstr_std()
{
    const char* pcstr = "ABCDEFG";
    char carray[] = "abcdefg";
    char* pszstr = carray;
    COUT(pcstr);
    COUT(pszstr);
    COUT(carray);
    COUT((void*)pcstr);
    COUT((void*)pszstr);

    //! std::begin not support raw char*
    //! auto itb = std::begin(pcstr);
    //! auto ite = std::end(pcstr);
    //
    char* itBegin = pszstr;
    char* itEnd = pszstr + 7;
    std::reverse_iterator<char*> ritBegin(itEnd);
    std::string rs(ritBegin, ritBegin + 7);
    std::cout << rs << std::endl;
    COUT(itEnd - itBegin, 7);
    //! reverse_iterator works, but it cannot simply cast to to char*
    //! COUT(ritBegin - itEnd);
    //! COUT((char*)ritBegin - itEnd);
}

template <typename strT>
void tast_basic_by_type(const strT& typeInfo)
{
    COUT(typeInfo);

    strT str("ABCDEFG");
    strT strLow("abcdefg");
    strT strEmpty = "";
    COUT(str);
    COUT(strLow);

    DESC("basic string properties");
    // COUT(str.data() == str.c_str());
    // COUT(str.data() == utd::str::data(str), true);
    // COUT(str.c_str() == utd::str::c_str(str), true);
    // COUT(str.data() == utd::str::buffer(str), true);
    COUT(utd::str::size(str), 7);
    COUT(utd::str::length(str), 7);
    // COUT(str.capacity());
    // COUT(str.capacity(), utd::str::capacity(str));
    // COUT(str.max_size(), utd::str::max_size(str));
    COUT(utd::str::empty(str), false);
    COUT(utd::str::empty(strEmpty), true);

    utd::str::output(std::cout, str);
    std::cout << std::endl;

    DESC("index access string");
    COUT(utd::str::head(str), 'A');
    COUT(utd::str::tail(str), 'G');
    COUT(utd::str::at(str, 0), 'A');
    COUT(utd::str::at(str, 1), 'B');
    COUT(utd::str::at(str, -1), 'G');
    COUT(utd::str::at(str, -2), 'F');
    COUT(utd::str::at(str, 3), 'D');
    COUT(utd::str::at(str, -4), 'D');

    DESC("find in string");
    COUT(utd::str::find(str, 'D'), 3);
    COUT(utd::str::find(str, 'd'), size_t(-1));
    COUT(utd::str::has(str, 'D'), true);
    COUT(utd::str::has(str, 'd'), false);

    // COUT(utd::str::findin(str, "Dd"), 3);
    // COUT(utd::str::findin(str, "Hh"), size_t(-1));
    // COUT(utd::str::hasin(str, "Dd"), true);
    // COUT(utd::str::hasin(str, "Hh"), false);
    COUT(utd::str::findin(str, strT("Dd")), 3);
    COUT(utd::str::findin(str, strT("Hh")), size_t(-1));
    COUT(utd::str::hasin(str, strT("Dd")), true);
    COUT(utd::str::hasin(str, strT("Hh")), false);

    COUT(utd::str::has_prefix(str, strEmpty), true);
    COUT(utd::str::has_prefix(str, str), true);
    COUT(utd::str::has_prefix(str, strT("ABC")), true);
    COUT(utd::str::has_prefix(str, strT("abc")), false);
    COUT(utd::str::has_suffix(str, strEmpty), true);
    COUT(utd::str::has_suffix(str, str), true);
    COUT(utd::str::has_suffix(str, strT("ABC")), false);
    COUT(utd::str::has_suffix(str, strT("EFG")), true);

    DESC("compare string");
    strT strCopy = str;
    COUT(utd::str::equal(str, strCopy), true);
    COUT(utd::str::less(str, strLow), true);
    COUT(utd::str::greater(str, strLow), false);

    DESC("sub part of string");
    // return tstring(tuple) but not guarantee null-terminated
    COUT(utd::str::head(str, 3)); // ABC, 
    COUT(utd::str::tail(str, 3));
    COUT(utd::str::left(str, 3));
    COUT(utd::str::right(str, 3));
    COUT(utd::str::mid(str, 2, 3));
    COUT(utd::str::substr(str, 2, 3));
    COUT(utd::str::equal(utd::str::left(str, 3), "ABC"), true);
    COUT(utd::str::equal(utd::str::right(str, 3), "EFG"), true);
    COUT(utd::str::equal(utd::str::mid(str, 2, 3), "CDE"), true);

    DESC("split string by single char");
    strT strForSplit(",abc,dfeg;,hij.jkl,mn,");
    COUT(strForSplit);
    std::vector<utd::tstring> vecSplit = utd::str::split(strForSplit, ',');
    COUT(vecSplit.size(), 4);
    for (auto& sp : vecSplit)
    {
        COUT(sp);
    }
    vecSplit = utd::str::split(strForSplit, ',', true);
    COUT(vecSplit.size(), 6);
    for (auto& sp : vecSplit)
    {
        COUT(sp);
    }
    vecSplit = utd::str::split(strForSplit, ",;", true);
    COUT(vecSplit.size(), 7);
    for (auto& sp : vecSplit)
    {
        COUT(sp);
    }

    return;
}

template <typename strT, typename strU>
void tast_mixed_string_type(const strT& strType1, const strU& strType2)
{
    COUT(strType1);
    COUT(strType2);

    strT str1("ABCDEFG");
    strU str2("CDE");
    COUT(str1);
    COUT(str2);
    COUT(utd::str::find(str1, str2), 2);
    COUT(utd::str::has(str1, str2), true);
    COUT(utd::str::has_prefix(str1, str2), false);
    COUT(utd::str::has_prefix(str1, strU("ABC")), true);
    COUT(utd::str::has_suffix(str1, str2), false);
    COUT(utd::str::has_suffix(str1, strU("FG")), true);
    COUT(utd::str::equal(str1, str2), false);
    COUT(utd::str::less(str1, str2), true);
}

template <typename strT>
void tast_modify_inplace(const strT& typeInfo, strT& str)
{
    COUT(typeInfo);
    // strT str("abcdefg");
    COUT(str);
    utd::str::upper(str);
    COUT(str);
    utd::str::lower(str);
    COUT(str);
    COUT(utd::str::equal(str, "abcdefg"), true);

    utd::str::replace(str, 'd');
    COUT(str);
    COUT(utd::str::equal(str, "abc efg"), true);
    utd::str::replace(str, 'b', 'B', "f", "F");
    COUT(str);
    COUT(utd::str::equal(str, "aBc eFg"), true);

    utd::str::reverse(str);
    COUT(str);
    COUT(utd::str::equal(str, "gFe cBa"), true);
}

void tast_with_stdstring()
{
    tast_basic_by_type(std::string("std::string"));
}

void tast_with_rawstring()
{
    //! will pass as strT type char[32], and fail to compile
    //! tast_basic_by_type("const char* raw string");
    const char* raw = "const char* raw string";
    tast_basic_by_type(raw);
}

void tast_with_tinystring()
{
    tast_basic_by_type(utd::str32_t("utd::str32_t"));
}

void tast_with_unitstring()
{
    tast_basic_by_type(utd::ustring("utd::ustring"));
}

void tast_mix_std_ustring()
{
    tast_mixed_string_type(std::string("std::string"), utd::ustring("utd::ustring"));
}

void tast_mix_tiny_ustring()
{
    tast_mixed_string_type(utd::str32_t("utd::str32_t"), utd::ustring("utd::ustring"));
}

void tast_modify_inplace_stdstring()
{
    std::string buffer = "abcdefg";
    tast_modify_inplace(std::string("std::string"), buffer);
}

#if 0
//! fail to compile
void tast_modify_inplace_rawstring()
{
    char charray[] = "abcdefg";
    const char* raw = "const char* raw string";
    char* buffer = charray;
    tast_modify_inplace(raw, buffer);
}
#endif

void tast_modify_inplace_tinystring()
{
    utd::str32_t buffer = "abcdefg";
    tast_modify_inplace(utd::str32_t("utd::str32_t"), buffer);
}

void tast_modify_inplace_unitstring()
{
    utd::ustring buffer = "abcdefg";
    tast_modify_inplace(utd::ustring("utd::ustring"), buffer);
}

int main(int argc, char* argv[])
{
    // the pre-tast cout address, cannot diff
    // TAST(tast_rawstr_std);

    TAST(tast_with_stdstring);
    TAST(tast_with_rawstring);
    TAST(tast_with_tinystring);
    TAST(tast_with_unitstring);

    TAST(tast_mix_std_ustring);
    TAST(tast_mix_tiny_ustring);

    TAST(tast_modify_inplace_stdstring);
    // TAST(tast_modify_inplace_rawstring);
    TAST(tast_modify_inplace_tinystring);
    TAST(tast_modify_inplace_unitstring);

    return TAST_RESULT;
}
