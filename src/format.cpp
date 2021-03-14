#include "format.h"

#include <string>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  const int h = seconds / 3600;
  seconds %= 3600;
  const int m = seconds / 60;
  seconds %= 60;
  const int s = seconds;
  return std::to_string(h) + ":" + std::to_string(m) + ":" + std::to_string(s);
}