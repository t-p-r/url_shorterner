#include "url_id_descriptor.hpp"

auto url_id_descriptor::generate_url_id(random_device& rng) const -> url_id_t {
    url_id_t result;
    result.resize(URL_ID_WIDTH);

    for (int i = 0; i < URL_ID_WIDTH; i++)
        result[i] = url_id_to_char[rng.next_uint() % ALPHABET_SIZE];
        
    return result;
}

url_id_descriptor::url_id_descriptor() {
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