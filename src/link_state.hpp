#include <chrono>
#include <string>

using time_point = std::chrono::time_point<std::chrono::utc_clock>;

class link_state {
   private:
    std::string dest_url;
    time_point expire_at;

   public:
    bool expired() { return expire_at <= std::chrono::utc_clock::now(); }
};