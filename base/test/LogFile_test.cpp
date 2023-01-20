#include "../../base/LogFile.h"
#include "../../base/Logging.h"

#include <unistd.h>

std::unique_ptr<tinyMuduo::LogFile> g_logFile;

void outputFunc(const char *msg, int len)
{
    g_logFile->append(msg, len);
}

void flushFunc()
{
    g_logFile->flush();
}

int main(int argc, char *argv[])
{
    char name[256] = {'\0'};
    strncpy(name, argv[0], sizeof name - 1);
    g_logFile.reset(new tinyMuduo::LogFile(::basename(name), 200 * 1000));
    tinyMuduo::Logger::setOutput(outputFunc);
    tinyMuduo::Logger::setFlush(flushFunc);

    tinyMuduo::string line = "1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

    for (int i = 0; i < 10000; ++i)
    {
        LOG_INFO << line << i;

        usleep(1000);
    }
}
