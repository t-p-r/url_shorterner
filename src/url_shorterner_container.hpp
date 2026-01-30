#ifndef URL_SHORTERNER_CONTAINER_HPP
#define URL_SHORTERNER_CONTAINER_HPP

#include "link_destination.hpp"
#include "random_device.hpp"
#include "url_id_descriptor.hpp"

class url_shorterner_container {
   public:
    using url_id_t = url_id_descriptor::url_id_t;
    using dest_url_t = link_destination::dest_url_t;
    using link_container = std::unordered_map<url_id_t, link_destination>;
    using time_point = link_destination::time_point;

    /**
     * To represent failures.
     */
    static constexpr url_id_t NULL_URL_ID = "";

    /**
     * Attempts to bind an URL to a shorterned URL_ID for easy sharing.
     * @param dest_url The URL that we want to share (e.g. google.com).
     * @return A valid URL_ID for use with the server (e.g. a1b2), or @c
     * NULL_URL_ID if the attempt fails.
     */
    url_id_t insert(link_destination&& link);

    /**
     * Find the destination URL that the URL_ID points to.
     * @param id
     * @return The destination URL, or @c NULL_URL_ID if the URL_ID currently
     * isn't pointed to any URLs.
     */
    dest_url_t at(const url_id_t& id) const;

    url_shorterner_container() = default;

    /**
     * Constructs the container from a file. The file must be a valid CSV file
     * where:
     * - the delimiter is the comma character `,`
     * - the headers are url_id,dest_url,expire_at, where:
     *      - url_id is a valid URL_ID
     *      - dest_url is a valid URL, where special characters must be rendered
     * in HEX format (e.g. space is %20)
     *      - expire_at is a valid ISO date string representing a UTC time point
     * (e.g. 2026-01-29T14:30:00Z)
     * - empty lines and spaces are permitted and will be ignored
     */
    url_shorterner_container(std::ifstream& ifs);

   private:
    random_device rng;
    url_id_descriptor descriptor;
    link_container container;
};

#endif  // URL_SHORTERNER_CONTAINER_HPP