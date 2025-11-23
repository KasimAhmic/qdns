#pragma once

#include <string>
#include <vector>

namespace util {
    namespace string {
        [[nodiscard]] inline std::string join(const std::vector<std::string>& parts, const std::string_view delim) {
            if (parts.empty()) return {};

            size_t total = (parts.size() - 1) * delim.size();

            for (auto& s : parts) {
                total += s.size();
            }

            std::string out;
            out.reserve(total);

            for (size_t i = 0; i < parts.size(); ++i) {
                out += parts[i];

                if (i + 1 < parts.size()) {
                    out += delim;
                }
            }

            return out;
        }

        // TODO: Optimize
        [[nodiscard]] inline std::vector<std::string> split(const std::string &string, const char delim) {
            std::vector<std::string> parts;
            size_t start = 0;
            size_t end = string.find(delim);

            while (end != std::string::npos) {
                parts.push_back(string.substr(start, end - start));
                start = end + 1;
                end = string.find(delim, start);
            }

            parts.push_back(string.substr(start));

            return parts;
        }
    }
}
