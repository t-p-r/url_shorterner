#include <fmt/chrono.h>
#include <httplib/httplib.h>
#include <spdlog/spdlog.h>
#include "url_shorterner_container.hpp"

using std::chrono::days;
using std::chrono::utc_clock;

constexpr int PORT = 6633;
constexpr auto BIND_TTL = days(7);

int main() {
    SPDLOG_INFO("server started at UTC time {}", utc_clock::now());

    httplib::Server server;
    url_shorterner_container container;

    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("here be dragons", "text/plain");
        SPDLOG_INFO("200 GET /");
    });

    // @todo 400 when url isn't valid
    server.Get("/bind", [&container](const httplib::Request& req,
                                     httplib::Response& res) {
        if (req.has_param("url")) {
            auto dest_url = req.get_param_value("url");
            link_destination ld(std::move(dest_url),
                                utc_clock::now() + BIND_TTL);

            auto link = container.insert(std::move(ld));
            if (link == container.end()) {
                res.status = 503;
                res.set_content("Unable to generate shorterned URL_ID.",
                                "text/plain");
            }

            auto res_str = fmt::format(
                "{{ \"url_id\": \"{}\", \"expire_at\": \"{:%FT:%TZ}\"  }}",
                link->first, link->second.expire_at);
            res.set_content(res_str, "application/json");

            SPDLOG_INFO(
                "200 GET /bind (dest_url = {} -> url_id = {}, expire_at = "
                "{:%FT:%TZ})",
                link->second.dest_url, link->first, link->second.expire_at);
        } else {
            res.status = 400;
            res.set_content(
                "400 Bad Request: no 'url' parameter found, e.g. "
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
            SPDLOG_INFO("400 GET /r/{}", url_id);
        } else {
            auto link = container.at(url_id);

            if (link == container.end()) {
                res.status = 404;
                auto res_str = fmt::format("404 no URL is bound to {}", url_id);
                res.set_content(res_str, "text/plain");
                SPDLOG_INFO("404 GET /r/{}", url_id);
            } else {
                auto handle_https =
                    [](url_shorterner_container::dest_url_t& url) -> void {
                    if (!std::strncmp(url.c_str(), "https://",
                                      std::strlen("https://")) ||
                        !std::strncmp(url.c_str(), "http://",
                                      std::strlen("http://")))
                        return;
                    url = "https://" + url;
                };

                handle_https(link->second.dest_url);
                res.set_redirect(link->second.dest_url, 301);
                SPDLOG_INFO("200 GET /r/{} -> {}", url_id,
                            link->second.dest_url);
            }
        }
    });

    SPDLOG_INFO("server now listening to http://localhost:{}", PORT);
    // @todo add docker define
    server.listen("0.0.0.0", PORT);
}
