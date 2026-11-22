#pragma once
#include <string>
#include "Junction.h"
#include "Incident.h"

class CityGrid {
public:
    bool run(const std::string& junctionPath,
             const std::string& incidentPath,
             const std::string& scriptPath);
};
