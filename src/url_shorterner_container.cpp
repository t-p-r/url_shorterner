#include "url_shorterner_container.hpp"

constexpr int MAX_RETRY = 5;
constexpr int DB_LINE_STATES_COUNT = 3;
constexpr std::string ZULU_ISO8601_FORMAT = "%FT%TZ";
enum DB_LINE_STATES { URL_ID = 0, DEST_URL = 1, EXPIRE_AT = 2 };

// Throw at most MAX_RETRY random URL_IDs, see what sticks
// @todo locks
auto url_shorterner_container::insert(link_destination link) -> url_id_t {
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

// doesn't need to be particularly fast
url_shorterner_container::url_shorterner_container(
    const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        SPDLOG_ERROR("failed to open: {}", filename);
        exit(EXIT_CODES::ERROR_IO);
    }

    int db_line_state = 0, line_count = 1;
    int imported = 0;

    std::string url_id;
    link_destination dest;
    std::string line;

    auto is_all_spaces = [](std::string_view line) -> bool {
        for (char c : line)
            if (c != ' ')
                return false;
        return true;
    };

    // From https://en.cppreference.com/w/cpp/chrono/parse.html
    auto parse_iso8601 = [filename,
                          &line_count](const std::string& line) -> time_point {
        time_point result;
        std::istringstream is(line);
        is.imbue(std::locale("en_US.utf-8"));
        is >> std::chrono::parse(ZULU_ISO8601_FORMAT, result);

        if (is.fail()) {
            SPDLOG_ERROR("in {}:{}, invalid time point: \"{}\"", filename,
                         line_count, line);
            exit(EXIT_CODES::ERROR_IO);
        }

        return result;
    };

    // main loop fetching lines from file
    while (std::getline(ifs, line)) {
        if (line.empty() || is_all_spaces(line))
            continue;

        switch (db_line_state) {
            case DB_LINE_STATES::URL_ID:
                if (descriptor.is_valid_url_id(line))
                    url_id = line;
                else {
                    SPDLOG_ERROR("in {}:{}, invalid URL_ID: {}", filename,
                                 line_count, line);
                    exit(EXIT_CODES::ERROR_IO);
                }
                break;
            case DB_LINE_STATES::DEST_URL:
                dest.dest_url = line;  // glhf to the browser handling this
                break;
            case DB_LINE_STATES::EXPIRE_AT:
                dest.expire_at = parse_iso8601(line);
                if (!dest.expired()) {
                    container.emplace(std::move(url_id), std::move(dest));
                    imported++;
                }
                break;
            default:
                SPDLOG_ERROR("wtf");
                exit(EXIT_CODES::ERROR_INEXPLICABLE);
        }

        // switch to next item to fetch
        db_line_state = (db_line_state + 1) % DB_LINE_STATES_COUNT;
        line_count++;
    }

    SPDLOG_INFO("imported {} entries from {}", imported, filename);
}