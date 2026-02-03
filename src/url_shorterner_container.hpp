#ifndef URL_SHORTERNER_CONTAINER_HPP
#define URL_SHORTERNER_CONTAINER_HPP

#include <chrono>
#include <fstream>
#include <locale>
#include <sstream>
#include "exit_codes.hpp"
#include "link_destination.hpp"
#include "random_device.hpp"
#include "url_id_descriptor.hpp"

class url_shorterner_container {
   public:
    using url_id_t = url_id_descriptor::url_id_t;
    using dest_url_t = link_destination::dest_url_t;
    url_shorterner_container() = default;
    using time_point = link_destination::time_point;
    using link_container = std::unordered_map<url_id_t, link_destination>;

    /**
     * To represent failures.
     *
     * @todo another one for dest_url?
     */
    static constexpr url_id_t NULL_URL_ID = "";

    /**
     * Attempts to bind an URL to a shorterned URL_ID for easy sharing.
     * @param dest_url The URL that we want to share (e.g. google.com).
     * @return A valid URL_ID for use with the server (e.g. a1b2), or @c
     * NULL_URL_ID if the attempt fails.
     *
     * @todo remove the link copy if possible
     */
    url_id_t insert(link_destination link);

    /**
     * Find the destination URL that the URL_ID points to.
     * @param id What you think it is.
     * @return The destination URL, or @c NULL_URL_ID if the URL_ID currently
     * isn't pointed to any URLs.
     */
    dest_url_t at(const url_id_t& id) const;

    /**
     * Constructs the container from a file. The file must has these properties:
     *
     * - The file must consist exclusively of valid lines, empty lines, or lines
     * consisting of only spaces. A valid line is one where all characters are
     * visible, have a width of 1, and are not spaces.
     *
     * - The number of valid lines must be `3 * X` where `X` is an unsigned
     * integer.
     *
     * - For all unsigned integers `i` between 0 and `X - 1`:
     *
     *     - valid line number `3i + 1` must be a valid URL_ID (e.g. a1b2)
     *
     *     - valid line number `3i + 2` must be a valid URL, where characters
     * with a width other than 1, or are otherwise not in common usage (i.e. not
     * on your keyboard), must be escaped using its hex code (e.g. Space ->
     * `%20`)
     *
     *     - valid line number `3i + 3` must be a valid ISO datetime string that
     * corresponds to a UTC time point (e.g. `2026-02-01T14:30:00Z`)
     *
     * In other words, when processing from the first non-empty line to the
     * last, we must be able to consecutively form groups of 3 lines
     * representing a bond between an URL_ID and its destination.
     *
     * Failure to satisfy these constraints will result in undefined behavior,
     * either within this function or beyond.
     *
     * @param filename File from which the container is formed.
     */
    url_shorterner_container(const std::string& filename);

   private:
    random_device rng;
    url_id_descriptor descriptor;
    link_container container;
};

#endif  // URL_SHORTERNER_CONTAINER_HPP