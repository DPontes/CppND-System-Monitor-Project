#include "format.h"

#include <string>
#include <iomanip>

using std::string;
using std::setfill;
using std::setw;

// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  long minutes;
  long hours;
  std::ostringstream time;

  hours = seconds / 3600;
  minutes = (seconds % 3600) / 60;
  seconds = (seconds % 3600) % 60;

  time << std::setw(2) << std::setfill('0') << hours   << ":"
       << std::setw(2) << std::setfill('0') << minutes << ":"
       << std::setw(2) << std::setfill('0') << seconds;

  return time.str();
}
