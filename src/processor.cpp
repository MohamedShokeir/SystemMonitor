#include "processor.h"

#include <cmath>

#include "linux_parser.h"

using std::abs;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  const float idle = LinuxParser::IdleJiffies();
  const float active = LinuxParser::ActiveJiffies();
  const float d_idle = abs(idle - idle_p);
  const float d_active = abs(active - active_p);

  if (active + idle > 0.f) {
    idle_p = idle;
    active_p = active;
    // return active / (active + idle);
    return d_active / (d_active + d_idle);
  } else
    return 0.f;
}