#include "processor.h"
#include "linux_parser.h"

#include <cmath>

using std::abs;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  float idle = LinuxParser::IdleJiffies();
  float total = LinuxParser::ActiveJiffies();
  float d_idle = abs(idle - idle_p);
  float d_total = abs(total - total_p);

  if (total > 0.f) {
    idle_p = idle;
    total_p = total;
    //return active / (active + idle);
    return (d_total - d_idle) / d_total;
  }
  else
    return 0.f;
}