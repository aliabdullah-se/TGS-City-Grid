#include <iostream>
#include "CityGrid.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout
            << "USAGE: citygrid <junctions file> <incidents file> <script file>\n";
        return 1;
    }

    CityGrid app;

    if (!app.run(argv[1], argv[2], argv[3]))
        return 1;

    return 0;
}
