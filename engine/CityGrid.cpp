#include "CityGrid.h"
#include <iostream>
#include <fstream>

bool CityGrid::run(const std::string& junctionPath,
                   const std::string& incidentPath,
                   const std::string& scriptPath) {

    std::ifstream junctionFile(junctionPath);
    if (!junctionFile) {
        std::cout << "FATAL: cannot open " << junctionPath << '\n';
        return false;
    }

    std::ifstream incidentFile(incidentPath);
    if (!incidentFile) {
        std::cout << "FATAL: cannot open " << incidentPath << '\n';
        return false;
    }

    std::ifstream scriptFile(scriptPath);
    if (!scriptFile) {
        std::cout << "FATAL: cannot open " << scriptPath << '\n';
        return false;
    }

    return true;
}
