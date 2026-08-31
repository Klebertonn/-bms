#pragma once

// Interface abstrata para leitura de células/tensões.
// Implementações devem existir para: Hardware real, Wokwi e Mock.

class IADC {
public:
    virtual ~IADC() = default;

    // @param index índice da célula [0..N-1]
    // @return tensão da célula em Volts
    virtual float readCell(int index) = 0;
};

