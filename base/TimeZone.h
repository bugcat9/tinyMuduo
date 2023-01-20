#ifndef _TIMEZONE_H
#define _TIMEZONE_H

#include "copyable.h"
#include <memory>
#include <time.h>

namespace tinyMuduo
{

    // Local time in unspecified timezone.
    // A minute is always 60 seconds, no leap seconds.
    struct DateTime
    {
        DateTime() {}
        explicit DateTime(const struct tm &);
        DateTime(int _year, int _month, int _day, int _hour, int _minute, int _second)
            : year(_year), month(_month), day(_day), hour(_hour), minute(_minute), second(_second)
        {
        }

        // "2011-12-31 12:34:56"
        std::string toIsoString() const;

        int year = 0;   // [1900, 2500]
        int month = 0;  // [1, 12]
        int day = 0;    // [1, 31]
        int hour = 0;   // [0, 23]
        int minute = 0; // [0, 59]
        int second = 0; // [0, 59]
    };

    // TimeZone for 1970~2100
    class TimeZone : public tinyMuduo::copyable
    {
    public:
        TimeZone() = default;                        // an invalid timezone
        TimeZone(int eastOfUtc, const char *tzname); // a fixed timezone

        static TimeZone UTC();
        static TimeZone China(); // Fixed at GMT+8, no DST
        static TimeZone loadZoneFile(const char *zonefile);

        // default copy ctor/assignment/dtor are Okay.

        bool valid() const
        {
            // 'explicit operator bool() const' in C++11
            return static_cast<bool>(data_);
        }

        struct DateTime toLocalTime(int64_t secondsSinceEpoch, int *utcOffset = nullptr) const;
        int64_t fromLocalTime(const struct DateTime &, bool postTransition = false) const;

        // gmtime(3)
        static struct DateTime toUtcTime(int64_t secondsSinceEpoch);
        // timegm(3)
        static int64_t fromUtcTime(const struct DateTime &);

        struct Data;

    private:
        explicit TimeZone(std::unique_ptr<Data> data);

        std::shared_ptr<Data> data_;

        friend class TimeZoneTestPeer;
    };

} // namespace tinyMuduo

#endif // _TIMEZONE_H