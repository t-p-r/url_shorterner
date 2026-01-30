#ifndef LINK_DESTINATION_HPP
#define LINK_DESTINATION_HPP

#include <chrono>
#include <string>

/**
 * Defenition of a destination of a shorterned link.
 * @param dest_url Destination URL that a shortened link points to.
 * @param expire_at UTC time at which the bond between the URL_ID and dest_url
 * expires (and said URL_ID can now bond with another URL).
 */
class link_destination {
   public:
    using dest_url_t = std::string;
    using time_point = std::chrono::time_point<std::chrono::utc_clock>;

    dest_url_t dest_url;
    time_point expire_at;

    bool expired() { return expire_at <= std::chrono::utc_clock::now(); }
};

#endif  // LINK_DESTINATION_HPP