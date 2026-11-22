#include "CityGrid.h"
#include <iostream>
#include <sstream>

void CityGrid::printJunction(const Junction& junction) {
    std::cout << "JUNCTION " << junction.id << ' '
              << junction.name << ' ' << junction.type << '\n';
}

void CityGrid::printIncident(const Incident& incident) {
    std::cout << "INCIDENT " << incident.id
              << " junction=" << incident.junctionId
              << " category=" << incident.category
              << " severity=" << incident.severity
              << " time=" << incident.timestamp
              << " status=" << incident.status << '\n';
}

void CityGrid::closeIncident(int id) {
    Incident* incident = incidentIndex.find(id);

    if (incident->status == "PENDING")
        pending.removeById(id);

    // Remove references before the BST frees the object.
    incidentIndex.remove(id);
    incidents.remove(id);
}

void CityGrid::executeScript(std::ifstream& script) {
    std::string line;
    int lineNumber = 0;

    while (std::getline(script, line)) {
        ++lineNumber;

        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        line = trim(line);

        if (line.empty() || line[0] == '#')
            continue;

        std::cout << "> " << line << '\n';
        executeCommand(line, lineNumber);
    }
}

void CityGrid::executeCommand(
    const std::string& line, int lineNumber) {

    std::istringstream input(line);
    std::string word[6];
    std::string token;
    int wordCount = 0;

    while (input >> token) {
        if (wordCount < 6)
            word[wordCount] = token;
        ++wordCount;
    }

    const std::string& command = word[0];
    int expected = -1;

    if (command == "CITY_ADD")
        expected = 3;
    else if (command == "CITY_FIND")
        expected = 1;
    else if (command == "CITY_LIST" ||
             command == "CITY_PREORDER")
        expected = 0;
    else if (command == "INC_REPORT")
        expected = 5;
    else if (command == "INC_FIND" ||
             command == "INC_CLOSE" ||
             command == "REQUEST")
        expected = 1;
    else if (command == "INC_LIST" ||
             command == "INC_PREORDER" ||
             command == "PRIORITY_NEXT" ||
             command == "PRIORITY_SERVE" ||
             command == "PRIORITY_COUNT" ||
             command == "HASH_STATS")
        expected = 0;

    std::string reason;

    if (expected < 0)
        reason = "unknown command " + command;
    else if (wordCount != expected + 1)
        reason = "wrong argument count";

    if (!reason.empty()) {
        std::cout << "ERROR line " << lineNumber
                  << ": " << reason << '\n';
        return;
    }

    int id = 0;
    int junctionId = 0;
    int severity = 0;
    int timestamp = 0;

    if (command == "CITY_ADD") {

        if (!number(word[1], 1, 999999999, id))
            reason = "invalid junction ID";
        else if (word[2].find('|') != std::string::npos)
            reason = "invalid name";
        else if (!junctionType(word[3]))
            reason = "invalid type";
        else if (junctions.find(id) != nullptr)
            reason = "duplicate junction ID";

        if (reason.empty()) {
            Junction junction;
            junction.id = id;
            junction.name = word[2];
            junction.type = word[3];

            junctions.insert(id, junction);
            std::cout << "JUNCTION ADDED " << id << '\n';
        }

    } else if (command == "CITY_FIND") {

        if (!number(word[1], 1, 999999999, id)) {
            reason = "invalid junction ID";
        } else {
            const Junction* junction = junctions.find(id);

            if (junction == nullptr)
                reason = "junction " + std::to_string(id)
                         + " not found";
            else
                printJunction(*junction);
        }

    } else if (command == "CITY_LIST") {

        std::cout << "JUNCTIONS " << junctions.size() << '\n';

        junctions.inOrder(
            [](const int&, const Junction& junction) {
                printJunction(junction);
            }
        );

    } else if (command == "CITY_PREORDER") {

        std::cout << "JUNCTION PREORDER";

        if (junctions.size() == 0) {
            std::cout << " EMPTY";
        } else {
            junctions.preOrder(
                [](const int& key, const Junction&) {
                    std::cout << ' ' << key;
                }
            );
        }

        std::cout << '\n';

    } else if (command == "INC_REPORT") {

        if (!number(word[1], 1, 999999999, id))
            reason = "invalid incident ID";
        else if (!number(word[2], 1, 999999999,
                         junctionId))
            reason = "invalid junction ID";
        else if (!incidentCategory(word[3]))
            reason = "invalid category";
        else if (!number(word[4], 1, 5, severity))
            reason = "invalid severity";
        else if (!number(word[5], 0, 999999999,
                         timestamp))
            reason = "invalid timestamp";
        else if (incidentIndex.find(id) != nullptr)
            reason = "duplicate incident ID";
        else if (junctions.find(junctionId) == nullptr)
            reason = "junction " + std::to_string(junctionId)
                     + " not found";

        if (reason.empty()) {
            Incident* incident = new Incident;

            incident->id = id;
            incident->junctionId = junctionId;
            incident->category = word[3];
            incident->severity = severity;
            incident->timestamp = timestamp;
            incident->status = "REPORTED";

            incidents.insert(id, incident);
            incidentIndex.insert(id, incident);

            std::cout << "INCIDENT REPORTED " << id << '\n';
        }

    } else if (command == "INC_FIND") {

        if (!number(word[1], 1, 999999999, id)) {
            reason = "invalid incident ID";
        } else {
            Incident* incident = incidentIndex.find(id);

            if (incident == nullptr)
                reason = "incident " + std::to_string(id)
                         + " not found";
            else
                printIncident(*incident);
        }

    } else if (command == "INC_LIST") {

        std::cout << "INCIDENTS " << incidents.size() << '\n';

        incidents.inOrder(
            [](const int&, Incident* incident) {
                printIncident(*incident);
            }
        );

    } else if (command == "INC_PREORDER") {

        std::cout << "INCIDENT PREORDER";

        if (incidents.size() == 0) {
            std::cout << " EMPTY";
        } else {
            incidents.preOrder(
                [](const int& key, Incident*) {
                    std::cout << ' ' << key;
                }
            );
        }

        std::cout << '\n';

    } else if (command == "INC_CLOSE" ||
               command == "REQUEST") {

        if (!number(word[1], 1, 999999999, id)) {
            reason = "invalid incident ID";
        } else {
            Incident* incident = incidentIndex.find(id);

            if (incident == nullptr) {
                reason = "incident " + std::to_string(id)
                         + " not found";
            } else if (command == "INC_CLOSE") {
                closeIncident(id);

                std::cout << "INCIDENT CLOSED "
                          << id << '\n';
            } else if (incident->status == "PENDING") {
                reason = "incident " + std::to_string(id)
                         + " is already PENDING";
            } else {
                incident->status = "PENDING";
                pending.insert(incident);

                std::cout << "INCIDENT PENDING "
                          << id << '\n';
            }
        }

    } else if (command == "PRIORITY_NEXT") {

        Incident* incident = pending.peek();

        if (incident == nullptr)
            std::cout << "NO PENDING INCIDENTS\n";
        else
            printIncident(*incident);

    } else if (command == "PRIORITY_SERVE") {

        Incident* incident = pending.extractMax();

        if (incident == nullptr) {
            std::cout << "NO PENDING INCIDENTS\n";
        } else {
            int servedId = incident->id;

            incidentIndex.remove(servedId);
            incidents.remove(servedId);

            std::cout << "INCIDENT SERVED "
                      << servedId << '\n';
        }

    } else if (command == "PRIORITY_COUNT") {

        std::cout << "PENDING " << pending.size() << '\n';

    } else if (command == "HASH_STATS") {

        std::cout << "HASH capacity="
                  << incidentIndex.capacity()
                  << " size="
                  << incidentIndex.size() << '\n';
    }

    if (!reason.empty()) {
        std::cout << "ERROR line " << lineNumber
                  << ": " << reason << '\n';
    }
}
