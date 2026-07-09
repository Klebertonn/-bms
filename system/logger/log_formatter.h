#pragma once

#include <stddef.h>

#include "event.h"

class LogFormatter
{
public:
    // Formata em string (placeholder: imprime apenas campos base).
    static void format(const Event& e, char* out, size_t outLen);
};

