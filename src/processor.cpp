#include "processor.h"

#include <iostream>
#include <vector>

#include "linux_parser.h"

float Processor::Utilization() {
  float totalTime = LinuxParser::Jiffies();
  float activeTime = LinuxParser::ActiveJiffies();

  // prev_totalTime = totalTime;
  // prev_activeTime = activeTime;

  // long delta_totalTime = totalTime - prev_totalTime;
  // long delta_activeTime = activeTime - prev_activeTime;

  // return 1.0 * (delta_totalTime - delta_activeTime) / delta_totalTime;
  return 1.0 * (activeTime / totalTime);
}
