#include "app/app.h"
#include <iostream>

int main()
{
    std::cout << "=== BMS START ===" << std::endl;

    App app;

    app.init();

    std::cout << "=== BMS INIT OK ===" << std::endl;

    while(true)
    {
        app.update();

        std::cout << "BMS running..." << std::endl;

        break;
    }

    return 0;
}