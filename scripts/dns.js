// @ts-check

import dns from "node:dns";

dns.setServers(["127.0.0.1:55555"]);

dns.resolve("google.com", "A", (err, addresses) => {
  console.log(err, addresses);
});
dns.resolve("example.org", "AAAA", (err, addresses) => {
  console.log(err, addresses);
});
dns.resolve("sub.domain.co.uk", "TXT", (err, addresses) => {
  console.log(err, addresses);
});

// const qr = 0b1000000000000000;
// const op = 0b0000000000000000;
// const aa = 0b0000000000000000;
// const tc = 0b0000000000000000;
// const rd = 0b0000000100000000;
// const ra = 0b0000000000000000;
// const z = 0b0000000000000000;
// const r = 0b0000000000000000;

// const target = 0b1000000100000000;
// const actual = qr | op | aa | tc | rd | ra | z | r;
// const flags = {
//     qr: (actual >> 15) & 0x1,
//     op: (actual >> 11) & 0xF,
//     aa: (actual >> 10) & 0x1,
//     tc: (actual >>  9) & 0x1,
//     rd: (actual >>  8) & 0x1,
//     ra: (actual >>  7) & 0x1,
//     z:  (actual >>  4) & 0x7,
//     r:  (actual >>  0) & 0xF,
// }

// console.log(JSON.stringify({ target, actual, flags }, null, 2));

/**
 *
 * @param {string} buffer - hex string
 * @param {number} bytes - number of bytes
 * @returns {number}
 */
function read8(buffer, bytes) {
  const offset = bytes * 2;
  return parseInt(buffer.slice(offset, offset + 2), 16);
}

/**
 *
 * @param {string} str
 * @returns
 */
function stringToHex(str) {
  return (
    str
      .split(".")
      .map(
        (part) =>
          part.length.toString(16).padStart(2, "0") +
          Array.from(part)
            .map((c) => c.charCodeAt(0).toString(16).padStart(2, "0"))
            .join("")
      )
      .join("") + "00"
  );
}

/**
 *
 * @type {string[]}
 */
const queries = [
  stringToHex("google.com"),
  stringToHex("example.org"),
  stringToHex("sub.domain.co.uk"),
];

for (const query of queries) {
  console.log({ query });
  let domain = "";
  let offset = 0;

  while (true) {
    const len = read8(query, offset);
    offset++;

    if (len === 0) {
      break;
    }

    for (let i = 0; i < len; i++) {
      domain += String.fromCharCode(read8(query, i + offset));
    }
    domain += ".";

    offset += len;
  }

  console.log({ domain });
}
