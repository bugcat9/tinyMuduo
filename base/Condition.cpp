#include "Condition.h"
#include <chrono>

/**
 * @brief returns true if time out, false otherwise.
 *
 * @param seconds
 * @return true if time out
 * @return false otherwise
 */
bool tinyMuduo::Condition::waitForSeconds(int64_t seconds)
{
    MutexLock::UnassignGuard ug(mutex_);
    std::unique_lock<std::mutex> lk(*(mutex_.getPthreadMutex()));

    return cond_.wait_for(lk, std::chrono::seconds(seconds)) == std::cv_status::timeout;
}