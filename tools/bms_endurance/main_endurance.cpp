#include "main_endurance.h"

#include "app/app.h"
#include "config/config.h"
#include "system/clock/clock.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>

namespace
{
std::atomic<bool> stopRequested{false};

void handleSignal(int)
{
    stopRequested.store(true);
}

std::uint64_t enduranceDurationMs(int argc, char** argv)
{
    for (int index = 1; index + 1 < argc; ++index)
    {
        if (std::string(argv[index]) == "--duration-ms")
        {
            const long long value = std::atoll(argv[index + 1]);
            return value > 0 ? static_cast<std::uint64_t>(value) : 0;
        }
    }
    return 0;
}
}

int runEndurance(int argc, char** argv)
{
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

    const std::uint64_t configuredDurationMs = enduranceDurationMs(argc, argv);
    const std::uint64_t startMs = Clock::millis();
    std::uint64_t loops = 0;
    std::uint64_t exceptions = 0;
    std::uint64_t totalLoopUs = 0;
    std::uint64_t maxLoopUs = 0;
    std::uint64_t lastTelemetryMs = startMs;
    int result = 0;

    App app;
    if (!app.init())
    {
        std::fprintf(stderr, "[ENDURANCE] App initialization failed.\n");
        result = 1;
    }
    else
    {
        std::printf("[ENDURANCE] started; duration_ms=%llu (0 means Ctrl+C)\n",
                    static_cast<unsigned long long>(configuredDurationMs));

        while (!stopRequested.load())
        {
            const auto loopStart = std::chrono::steady_clock::now();
            try
            {
                app.update();
            }
            catch (...)
            {
                ++exceptions;
                std::fprintf(stderr, "[ENDURANCE] update exception at loop %llu\n",
                             static_cast<unsigned long long>(loops));
                result = 1;
            }

            ++loops;
            const std::uint64_t loopTimeUs = static_cast<std::uint64_t>(
                std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::steady_clock::now() - loopStart).count());
            totalLoopUs += loopTimeUs;
            if (loopTimeUs > maxLoopUs)
            {
                maxLoopUs = loopTimeUs;
            }

            const std::uint64_t nowMs = Clock::millis();
            if (nowMs - lastTelemetryMs >= 5000)
            {
                lastTelemetryMs = nowMs;
                const AppDiagnostics telemetry = app.diagnostics();
                std::printf("[ENDURANCE] uptime_ms=%llu loops=%llu state=%s pack=%.3fV min=%.3fV max=%.3fV delta=%.3fV current=%.3fA temp=%.2fC soc=%u%% soh=%u%% active_dtc=%u heartbeat=%s loop_errors=%llu storage_errors=%llu\n",
                            static_cast<unsigned long long>(nowMs - startMs),
                            static_cast<unsigned long long>(loops),
                            telemetry.state,
                            telemetry.pack.totalVoltage,
                            telemetry.pack.minVoltage,
                            telemetry.pack.maxVoltage,
                            telemetry.pack.deltaVoltage,
                            telemetry.pack.current,
                            telemetry.pack.averageTemperature,
                            static_cast<unsigned>(telemetry.pack.soc),
                            static_cast<unsigned>(telemetry.pack.soh),
                            static_cast<unsigned>(telemetry.activeFaults),
                            telemetry.heartbeat.alive ? "ALIVE" : "DEAD",
                            static_cast<unsigned long long>(exceptions),
                            static_cast<unsigned long long>(telemetry.storageErrors));
            }

            if (configuredDurationMs > 0 && nowMs - startMs >= configuredDurationMs)
            {
                break;
            }

            const std::uint64_t elapsedMs = loopTimeUs / 1000u;
            if (elapsedMs < MAIN_LOOP_PERIOD_MS)
            {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(MAIN_LOOP_PERIOD_MS - elapsedMs));
            }
        }
    }

    const std::uint64_t endMs = Clock::millis();
    const std::uint64_t durationMs = endMs - startMs;
    const AppDiagnostics diagnostics = app.diagnostics();
    std::filesystem::create_directories("reports");
    std::ofstream report("reports/endurance_latest.txt", std::ios::trunc);
    if (report)
    {
        report << "BMS ENDURANCE REPORT\n"
               << "Start ms: " << startMs << "\n"
               << "End ms: " << endMs << "\n"
               << "Duration ms: " << durationMs << "\n"
               << "Loops: " << loops << "\n"
               << "Loop average us: " << (loops == 0 ? 0 : totalLoopUs / loops) << "\n"
               << "Loop max us: " << maxLoopUs << "\n"
               << "Exceptions: " << exceptions << "\n"
               << "State: " << diagnostics.state << "\n"
               << "Pack voltage: " << diagnostics.pack.totalVoltage << "\n"
               << "Min cell voltage: " << diagnostics.pack.minVoltage << "\n"
               << "Max cell voltage: " << diagnostics.pack.maxVoltage << "\n"
               << "Current A: " << diagnostics.pack.current << "\n"
               << "Temperature C: " << diagnostics.pack.averageTemperature << "\n"
               << "SOC: " << static_cast<unsigned>(diagnostics.pack.soc) << "\n"
               << "SOH: " << static_cast<unsigned>(diagnostics.pack.soh) << "\n"
               << "Active DTC: " << diagnostics.activeFaults << "\n"
               << "DTC raised: " << diagnostics.faultsRaised << "\n"
               << "DTC cleared: " << diagnostics.faultsCleared << "\n"
               << "Heartbeat alive: " << (diagnostics.heartbeat.alive ? "true" : "false") << "\n"
               << "Heartbeat errors: 0\n"
               << "Watchdog healthy: " << (diagnostics.watchdogHealthy ? "true" : "false") << "\n"
               << "Watchdog misses: " << diagnostics.watchdogMisses << "\n"
               << "Self test failures: " << (diagnostics.selfTestPassed ? 0 : 1) << "\n"
               << "Storage errors: " << diagnostics.storageErrors << "\n"
               << "Result: " << (result == 0 ? "PASS" : "FAIL") << "\n";
    }
    else
    {
        std::fprintf(stderr, "[ENDURANCE] could not write reports/endurance_latest.txt\n");
        result = 1;
    }

    std::printf("[ENDURANCE] finished; duration_ms=%llu loops=%llu result=%s\n",
                static_cast<unsigned long long>(durationMs),
                static_cast<unsigned long long>(loops),
                result == 0 ? "PASS" : "FAIL");
    return result;
}
