#pragma once

#include <array>
#include <bit>
#include <iostream>
#include <string>

namespace dns {
    constexpr uint16_t REQUEST_SIZE = 512;
    constexpr uint8_t HEADER_SIZE = 12;

    namespace detail {
        constexpr static uint16_t network_to_host16(const uint16_t x) {
            if constexpr (std::endian::native == std::endian::little) {
                return std::byteswap(x);
            } else {
                return x;
            }
        }

        static uint16_t read16(const std::span<const std::byte> data, const uint8_t offset) noexcept {
            uint16_t result;
            std::memcpy(&result, data.data() + offset, sizeof(uint16_t));
            return dns::detail::network_to_host16(result);
        }

        static uint8_t read8(const std::span<const std::byte> data, const uint8_t offset) noexcept {
            uint8_t result;
            std::memcpy(&result, data.data() + offset, sizeof(uint8_t));
            return result;
        }
    }

    enum class record : uint16_t {
        A = 1,
        NS = 2,
        CNAME = 5,
        SOA = 6,
        PTR = 12,
        HINFO = 13,
        MX = 15,
        TXT = 16,
        RP = 17,
        AFSDBB = 18,
        SIG = 24,
        KEY = 25,
        AAAA = 28,
        LOC = 29,
        SRV = 33,
        NAPTR = 35,
        KX = 36,
        CERT = 37,
        DNAME = 39,
        APL = 42,
        DS = 43,
        SSHFP = 44,
        IPSECKEY = 45,
        RRSIG = 46,
        NSEC = 47,
        DNSKEY = 48,
        DHCID = 49,
        NSEC3 = 50,
        NSEC3PARAM = 51,
        TLSA = 52,
        SMIMEA = 53,
        HIP = 55,
        CDS = 59,
        CDNSKEY = 60,
        OPENPGPKEY = 61,
        CSYNC = 62,
        ZONEMD = 63,
        SVCB = 64,
        HTTPS = 65,
        EUI48 = 108,
        EUI64 = 109,
        TKEY = 249,
        TSIG = 250,
        ANY = 255,
        URI = 256,
        CAA = 257,
        TA = 32768,
        DLV = 32769,
    };

    enum class dns_class : uint8_t {
        INTERNET = 1,
        CSNET = 2,
        CHAOS = 3,
        HESOID = 4,
    };

    struct header_flags {
        const bool qr;
        const uint8_t opcode;
        const bool aa;
        const bool tc;
        const bool rd;
        const bool ra;
        const uint8_t z;
        const uint8_t rcode;

        [[nodiscard]] static constexpr header_flags from(const uint16_t flags) noexcept {
            return {
                static_cast<bool>(flags >> 15 & 0x1),
                static_cast<uint8_t>(flags >> 11 & 0xF),
                static_cast<bool>(flags >> 10 & 0x1),
                static_cast<bool>(flags >> 9 & 0x1),
                static_cast<bool>(flags >> 8 & 0x1),
                static_cast<bool>(flags >> 7 & 0x1),
                static_cast<uint8_t>(flags >> 4 & 0x7),
                static_cast<uint8_t>(flags >> 0 & 0xF),
            };
        }

        [[nodiscard]] std::string to_string() const {
            return std::format(
                "[ QR: {} | OPCODE: {} | AA: {} | TC: {} | RD: {} | RA: {} | Z: {} | RCODE: {} ]",
                this->qr,
                this->opcode,
                this->aa,
                this->tc,
                this->rd,
                this->ra,
                this->z,
                this->rcode
            );
        }
    };

    /**
     * @brief Represents the DNS header section.
     * @code
     * DNS Header Layout
     *
     *  00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
     * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     * |                      ID                       |
     * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     * |QR|   Opcode  |AA|TC|RD|RA|    Z   |   RCODE   |
     * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     * |                    QDCOUNT                    |
     * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     * |                    ANCOUNT                    |
     * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     * |                    NSCOUNT                    |
     * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     * |                    ARCOUNT                    |
     * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     * @endcode
     */
    struct header {
        uint16_t id;
        header_flags flags;
        uint16_t qd_count;
        uint16_t an_count;
        uint16_t ns_count;
        uint16_t ar_count;

        static header from(const std::span<const std::byte> &data) {
            return {
                dns::detail::read16(data, 0),
                dns::header_flags::from(dns::detail::read16(data, 2)),
                dns::detail::read16(data, 4),
                dns::detail::read16(data, 6),
                dns::detail::read16(data, 8),
                dns::detail::read16(data, 10)
            };
        }

        [[nodiscard]] std::string to_string() const {
            return std::format(
                "ID: {} | Flags: {} | QD: {} | AN: {} | NS: {} | AR: {}",
                this->id,
                this->flags.to_string(),
                this->qd_count,
                this->an_count,
                this->ns_count,
                this->ar_count
            );
        }
    };

    struct question {
        std::string name;
        uint16_t type;
        uint16_t cls;

        static question from(const std::span<const std::byte> &data) {
            std::string name;
            uint8_t offset = 0;

            while (true) {
                const uint8_t len = dns::detail::read8(data, offset);
                offset++;

                if (len == 0) {
                    break;
                }

                for (uint8_t i = 0; i < len; i++) {
                    name += static_cast<char>(dns::detail::read8(data, offset + i));
                }

                name += '.';
                offset += len;
            }

            return {
                name,
                dns::detail::read16(data, offset),
                dns::detail::read16(data, offset + 2)
            };
        }

        static std::string read_label(const std::span<const std::byte> &data, const uint8_t offset, const uint8_t length) {
            std::string label;

            for (uint8_t i = 0; i < length; i++) {
                label += static_cast<char>(dns::detail::read8(data, offset + i));
            }

            label += '.';

            return label;
        }

        [[nodiscard]] std::string to_string() const {
            return std::format(
                "Name: {} | Type: {} | Class: {}",
                this->name,
                this->type,
                this->cls
            );
        }
    };

    struct answer {
        dns::header header;
        dns::question question;
        std::string name;
        uint16_t type;
        uint16_t cls;
        uint32_t ttl;
        uint16_t length;
        std::array<char, 128> data;

        static dns::answer from(const dns::header &header, const dns::question &question) {
            return answer{
                header,
                question
            };
        }

        // TODO: Implement fully
        [[nodiscard]] std::vector<std::byte> encode() const {
            std::vector<std::byte> result;

            result.push_back(static_cast<std::byte>(0b001));

            return result;
        }
    };

    struct request {
        dns::header header{};
        dns::question question{};
        dns::answer answer{};

        static dns::request from(const std::span<const std::byte> &data) {
            const dns::header header = dns::header::from(std::span(data.data(), dns::HEADER_SIZE));
            const dns::question question = dns::question::from(std::span(data.data() + dns::HEADER_SIZE, dns::REQUEST_SIZE - dns::HEADER_SIZE));

            return {
                header,
                question,
                dns::answer::from(header, question)
            };
        }

        [[nodiscard]] std::string to_string() const {
            return std::format(
                "Header: [ {} ] | Question: [ {} ]",
                this->header.to_string(),
                this->question.to_string()
            );
        }
    };
}
