#pragma once

#include <cstdint>

/*
 * ==========================================================
 * Interface HAL — Display (OLED)
 * ==========================================================
 * Abstrai displays alfanuméricos (ex.: SSD1306/OLED).
 * A lógica de negócio pode exibir telemetria sem depender
 * do driver específico de hardware.
 * ==========================================================
 */
class IDisplay
{
public:
    virtual ~IDisplay() = default;

    // Inicializa o display.
    virtual bool init() = 0;

    // Limpa a tela.
    virtual void clear() = 0;

    // Escreve uma linha de texto (linha 0..N-1).
    virtual void printLine(uint8_t line, const char* text) = 0;

    // Atualiza/verifica renderização.
    virtual void refresh() = 0;
};
