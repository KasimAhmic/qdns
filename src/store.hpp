#pragma once

#include "util.hpp"

namespace dns {
    struct entry {
        std::string hostname;
        uint32_t ttl;
        std::vector<std::string> addresses; // TODO: Use uint32_t for addresses. Makes encoding easier and likely saves on memory

        [[nodiscard]] std::string to_string() const {
            return std::format(
                "Hostname: {}, TTL: {}, Addresses: [{}]",
                this->hostname,
                this->ttl,
                util::string::join(this->addresses, ", ")
            );
        }
    };

    struct store {
        std::vector<dns::entry> entries;

        // TODO: Convert this to a hashmap and use b-tree or similar for faster lookups
        [[nodiscard]] std::optional<dns::entry> find(const std::string &hostname) const {
            for (const auto &entry: this->entries) {
                if (entry.hostname == hostname) {
                    return entry;
                }
            }

            return std::nullopt;
        }

        void add(const dns::entry &entry) {
            this->entries.push_back(entry);
        }
    };
}
