#ifndef URL_SHORTERNER_CONTAINER_HPP
#define URL_SHORTERNER_CONTAINER_HPP

#include "link_state.hpp"
#include "random_device.hpp"
#include "url_id_descriptor.hpp"

class url_shorterner_container {
   public:
    using url_id_t = url_id_descriptor::url_id_t;
    using dest_url_t = std::string;
    using link_container = std::unordered_map<url_id_t, link_state>;

    url_id_t insert(const dest_url_t& dest_url);

    dest_url_t at(const url_id_t& id) const;

    url_shorterner_container() = default;

   private:
    random_device rng;
    url_id_descriptor descriptor;
    link_container container;
};

#endif  // URL_SHORTERNER_CONTAINER_HPP