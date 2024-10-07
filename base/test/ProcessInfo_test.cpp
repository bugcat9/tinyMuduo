#include "base/ProcessInfo.h"
#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

int main()
{
    printf("pid = %d\n", tinyMuduo::ProcessInfo::pid());
    printf("uid = %d\n", tinyMuduo::ProcessInfo::uid());
    printf("euid = %d\n", tinyMuduo::ProcessInfo::euid());
    printf("start time = %s\n", tinyMuduo::ProcessInfo::startTime().toFormattedString().c_str());
    printf("hostname = %s\n", tinyMuduo::ProcessInfo::hostname().c_str());
    printf("opened files = %d\n", tinyMuduo::ProcessInfo::openedFiles());
    printf("threads = %zd\n", tinyMuduo::ProcessInfo::threads().size());
    printf("num threads = %d\n", tinyMuduo::ProcessInfo::numThreads());
    printf("status = %s\n", tinyMuduo::ProcessInfo::procStatus().c_str());
}
