#include <vm/vm_crypto.hpp>

#include <blake3.h>

namespace VMPilot::Common::VM::Crypto {

void blake3_kdf(const uint8_t seed[32], const char* context, size_t context_len,
                uint8_t* out, size_t out_len) noexcept {
    blake3_hasher hasher;
    blake3_hasher_init_derive_key_raw(&hasher, context, context_len);
    blake3_hasher_update(&hasher, seed, 32);
    blake3_hasher_finalize(&hasher, out, out_len);
}

void blake3_keyed_hash(const uint8_t key[32], const uint8_t* data, size_t data_len,
                       uint8_t* out, size_t out_len) noexcept {
    blake3_hasher hasher;
    blake3_hasher_init_keyed(&hasher, key);
    blake3_hasher_update(&hasher, data, data_len);
    blake3_hasher_finalize(&hasher, out, out_len);
}

}  // namespace VMPilot::Common::VM::Crypto
