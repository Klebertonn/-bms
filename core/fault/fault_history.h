#pragma once

#include <cstddef>

#include "fault_reason.h"

class FaultHistory
{
public:
    void push(const FaultInfo& fault);

    const FaultInfo& at(size_t index) const;

    size_t size() const { return count_; }

    void clear();

private:
    static constexpr size_t MAX_HISTORY = 50;

    // Circular buffer
    FaultInfo history_[MAX_HISTORY];
    size_t head_ = 0;  // Próxima posição a escrever
    size_t count_ = 0; // Quantidade válida (até MAX_HISTORY)
};

