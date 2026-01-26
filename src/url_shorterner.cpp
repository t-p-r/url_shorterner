#include <fmt/base.h>
#include <fmt/chrono.h>
#include <httplib/httplib.h>
#include <spdlog/spdlog.h>
#include "link_state.hpp"
#include "random_device.hpp"

constexpr int PORT = 6633;

constexpr int ASCII_SIZE = 255;
constexpr int TABLE_SIZE = 36;  // a-z0-9
constexpr int ID_WIDTH = 4;

std::array<int, ASCII_SIZE> char_to_id;
std::array<char, TABLE_SIZE> id_to_char;

constexpr void init_cid() {
    int current_id = 0;
    for (int c = 'a'; c <= 'z'; c++) {
        id_to_char[current_id] = c;
        char_to_id[c] = current_id++;
    }
    for (int c = '0'; c <= '9'; c++) {
        id_to_char[current_id] = c;
        char_to_id[c] = current_id++;
    }
    SPDLOG_INFO("initialized character tables for {} characters", current_id);
}

random_device rng;

std::string generate_id() {
    std::string result;
    result.resize(ID_WIDTH);

    for (int i = 0; i < ID_WIDTH; i++)
        result[i] = id_to_char[rng.get() % TABLE_SIZE];

    return result;
}

std::unordered_map<std::string, link_state> links;

int main() {
    SPDLOG_INFO("server started at UTC time {}", std::chrono::utc_clock::now());
    init_cid();

    SPDLOG_INFO("some random ids: {} {} {} {}", generate_id(), generate_id(),
                generate_id(), generate_id());

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

    server.Get("/:id", [](const httplib::Request& req, httplib::Response& res) {
        auto id = req.path_params.at("id");
        auto res_str = fmt::format("id is {}", id);
        res.set_content(res_str, "text/plain");
    });

    SPDLOG_INFO("server listening at port {}", PORT);
    server.listen("localhost", PORT);
}