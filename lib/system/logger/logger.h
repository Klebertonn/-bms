#pragma once

// Logger abstrato (stubs iniciais)
// Implementações podem escrever em Serial, armazenamento ou telemetria.

class ILogger {
public:
    virtual ~ILogger() = default;

    virtual void info(const char* msg) = 0;
    virtual void warn(const char* msg) = 0;
    virtual void error(const char* msg) = 0;
};

