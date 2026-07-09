#pragma once

#include <stdint.h>

enum class BLEAuthLevel
{
    NONE = 0,
    USER = 1,
    ADMIN = 2
};

struct BLEAuthState
{
    BLEAuthLevel level = BLEAuthLevel::NONE;

    // Placeholder: em implementação real, substituir por token/session.
    uint32_t lastAuthEpochMs = 0;
};

