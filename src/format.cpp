#include "format.h"

#include <iomanip>
#include <string>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  const int h = seconds / 3600;
  const int m = (seconds % 3600) / 60;
  const int s = (seconds % 3600 % 60);
  std::ostringstream elapsedTime;
  elapsedTime << std::setw(2) << std::setfill('0') << h << ":" << std::setw(2)
              << std::setfill('0') << m << ":" << std::setw(2)
              << std::setfill('0') << s;
  return elapsedTime.str();
}