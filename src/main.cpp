#include "app/app.h"
#include "../tools/bms_endurance/main_endurance.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdio>

int main(int argc, char** argv)
{
    // Desabilita buffering de stdout para saída imediata em pipes/redirecionamento.
    std::setvbuf(stdout, nullptr, _IONBF, 0);

    if (argc > 1 && std::string(argv[1]) == "--endurance")
    {
        return runEndurance(argc, argv);
    }

    std::cout << "=== BMS START ===" << std::endl;

    App app;

    app.init();

    std::cout << "=== BMS INIT OK ===" << std::endl;

    while (true)
    {
        app.update();

        // Loop principal ~100 ms
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}