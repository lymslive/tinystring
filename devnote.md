# 开发日志琐记

## branch 分支意向

* main(master) 主分支
* dev/basic 字符串功能
* dev/allocator 内存池分配器功能
* dev/tast 测试

## stringimp.inc.hpp 说明

考虑抽象字符串概念，只要提供 `c_str()` `size()` 与 `capacity()` 这三个方法，就
能实现大部分字符串常规操作了。但在 C++ 中又不想设计一个抽象字符串基类搞继承将
其作为虚函数重写。所以采用 include 大法实现代码复用，在不同的字符串类中包含
`stringimp.inc.hpp` 里面的公共代码。
