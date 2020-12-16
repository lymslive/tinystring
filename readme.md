# tinystring: 小字符串优化实现

一种 C++ 的字符串实现，专注优化小字符串的空间利用率。主要特性：

* 长度 255 及以下的字符串，复用一个字节在末尾表示长度。
* 也可自然扩展至表示长度在 256 以上的字符串。
* 理论上最大支持长度 0xFFFFFFFF 的字符串。
* 不支持引用计数（如需要请按引用传参或利用 `share_ptr` 封装）。
* 只支持 char 字符型的字符串，不支持宽字符，但支持 utf-8 编码的 unicode. 

## 使用概述

可当作 head-only 库使用，include `tinystring/tinystring.hpp` 单文件。
提供的字符串类型在 `utd` 命名空间下。

### 定容字符串

模板类 `basic_tiny_string<int SIZE>`，其中 SIZE <= 256 ，表示占用空间，可容纳
字符串最长为 `SIZE - 1` ，因为必须还要以 `\0` 空字符结尾。

提供如下五个“整数”空间的定容字符串类型：

```cpp
typedef basic_tiny_string<16> str16_t;
typedef basic_tiny_string<32> str32_t;
typedef basic_tiny_string<64> str64_t;
typedef basic_tiny_string<128> str128_t;
typedef basic_tiny_string<256> str256_t;
```

定容字符串可完全创建在栈空间上，也可手动用 `new` 操作符在堆空间上创建而使用其
指针。

### 统一可扩容字符串

模板类 `united_tiny_string<Allocator>` 表示常规通用字符串类，一般可用其默认的
`utd::ustring` 类型。

```cpp
typedef united_tiny_string<> ustring;
```

`utd::ustring` 初始状态与 `utd::str16_t` 的内存布局完全一样，可容纳长度仅 15
以下的“微字符串”。若首次需扩容至 32 ，则相当于在堆空间创建 `str32_t` 对象，
而原来在栈上的 `ustring` 保存其指针。直至扩容至 256 以上才与定容小字符串类不兼
容。

`utd::ustring` 支持大部分类似标准 `std::string` 的常用函数与操作。

### 小字符串内存池

用内存池技术实现了配套的 allocator 。

`utd::tinypool` 主要为 `str32_t` 至 `str256_t` 这四种规格的字符串提供内存池管理。
`str16_t` 太小认为适合直接在栈上创建并按值复制。同时也为长度在 256 以下的
`ustrig` 提供服务，其默认的 allocator 模板参数即 `tinypool` 。

内存池 `tinypool` 与字符串 `ustring` 是相互独立的。`tinypool` 也可服务于其他对象，
只要目标对象的大小也在 32/64/128/256 附近就比较合适。而 `ustring` 也可使用其他
allocator 如 `std::allocator` 。

## 体验测试

见 [tast](./tast/) 子目录。

## 开发日志

[devnote](./devnote.md)
