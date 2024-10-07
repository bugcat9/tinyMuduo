#ifndef _HTTPRESPONSE_H
#define _HTTPRESPONSE_H

#include "base/copyable.h"
#include "base/Types.h"

#include <map>

namespace tinyMuduo
{
    namespace net
    {

        class Buffer;
        class HttpResponse : public tinyMuduo::copyable
        {
        public:
            enum HttpStatusCode
            {
                kUnknown,
                k200Ok = 200,
                k301MovedPermanently = 301,
                k400BadRequest = 400,
                k404NotFound = 404,
            };

            explicit HttpResponse(bool close)
                : statusCode_(kUnknown),
                  closeConnection_(close)
            {
            }

            void setStatusCode(HttpStatusCode code)
            {
                statusCode_ = code;
            }

            void setStatusMessage(const string &message)
            {
                statusMessage_ = message;
            }

            void setCloseConnection(bool on)
            {
                closeConnection_ = on;
            }

            bool closeConnection() const
            {
                return closeConnection_;
            }

            void setContentType(const string &contentType)
            {
                addHeader("Content-Type", contentType);
            }

            void setContentLength(int length)
            {
                addHeader("Content-Length", std::to_string(length));
            }
            // FIXME: replace string with StringPiece
            void addHeader(const string &key, const string &value)
            {
                headers_[key] = value;
            }

            void setBody(const string &body)
            {
                body_ = body;
            }

            void setFile(string file)
            {
                g_file = file;
            }

            void appendToBuffer(Buffer *output) const;
            string g_file;

        private:
            std::map<string, string> headers_;
            HttpStatusCode statusCode_;
            // FIXME: add http version
            string statusMessage_;
            bool closeConnection_;
            string body_;
        };

    } // namespace net
} // namespace tinyMuduo

#endif // _HTTPRESPONSE_H