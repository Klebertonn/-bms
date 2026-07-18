#pragma once

#include <vector>

#include "../../core/fault/fault_reason.h"

class FaultStorage
{
public:
    FaultStorage();


    bool init();

    bool append(const FaultInfo& fault);

    bool load(std::vector<FaultInfo>& history);

    bool clear();
};




