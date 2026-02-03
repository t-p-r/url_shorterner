#include <fmt/base.h>
#include <fmt/chrono.h>
#include <httplib/httplib.h>
#include <spdlog/spdlog.h>
#include "url_shorterner_container.hpp"

constexpr int PORT = 6633;
constexpr auto BIND_EXPIRATION_PERIOD = std::chrono::days(7);

url_shorterner_container container;

int main() {
    using dest_url_t = url_shorterner_container::dest_url_t;
    using url_id_t = url_shorterner_container::url_id_t;
    using time_point = url_shorterner_container::time_point;
    using namespace std::chrono_literals;

    SPDLOG_INFO("server started at UTC time {}", std::chrono::utc_clock::now());

    httplib::Server server;

    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("here be dragons", "text/plain");
    });

    server.Get(
        "/bind", [](const httplib::Request& req, httplib::Response& res) {
            if (req.has_param("url")) {
                auto dest_url = req.get_param_value("url");

                link_destination ld(dest_url, std::chrono::utc_clock::now() +
                                                  BIND_EXPIRATION_PERIOD);

                auto url_id = container.insert(ld);

                if (url_id == url_shorterner_container::NULL_URL_ID) {
                    // @todo
                }

                // @todo url isn't valid

                auto res_str = fmt::format(
                    "{{ \"url_id\": \"{}\", \"expire_at\": \"{}\"  }}", url_id,
                    ld.expire_at);

                res.set_content(res_str, "application/json");
            } else {
                res.status = 400;
                res.set_content(
                    "Bad request: no 'url' parameter found, e.g. "
                    "localhost:6633/bind?url=google.com",
                    "text/plain");
            }
        });

    server.Get(
        "/:url_id", [](const httplib::Request& req, httplib::Response& res) {
            auto url_id = req.path_params.at("url_id");
            auto dest_url = container.at(url_id);

            SPDLOG_INFO("GET /{} -> {}", url_id, dest_url);

            // @todo invalid url_id

            if (dest_url == url_shorterner_container::NULL_URL_ID) {
                res.status = 404;
                auto res_str = fmt::format("404 no URL is bound to {}", url_id);
                res.set_content(res_str, "text/plain");
            } else {
                // @todo auto handle https://
                auto result = fmt::format("https://{}", dest_url);
                res.set_redirect(result);
            }
        });

    SPDLOG_INFO("server listening at port {}", PORT);
    server.listen("localhost", PORT);
}
