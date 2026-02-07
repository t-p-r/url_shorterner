#include "url_id_descriptor.hpp"

constexpr int INVALID_CHAR = -1;

auto url_id_descriptor::generate_url_id(random_device& rng) const -> url_id_t {
    url_id_t result;
    result.resize(URL_ID_WIDTH);

    for (int i = 0; i < URL_ID_WIDTH; i++)
        result[i] = url_id_to_char[rng.next_uint() % ALPHABET_SIZE];

    return result;
}

auto url_id_descriptor::is_valid_url_id(std::string_view s) const -> bool {
    if (s.size() != URL_ID_WIDTH)
        return false;

    for (char c : s)
        if (char_to_url_id[c] == INVALID_CHAR)
            return false;

    return true;
}

url_id_descriptor::url_id_descriptor() {
    std::fill(char_to_url_id.begin(), char_to_url_id.end(), INVALID_CHAR);

    int current_url_id = 0;
    for (int c = 'a'; c <= 'z'; c++) {
        url_id_to_char[current_url_id] = c;
        char_to_url_id[c] = current_url_id++;
    }
    for (int c = '0'; c <= '9'; c++) {
        url_id_to_char[current_url_id] = c;
        char_to_url_id[c] = current_url_id++;
    }

    SPDLOG_INFO("initialized character tables for {} characters",
                current_url_id);
}