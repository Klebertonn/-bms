#include "fault_storage.h"

#include <cstdint>
#include <cstdio>

static constexpr const char* FAULT_HISTORY_FILE = "fault_history.bin";

FaultStorage::FaultStorage() = default;

bool FaultStorage::init()
{
    // Verifica se existe; se não existir, cria arquivo vazio.
    FILE* f = std::fopen(FAULT_HISTORY_FILE, "rb");
    if (f)
    {
        std::fclose(f);
        return true;
    }

    f = std::fopen(FAULT_HISTORY_FILE, "wb");
    if (!f)
    {
        return false;
    }
    std::fclose(f);
    return true;
}

bool FaultStorage::append(const FaultInfo& fault)
{
    FILE* f = std::fopen(FAULT_HISTORY_FILE, "ab");
    if (!f)
    {
        return false;
    }

    const std::size_t written = std::fwrite(&fault, sizeof(FaultInfo), 1u, f);
    std::fclose(f);

    return (written == 1u);
}

bool FaultStorage::load(std::vector<FaultInfo>& history)
{
    FILE* f = std::fopen(FAULT_HISTORY_FILE, "rb");
    if (!f)
    {
        history.clear();
        return true;
    }

    history.clear();

    FaultInfo tmp;
    while (true)
    {
        const std::size_t readCount = std::fread(&tmp, sizeof(FaultInfo), 1u, f);
        if (readCount != 1u)
        {
            break;
        }

        history.push_back(tmp);
    }

    std::fclose(f);
    return true;
}

bool FaultStorage::clear()
{
    // Trunca o arquivo sem apagar.
    FILE* f = std::fopen(FAULT_HISTORY_FILE, "wb");
    if (!f)
    {
        return false;
    }

    std::fclose(f);
    return true;
}


