#include "CityGrid.h"
#include <iostream>

std::string CityGrid::trim(const std::string& text) {
    std::string::size_type first =
        text.find_first_not_of(" \t");

    if (first == std::string::npos)
        return "";

    std::string::size_type last =
        text.find_last_not_of(" \t");

    return text.substr(first, last - first + 1);
}

bool CityGrid::blank(const std::string& text) {
    return trim(text).empty();
}

bool CityGrid::fields(const std::string& line, int needed,
                      std::string result[]) {
    std::string::size_type start = 0;
    int count = 0;

    for (std::string::size_type i = 0;
         i <= line.size(); ++i) {

        if (i == line.size() || line[i] == '|') {
            if (count >= needed)
                return false;

            result[count++] =
                trim(line.substr(start, i - start));

            start = i + 1;
        }
    }

    return count == needed;
}

bool CityGrid::number(const std::string& text,
                      int minimum, int maximum,
                      int& result) {
    if (text.empty() || text.size() > 9)
        return false;

    int value = 0;

    for (std::string::size_type i = 0;
         i < text.size(); ++i) {

        if (text[i] < '0' || text[i] > '9')
            return false;

        value = value * 10 + (text[i] - '0');
    }

    if (value < minimum || value > maximum)
        return false;

    result = value;
    return true;
}

bool CityGrid::junctionType(const std::string& text) {
    return text == "HOSPITAL" ||
           text == "FIRE" ||
           text == "POLICE" ||
           text == "DEPOT" ||
           text == "PLAIN";
}

bool CityGrid::incidentCategory(const std::string& text) {
    return text == "MEDICAL" ||
           text == "FIRE" ||
           text == "CRIME";
}

void CityGrid::loadJunctions(
    std::ifstream& input,
    const std::string& filename) {

    std::string line;
    int lineNumber = 0;
    int accepted = 0;
    int rejected = 0;

    while (std::getline(input, line)) {
        ++lineNumber;

        // Remove Windows carriage return.
        if (!line.empty() &&
            line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        if (blank(line))
            continue;

        std::string part[3];
        int id = 0;
        std::string reason;

        if (!fields(line, 3, part))
            reason = "wrong field count";
        else if (!number(part[0], 1, 999999999, id))
            reason = "invalid junction ID";
        else if (part[1].empty())
            reason = "empty name";
        else if (!junctionType(part[2]))
            reason = "invalid type";
        else if (junctions.find(id) != nullptr)
            reason = "duplicate junction ID";

        if (!reason.empty()) {
            std::cout << "ERROR " << filename
                      << ':' << lineNumber
                      << ": " << reason << '\n';

            ++rejected;
            continue;
        }

        Junction junction;
        junction.id = id;
        junction.name = part[1];
        junction.type = part[2];

        junctions.insert(id, junction);
        ++accepted;
    }

    std::cout << "LOAD " << filename << ": "
              << accepted << " accepted, "
              << rejected << " rejected\n";
}

void CityGrid::loadIncidents(
    std::ifstream& input,
    const std::string& filename) {

    std::string line;
    int lineNumber = 0;
    int accepted = 0;
    int rejected = 0;

    while (std::getline(input, line)) {
        ++lineNumber;

        if (!line.empty() &&
            line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        if (blank(line))
            continue;

        std::string part[5];
        int id = 0;
        int junctionId = 0;
        int severity = 0;
        int timestamp = 0;
        std::string reason;

        if (!fields(line, 5, part))
            reason = "wrong field count";
        else if (!number(part[0], 1, 999999999, id))
            reason = "invalid incident ID";
        else if (!number(part[1], 1, 999999999,
                         junctionId))
            reason = "invalid junction ID";
        else if (!incidentCategory(part[2]))
            reason = "invalid category";
        else if (!number(part[3], 1, 5, severity))
            reason = "invalid severity";
        else if (!number(part[4], 0, 999999999,
                         timestamp))
            reason = "invalid timestamp";
        else if (incidentIndex.find(id) != nullptr)
            reason = "duplicate incident ID";
        else if (junctions.find(junctionId) == nullptr)
            reason = "junction " +
                     std::to_string(junctionId) +
                     " not found";

        if (!reason.empty()) {
            std::cout << "ERROR " << filename
                      << ':' << lineNumber
                      << ": " << reason << '\n';

            ++rejected;
            continue;
        }

        Incident* incident = new Incident;

        incident->id = id;
        incident->junctionId = junctionId;
        incident->category = part[2];
        incident->severity = severity;
        incident->timestamp = timestamp;
        incident->status = "REPORTED";

        // BST owns the Incident object.
        incidents.insert(id, incident);

        // HashMap stores the same pointer.
        incidentIndex.insert(id, incident);

        ++accepted;
    }

    std::cout << "LOAD " << filename << ": "
              << accepted << " accepted, "
              << rejected << " rejected\n";
}

bool CityGrid::run(
    const std::string& junctionPath,
    const std::string& incidentPath,
    const std::string& scriptPath) {

    std::ifstream junctionFile(junctionPath);

    if (!junctionFile) {
        std::cout << "FATAL: cannot open "
                  << junctionPath << '\n';
        return false;
    }

    std::ifstream incidentFile(incidentPath);

    if (!incidentFile) {
        std::cout << "FATAL: cannot open "
                  << incidentPath << '\n';
        return false;
    }

    std::ifstream scriptFile(scriptPath);

    if (!scriptFile) {
        std::cout << "FATAL: cannot open "
                  << scriptPath << '\n';
        return false;
    }

    loadJunctions(junctionFile, junctionPath);
    loadIncidents(incidentFile, incidentPath);

    executeScript(scriptFile);
    return true;
}

