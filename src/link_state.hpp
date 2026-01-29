#ifndef LINK_STATE_HPP
#define LINK_STATE_HPP

#include <chrono>
#include <string>

using time_point = std::chrono::time_point<std::chrono::utc_clock>;

class link_state {
   public:
    std::string dest_url;
    time_point expire_at;

    bool expired() { return expire_at <= std::chrono::utc_clock::now(); }
};

#endif  // LINK_STATE_HPP