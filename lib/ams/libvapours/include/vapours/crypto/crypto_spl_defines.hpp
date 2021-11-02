#pragma once

namespace ams::crypto {
    namespace AesEncryptor128 {
        constexpr inline size_t KeySize = 16;
        constexpr inline size_t BlockSize = 16;
    }

    namespace Sha256Generator {
        constexpr inline size_t HashSize = 0x20;
    }
}
