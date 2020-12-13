#define _DEV_DEBUG
#include "../tinystring/string_allocator.hpp"
#include "../tinystring/tinystring.hpp"
#include "tast_util.hpp"
#include <iostream>

#define COUT_ADDRESS(ptr) printf("| %s => %p\n", #ptr, ptr)

void print_pool_report(const tinypool::SPoolStatistic& report)
{
    std::cout <<  "beyondAllocTimes = " << report.beyondAllocTimes << std::endl;
    std::cout <<  "beyondFreeTimes = " << report.beyondFreeTimes << std::endl;
    std::cout <<  "allocateTimes = " << report.allocateTimes << std::endl;
    std::cout <<  "allocateSpace = " << report.allocateSpace << std::endl;
    std::cout <<  "deallocateTimes = " << report.deallocateTimes << std::endl;
    std::cout <<  "deallocateSpace = " << report.deallocateSpace << std::endl;
    std::cout <<  "freeSpace = " << report.freeSpace << std::endl;
    std::cout <<  "freeBlock(32) = " << report.freeBlock[0] << std::endl;
    std::cout <<  "freeBlock(64) = " << report.freeBlock[1] << std::endl;
    std::cout <<  "freeBlock(128) = " << report.freeBlock[2] << std::endl;
    std::cout <<  "freeBlock(256) = " << report.freeBlock[3] << std::endl;
    std::cout <<  "totalTimes = " << report.totalTimes << std::endl;
    std::cout <<  "total = " << report.total << std::endl;
    std::cout <<  "blance = " << report.blance << std::endl;
    std::cout << std::endl;
}

void tast_pool_alone()
{
    tinypool allocator;
    tinypool other;

    DESC("alloctor property");
    COUT(sizeof(allocator));
    COUT(allocator == other);

    DESC("alloc 32");
    void* p32 = allocator.allocate(32);
    COUT(p32 != nullptr);
    // COUT_ADDRESS(p32);
    print_pool_report(allocator.get_pool_report());

    DESC("alloc 64");
    void* p64 = allocator.allocate(64);
    COUT(p64 != nullptr);
    // COUT_ADDRESS(p64);
    print_pool_report(allocator.get_pool_report());

    DESC("alloc 128");
    void* p128 = allocator.allocate(128);
    COUT(p128 != nullptr);
    print_pool_report(allocator.get_pool_report());

    DESC("alloc 256");
    void* p256 = allocator.allocate(256);
    COUT(p256 != nullptr);
    print_pool_report(allocator.get_pool_report());

    DESC("alloc 300: should not alloc in pool");
    void* p300 = allocator.allocate(300);
    COUT(p300 != nullptr);
    print_pool_report(allocator.get_pool_report());

    DESC("alloc 200: should actually alloc 256");
    void* p200 = allocator.allocate(200);
    COUT(p200 != nullptr);
    print_pool_report(allocator.get_pool_report());

    DESC("alloc 100: actually alloc 128");
    void* p100 = allocator.allocate(100);
    COUT(p100 != nullptr);
    print_pool_report(allocator.get_pool_report());

    DESC("alloc 50: actually alloc 64");
    void* p50 = allocator.allocate(50);
    COUT(p50 != nullptr);
    print_pool_report(allocator.get_pool_report());

    DESC("alloc 25: actually alloc 32");
    void* p25 = allocator.allocate(25);
    COUT(p25 != nullptr);
    print_pool_report(allocator.get_pool_report());

    DESC("alloc 12: actually alloc 32, min block in pool");
    void* p12 = allocator.allocate(12);
    COUT(p12 != nullptr);
    print_pool_report(allocator.get_pool_report());

    DESC("free 32 64 128 256 blocks");
    allocator.deallocate(p32, 32);
    allocator.deallocate(p64, 64);
    allocator.deallocate(p128, 128);
    allocator.deallocate(p256, 256);
    print_pool_report(allocator.get_pool_report());

    DESC("free 300: also not free by pool");
    allocator.deallocate(p300, 300);
    print_pool_report(allocator.get_pool_report());

    DESC("free 200: pass the actually size or not don't matter");
    allocator.deallocate(p200, 256);
    print_pool_report(allocator.get_pool_report());

    DESC("free 100");
    allocator.deallocate(p100, 100);
    print_pool_report(allocator.get_pool_report());

    DESC("free 50");
    allocator.deallocate(p50, 64);
    print_pool_report(allocator.get_pool_report());

    DESC("free 25");
    DESC("free 12");
    allocator.deallocate(p25, 32);
    allocator.deallocate(p12, 12);
    print_pool_report(allocator.get_pool_report());

}

void tast_alloc_lot()
{
    tinypool allocator;
    ustring ch13 = "abcdefghijklm";
    ustring pad4 = "****";
    for (int i = 0; i < 11; ++i)
    {
        ustring str;
        for (int j = 0; j < 23; ++j)
        {
            str << ch13.c_str();
            str << pad4.c_str();
        }
        DESC("loop i = %d", i);
        print_pool_report(allocator.get_pool_report());
    }
    DESC("out loop");
    print_pool_report(allocator.get_pool_report());
}

int main(int argc, char* argv[])
{
    TAST(tast_pool_alone);
    TAST(tast_alloc_lot);
}
