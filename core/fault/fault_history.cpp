#include "fault_history.h"

void FaultHistory::push(const FaultInfo& fault)
{
    history_[head_] = fault;
    head_ = (head_ + 1u) % MAX_HISTORY;
    if (count_ < MAX_HISTORY)
    {
        ++count_;
    }
}

const FaultInfo& FaultHistory::at(size_t index) const
{
    // index 0 => mais recente
    // Mantemos um arranjo circular: o elemento mais recente está em (head_-1).
    if (index >= count_)
    {
        // Em firmware, evitar exceções. Retornar um “default” estático.
        static FaultInfo empty{};
        return empty;
    }

    const size_t latestPos = (head_ + MAX_HISTORY - 1u) % MAX_HISTORY;
    const size_t pos = (latestPos + MAX_HISTORY - index) % MAX_HISTORY;
    return history_[pos];
}

void FaultHistory::clear()
{
    head_ = 0u;
    count_ = 0u;
    // Não precisamos limpar o array inteiro para evitar custo.
}

