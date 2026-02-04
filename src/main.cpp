#include <fmt/base.h>
#include <fmt/chrono.h>
#include <httplib/httplib.h>
#include <spdlog/spdlog.h>
#include "url_shorterner_container.hpp"

using namespace std::chrono;

constexpr int PORT = 6633;
constexpr auto BIND_EXPIRATION_PERIOD = days(7);

int main() {
    SPDLOG_INFO("server started at UTC time {}", utc_clock::now());

    httplib::Server server;
    url_shorterner_container container;

    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("here be dragons", "text/plain");
        SPDLOG_INFO("200 GET /");
    });

    server.Get("/bind", [&container](const httplib::Request& req,
                                     httplib::Response& res) {
        if (req.has_param("url")) {
            auto dest_url = req.get_param_value("url");
            link_destination ld(dest_url,
                                utc_clock::now() + BIND_EXPIRATION_PERIOD);
            auto url_id = container.insert(ld);

            if (url_id == url_shorterner_container::NULL_URL_ID) {
                res.status = 503;
                res.set_content("Unable to generate shorterned URL_ID.",
                                "text/plain");
            }

            // @todo url isn't valid

            auto res_str =
                fmt::format("{{ \"url_id\": \"{}\", \"expire_at\": \"{}\"  }}",
                            url_id, ld.expire_at);

            res.set_content(res_str, "application/json");
        } else {
            res.status = 400;
            res.set_content(
                "Bad request: no 'url' parameter found, e.g. "
                "localhost:6633/bind?url=google.com",
                "text/plain");
            SPDLOG_INFO("400 GET /bind");
        }
    });

    server.Get("/r/:url_id", [&container](const httplib::Request& req,
                                        httplib::Response& res) {
        auto url_id = req.path_params.at("url_id");

        if (!container.descriptor.is_valid_url_id(url_id)) {
            res.status = 400;
            auto res_str = fmt::format("400 invalid URL_ID: {}", url_id);
            res.set_content(res_str, "text/plain");
            SPDLOG_INFO("400 GET /{}", url_id);
        } else {
            auto dest_url = container.at(url_id);

            if (dest_url == url_shorterner_container::NULL_URL_ID) {
                res.status = 404;
                auto res_str = fmt::format("404 no URL is bound to {}", url_id);
                res.set_content(res_str, "text/plain");
                SPDLOG_INFO("404 GET /{}", url_id);
            } else {
                // basically adds https:// to an URL if it didn't have that
                // already
                auto handle_https =
                    [](url_shorterner_container::dest_url_t& url) -> void {
                    constexpr char* HTTPS_PATTERN = "https://";
                    constexpr int PATTERN_LEN = strlen(HTTPS_PATTERN);
                    SPDLOG_INFO("{} + {} + {}", url, url.size(), PATTERN_LEN);
                    if (url.size() < PATTERN_LEN ||
                        strncmp(&url[0], &HTTPS_PATTERN[0], PATTERN_LEN)) {
                        url = HTTPS_PATTERN + url;
                    }
                };

                handle_https(dest_url);
                res.set_redirect(dest_url, 301);
                SPDLOG_INFO("200 GET /{} -> {}", url_id, dest_url);
            }
        }
    });

    SPDLOG_INFO("server now listening to localhost:{}", PORT);
    server.listen("localhost", PORT);
}
