#pragma once

#include <cstddef>
#include <cstdint>

namespace q::dns {
  enum class Record : uint16_t {
    A          = 1,
    NS         = 2,
    CNAME      = 5,
    SOA        = 6,
    PTR        = 12,
    HINFO      = 13,
    MX         = 15,
    TXT        = 16,
    RP         = 17,
    AFSDBB     = 18,
    SIG        = 24,
    KEY        = 25,
    AAAA       = 28,
    LOC        = 29,
    SRV        = 33,
    NAPTR      = 35,
    KX         = 36,
    CERT       = 37,
    DNAME      = 39,
    APL        = 42,
    DS         = 43,
    SSHFP      = 44,
    IPSECKEY   = 45,
    RRSIG      = 46,
    NSEC       = 47,
    DNSKEY     = 48,
    DHCID      = 49,
    NSEC3      = 50,
    NSEC3PARAM = 51,
    TLSA       = 52,
    SMIMEA     = 53,
    HIP        = 55,
    CDS        = 59,
    CDNSKEY    = 60,
    OPENPGPKEY = 61,
    CSYNC      = 62,
    ZONEMD     = 63,
    SVCB       = 64,
    HTTPS      = 65,
    EUI48      = 108,
    EUI64      = 109,
    TKEY       = 249,
    TSIG       = 250,
    ANY        = 255,
    URI        = 256,
    CAA        = 257,
    TA         = 32768,
    DLV        = 32769,
  };

  enum class Class : uint16_t {
    INTERNET = 1,
    CSNET    = 2,
    CHAOS    = 3,
    HESIOD   = 4,
  };

} // namespace q::dns
