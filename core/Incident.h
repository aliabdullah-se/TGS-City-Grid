#pragma once
#include <string>

struct Incident {
    int id = 0;
    int junctionId = 0;
    std::string category;
    int severity = 0;
    int timestamp = 0;
    std::string status = "REPORTED";
};

inline bool higherPriority(const Incident* a, const Incident* b) {
    if (a->severity != b->severity)
        return a->severity > b->severity;

    if (a->timestamp != b->timestamp)
        return a->timestamp < b->timestamp;

    return a->id < b->id;
}
