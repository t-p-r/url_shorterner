#ifndef URL_ID_DESCRIPTOR_HPP
#define URL_ID_DESCRIPTOR_HPP

#include <spdlog/spdlog.h>
#include <array>
#include "random_device.hpp"

/**
 * We define an URL_ID string to be a string satisfying the regex [a-z0-9]{4},
 * meaning that it is a 4-character string composed of either lowercase English
 * characters or decimal digits.
 *
 * This facilitates the creation of shortened links, for example:
 *
 * http://localhost:6633/a1b2 -> https://www.youtube.com/watch?v=i-6jdvdAiEQ
 *
 * where our URL_ID is a1b2.
 */
class url_id_descriptor {
   public:
    static constexpr int ASCII_SIZE = 255;
    static constexpr int ALPHABET_SIZE = 36;  // 26 chars + 10 digits
    static constexpr int URL_ID_WIDTH = 4;

    using url_id_t = std::string;

    /**
     * @param rng Random device to help generate the characters.
     * @return A randomly generated URL_ID (e.g. a1b2).
     * @note Maybe this is in the wrong place?
     */
    url_id_t generate_url_id(random_device& rng) const;

    /**
     * @return true if `s` is a valid URL_ID, false otherwise.
     */
    bool is_valid_url_id(std::string_view s) const;

    url_id_descriptor();

   private:
    std::array<int, ASCII_SIZE> char_to_url_id;
    std::array<char, ALPHABET_SIZE> url_id_to_char;
};

#endif  // URL_ID_DESCRIPTOR_HPP