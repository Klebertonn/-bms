#pragma once

#include <cstdint>

class WatchdogManager
{
public:
    void init();

    void heartbeat(unsigned taskIndex = 0);

    void update();

    bool isHealthy() const { return healthy_; }
    std::uint32_t timeoutMs() const { return timeoutMs_; }
    std::uint32_t missedHeartbeats() const { return missedHeartbeats_; }

private:
    static constexpr std::uint32_t DEFAULT_TIMEOUT_MS = 500;
    std::uint64_t lastHeartbeatMs_ = 0;
    std::uint32_t timeoutMs_ = DEFAULT_TIMEOUT_MS;
    std::uint32_t missedHeartbeats_ = 0;
    bool healthy_ = false;
};

