#include "format.h"

#include <string>

using std::string;
using std::to_string;

// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    int minutes;
    int hours;
    string time;

    hours   = seconds / 3600;
    minutes = (seconds % 3600) / 60;
    seconds = (seconds % 3600) % 60;

    time = to_string(hours) + ":" + to_string(minutes) + ":" + to_string(seconds);

    return time;
}
