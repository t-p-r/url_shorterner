#ifndef URL_ID_DESCRIPTOR_HPP
#define URL_ID_DESCRIPTOR_HPP

#include <spdlog/spdlog.h>
#include <array>
#include "random_device.hpp"

/**
 * We use an alphabet of lowercase Latin characters and single digits, i.e.
 * [a-z0-9] to form URL_ID characters.
 */
class url_id_descriptor {
   public:
    static constexpr int ASCII_SIZE = 255;
    static constexpr int ALPHABET_SIZE = 36;  // 26 chars + 10 digits
    static constexpr int URL_ID_WIDTH = 4;

    using url_id_t = std::string;

    /**
     * @param rng Random device to help generate the characters.
     *
     * @return @c URL_ID_WIDTH characters within the alphabet, randomly
     * generated (e.g. a1b2).
     *
     * @note Maybe this is in the wrong place?
     */
    url_id_t generate_url_id(random_device& rng) const;

    url_id_descriptor();

   private:
    std::array<int, ASCII_SIZE> char_to_url_id;
    std::array<char, ALPHABET_SIZE> url_id_to_char;
};

#endif  // URL_ID_DESCRIPTOR_HPP