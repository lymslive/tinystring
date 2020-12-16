# tinystring: С�ַ����Ż�ʵ��

һ�� C++ ���ַ���ʵ�֣�רע�Ż�С�ַ����Ŀռ������ʡ���Ҫ���ԣ�

* ���� 255 �����µ��ַ���������һ���ֽ���ĩβ��ʾ���ȡ�
* Ҳ����Ȼ��չ����ʾ������ 256 ���ϵ��ַ�����
* ���������֧�ֳ��� 0xFFFFFFFF ���ַ�����
* ��֧�����ü���������Ҫ�밴���ô��λ����� `share_ptr` ��װ����
* ֻ֧�� char �ַ��͵��ַ�������֧�ֿ��ַ�����֧�� utf-8 ����� unicode. 

## ʹ�ø���

�ɵ��� head-only ��ʹ�ã�include `tinystring/tinystring.hpp` ���ļ���
�ṩ���ַ��������� `utd` �����ռ��¡�

### �����ַ���

ģ���� `basic_tiny_string<int SIZE>`������ SIZE <= 256 ����ʾռ�ÿռ䣬������
�ַ����Ϊ `SIZE - 1` ����Ϊ���뻹Ҫ�� `\0` ���ַ���β��

�ṩ����������������ռ�Ķ����ַ������ͣ�

```cpp
typedef basic_tiny_string<16> str16_t;
typedef basic_tiny_string<32> str32_t;
typedef basic_tiny_string<64> str64_t;
typedef basic_tiny_string<128> str128_t;
typedef basic_tiny_string<256> str256_t;
```

�����ַ�������ȫ������ջ�ռ��ϣ�Ҳ���ֶ��� `new` �������ڶѿռ��ϴ�����ʹ����
ָ�롣

### ͳһ�������ַ���

ģ���� `united_tiny_string<Allocator>` ��ʾ����ͨ���ַ����࣬һ�������Ĭ�ϵ�
`utd::ustring` ���͡�

```cpp
typedef united_tiny_string<> ustring;
```

`utd::ustring` ��ʼ״̬�� `utd::str16_t` ���ڴ沼����ȫһ���������ɳ��Ƚ� 15
���µġ�΢�ַ����������״��������� 32 �����൱���ڶѿռ䴴�� `str32_t` ����
��ԭ����ջ�ϵ� `ustring` ������ָ�롣ֱ�������� 256 ���ϲ��붨��С�ַ����಻��
�ݡ�

`utd::ustring` ֧�ִ󲿷����Ʊ�׼ `std::string` �ĳ��ú����������

### С�ַ����ڴ��

���ڴ�ؼ���ʵ�������׵� allocator ��

`utd::tinypool` ��ҪΪ `str32_t` �� `str256_t` �����ֹ����ַ����ṩ�ڴ�ع���
`str16_t` ̫С��Ϊ�ʺ�ֱ����ջ�ϴ�������ֵ���ơ�ͬʱҲΪ������ 256 ���µ�
`ustrig` �ṩ������Ĭ�ϵ� allocator ģ������� `tinypool` ��

�ڴ�� `tinypool` ���ַ��� `ustring` ���໥�����ġ�`tinypool` Ҳ�ɷ�������������
ֻҪĿ�����Ĵ�СҲ�� 32/64/128/256 �����ͱȽϺ��ʡ��� `ustring` Ҳ��ʹ������
allocator �� `std::allocator` ��

## �������

�� [tast](./tast/) ��Ŀ¼��

## ������־

[devnote](./devnote.md)
