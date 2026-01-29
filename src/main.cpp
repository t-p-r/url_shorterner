#include <fmt/base.h>
#include <fmt/chrono.h>
#include <httplib/httplib.h>
#include <spdlog/spdlog.h>
#include "url_shorterner_container.hpp"

constexpr int PORT = 6633;

url_shorterner_container container;

int main() {
    SPDLOG_INFO("server started at UTC time {}", std::chrono::utc_clock::now());

    httplib::Server server;

    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("here be dragons", "text/plain");
    });

    server.Get(
        "/bind", [](const httplib::Request& req, httplib::Response& res) {
            if (req.has_param("url")) {
                auto val = req.get_param_value("url");
                auto res_str = fmt::format("binding {} to {}", val, "NULL");
                res.set_content(res_str, "text/plain");
            } else {
                res.status = 400;
                res.set_content(
                    "Bad request: no 'url' parameter found, e.g. "
                    "localhost:6633/bind?url=google.com",
                    "text/plain");
            }
        });

    server.Get("/:url_id",
               [](const httplib::Request& req, httplib::Response& res) {
                   auto url_id = req.path_params.at("url_id");
                   auto res_str = fmt::format("url_id is {}", url_id);
                   res.set_content(res_str, "text/plain");
               });

    SPDLOG_INFO("server listening at port {}", PORT);
    server.listen("localhost", PORT);
}
