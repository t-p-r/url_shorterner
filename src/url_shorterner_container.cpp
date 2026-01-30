#include "url_shorterner_container.hpp"

constexpr int MAX_RETRY = 5;

// Throw at most MAX_RETRY random URL_IDs, see what sticks
// TODO: locks
auto url_shorterner_container::insert(link_destination&& link) -> url_id_t {
    for (int iter = 0; iter < MAX_RETRY; iter++) {
        url_id_t candidate = this->descriptor.generate_url_id(this->rng);
        if (this->container.try_emplace(candidate, link).second) {
            return candidate;
        }
    }
    return NULL_URL_ID;
}

auto url_shorterner_container::at(const url_id_t& id) const -> dest_url_t {
    auto it = this->container.find(id);
    return it == this->container.end() ? NULL_URL_ID : it->second.dest_url;
};