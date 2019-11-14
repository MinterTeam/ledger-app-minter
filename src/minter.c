#include <stdbool.h>
#include <stdint.h>
#include <os.h>
#include <cx.h>
#include "minter.h"

void bin2hex(uint8_t *dst, uint8_t *data, uint64_t inlen) {
    static uint8_t const hex[] = "0123456789abcdef";
    for (uint64_t i = 0; i < inlen; i++) {
        dst[2 * i + 0] = hex[(data[i] >> 4) & 0x0F];
        dst[2 * i + 1] = hex[(data[i] >> 0) & 0x0F];
    }
    dst[2 * inlen] = '\0';
}
int bin2dec(uint8_t *dst, uint64_t n) {
    if (n == 0) {
        dst[0] = '0';
        dst[1] = '\0';
        return 1;
    }
    // determine final length
    int len = 0;
    for (uint64_t nn = n; nn != 0; nn /= 10) {
        len++;
    }
    // write digits in big-endian order
    for (int i = len - 1; i >= 0; i--) {
        dst[i] = (n % 10) + '0';
        n /= 10;
    }
    dst[len] = '\0';
    return len;
}

void deriveMinterKeypair(uint32_t index, cx_ecfp_private_key_t *privateKey, cx_ecfp_public_key_t *publicKey) {
    uint8_t root_privkey[32];
    cx_ecfp_private_key_t m_privKey;
    m_privKey.curve = CX_CURVE_SECP256K1;

    cx_ecfp_public_key_t m_pubKey;
    m_pubKey.curve = CX_CURVE_SECP256K1;

    uint8_t master_seed[12] = "Bitcoin seed";

    // bip32 path for: 44'/60'/n'/0'/0'
    // minter path:    44'/60'/0'/0/0
    uint32_t bip32Path[] = {
        44u | 0x80000000u,
        60u | 0x80000000u,
        index | 0x80000000u,
        0x0u,
        0x0u
    };

    os_perso_derive_node_bip32_seed_key(HDW_NORMAL, MINTER_CURVE, bip32Path, 5, root_privkey, NULL, master_seed, 12);
    cx_ecfp_init_private_key(MINTER_CURVE, root_privkey, 32, &m_privKey);
    cx_ecfp_init_public_key(MINTER_CURVE, NULL, 0, &m_pubKey);
    cx_ecfp_generate_pair(MINTER_CURVE, &m_pubKey, &m_privKey, 1);

    if (publicKey) {
        *publicKey = m_pubKey;
    }
    if (privateKey) {
        *privateKey = m_privKey;
    }

    os_memset(&m_privKey, 0, sizeof(m_privKey));
}

void deriveAndSign(uint8_t *dst, uint32_t index, const uint8_t *hash) {
    cx_ecfp_private_key_t privateKey;
    deriveMinterKeypair(index, &privateKey, NULL);

    const uint32_t signature_cap = 100;
    uint8_t signature[signature_cap];

    uint32_t info = 0;
    cx_ecdsa_sign(
        &privateKey,
        CX_RND_RFC6979 | CX_LAST,
        CX_SHA256,
        hash,
        CX_SHA256_SIZE,
        signature,
        signature_cap,
        &info);

    os_memset(&privateKey, 0, sizeof(privateKey));

    uint8_t rec[1] = {(uint8_t) 27};

    // signature TLV encoded
    // structure of it: 30 L 02 Lr r 02 Ls s
    // as our method is 100% determinated, just getting data from fixed positions
    os_memmove(dst + 0, signature + 4 + 0 + 0, 32);
    os_memmove(dst + 32, signature + 4 + 32 + 2, 32);
    os_memmove(dst + 64, rec, 1);

    os_memset(signature, 0, signature_cap);
}

size_t pubkeyToMinterAddress(uint8_t *dst, const uint8_t *publicKey) {
    uint8_t dropped_first[64];
    os_memmove(dropped_first, publicKey + 1, 64);
    PRINTF("dropped_first buffer:\n %.*H \n\n", 65, dropped_first);
    uint8_t hashed[CX_SHA3K_SIZE];

    cx_sha3_t sha3k_ctx;
    cx_keccak_init(&sha3k_ctx, CX_SHA3K_SIZE * 8);
    cx_hash(&sha3k_ctx.header, CX_LAST, dropped_first, 64, hashed, CX_SHA3K_SIZE);

    PRINTF("address:\n Mx%.*h \n\n", 20, hashed + 12);

    os_memmove(dst, hashed + 12, 20);
    return 20; // 20 bytes address
}
