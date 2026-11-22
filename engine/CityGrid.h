#pragma once
#include <string>
#include <fstream>
#include "Junction.h"
#include "Incident.h"
#include "AVL.h"
#include "BST.h"
#include "HashMap.h"

class CityGrid {
private:
    AVL<int, Junction> junctions;
    BST<int, Incident> incidents;
    HashMap<int, Incident> incidentIndex;

    static std::string trim(const std::string& text);
    static bool blank(const std::string& text);
    static bool fields(const std::string& line, int needed,
                       std::string result[]);
    static bool number(const std::string& text, int minimum,
                       int maximum, int& result);
    static bool junctionType(const std::string& text);
    static bool incidentCategory(const std::string& text);

    void loadJunctions(std::ifstream& input,
                       const std::string& filename);
    void loadIncidents(std::ifstream& input,
                       const std::string& filename);

public:
    bool run(const std::string& junctionPath,
             const std::string& incidentPath,
             const std::string& scriptPath);
};
