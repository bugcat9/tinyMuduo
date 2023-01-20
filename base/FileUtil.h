#ifndef _FILEUTIL_H
#define _FILEUTIL_H

#include "StringPiece.h"
#include <sys/types.h> // for off_t
#include <boost/noncopyable.hpp>
namespace tinyMuduo
{
    namespace FileUtil
    {

        // read small file < 64KB
        class ReadSmallFile : boost::noncopyable
        {
        public:
            ReadSmallFile(StringArg filename);
            ~ReadSmallFile();

            // return errno
            template <typename String>
            int readToString(int maxSize,
                             String *content,
                             int64_t *fileSize,
                             int64_t *modifyTime,
                             int64_t *createTime);

            /// Read at maxium kBufferSize into buf_
            // return errno
            int readToBuffer(int *size);

            const char *buffer() const { return buf_; }

            static const int kBufferSize = 64 * 1024;

        private:
            int fd_;
            int err_;
            char buf_[kBufferSize];
        };

        // read the file content, returns errno if error happens.
        template <typename String>
        int readFile(StringArg filename,
                     int maxSize,
                     String *content,
                     int64_t *fileSize = NULL,
                     int64_t *modifyTime = NULL,
                     int64_t *createTime = NULL)
        {
            ReadSmallFile file(filename);
            return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
        }

        // not thread safe
        class AppendFile : boost::noncopyable
        {
        public:
            explicit AppendFile(StringArg filename);

            ~AppendFile();

            void append(const char *logline, size_t len);

            void flush();

            off_t writtenBytes() const { return writtenBytes_; }

        private:
            size_t write(const char *logline, size_t len);

            FILE *fp_;
            char buffer_[64 * 1024];
            off_t writtenBytes_;
        };

    } // namespace FileUtil
} // namespace tinyMuduo

#endif // _FILEUTIL_H