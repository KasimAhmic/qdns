import { Resolver } from 'node:dns/promises';
import test from 'node:test';
import assert from 'node:assert/strict';

const resolver = new Resolver({
  maxTimeout: 10,
  timeout: 10,
  tries: 1,
});

resolver.setServers(['127.0.0.1:55555']);

class DnsUtil {
  // TODO: Expected should be an array
  private readonly aRecordTests = [
    { address: 'dns.google', expected: '8.8.8.8' },
    { address: 'google.com', expected: '192.178.50.78' },
    { address: 'one.one.one.one', expected: '1.1.1.1' },
  ];

  private readonly codecTests: string[] = ['google.com', 'example.org', 'sub.domain.co.uk'];

  async run(args: string[]): Promise<void> {
    if (args.length === 0) {
      throw new Error('Invalid arguments');
    }

    const command = args.at(0);

    switch (command) {
      case 'int':
      case 'integration-test':
        return this.runIntegrationTests();
      case 'imp':
      case 'implementation-test':
        return this.runImplementationTests();
    }
  }

  private runIntegrationTests(): void {
    for (const { address, expected } of this.aRecordTests) {
      test(`resolves ${address}`, async () => {
        const result = await resolver.resolve(address);

        assert.strictEqual(result, expected);
      });
    }
  }

  private runImplementationTests() {
    this.testCodecImpl();
    this.testHeaderParserImpl();
  }

  private testHeaderParserImpl() {
    const qr = 0b1000000000000000;
    const op = 0b0000000000000000;
    const aa = 0b0000000000000000;
    const tc = 0b0000000000000000;
    const rd = 0b0000000100000000;
    const ra = 0b0000000000000000;
    const z = 0b0000000000000000;
    const r = 0b0000000000000000;

    const target = 0b1000000100000000;
    const actual = qr | op | aa | tc | rd | ra | z | r;
    const flags = {
      qr: (actual >> 15) & 0x1,
      op: (actual >> 11) & 0xf,
      aa: (actual >> 10) & 0x1,
      tc: (actual >> 9) & 0x1,
      rd: (actual >> 8) & 0x1,
      ra: (actual >> 7) & 0x1,
      z: (actual >> 4) & 0x7,
      r: (actual >> 0) & 0xf,
    };

    assert.strictEqual(actual, target);
    assert.strictEqual(qr, flags.qr);
    assert.strictEqual(op, flags.op);
    assert.strictEqual(aa, flags.aa);
    assert.strictEqual(tc, flags.tc);
    assert.strictEqual(rd, flags.rd);
    assert.strictEqual(ra, flags.ra);
    assert.strictEqual(z, flags.z);
    assert.strictEqual(r, flags.r);
  }

  private testCodecImpl() {
    for (const address of this.codecTests) {
      test(`encodes and decodes ${address} properly`, () => {
        const query = this.stringToHex(address);

        let domain = '';
        let offset = 0;

        while (true) {
          const len = this.read8(query, offset);
          offset++;

          if (len === 0) {
            break;
          }

          for (let i = 0; i < len; i++) {
            domain += String.fromCharCode(this.read8(query, i + offset));
          }
          domain += '.';

          offset += len;
        }

        assert.strictEqual(domain, `${address}.`);
      });
    }
  }

  private read8(buffer: string, bytes: number): number {
    const offset = bytes * 2;

    return parseInt(buffer.slice(offset, offset + 2), 16);
  }

  private stringToHex(str: string): string {
    return (
      str
        .split('.')
        .map(
          (part) =>
            part.length.toString(16).padStart(2, '0') +
            Array.from(part)
              .map((c) => c.charCodeAt(0).toString(16).padStart(2, '0'))
              .join(''),
        )
        .join('') + '00'
    );
  }
}

new DnsUtil().run(process.argv.slice(2)).then(() => console.log('Done'));
