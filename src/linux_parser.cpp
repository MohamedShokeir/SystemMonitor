#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

// this filesystem works with my compiler:
#include <filesystem>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;

  // implementation wo filesystem:
  //------------------------------
  /*
    DIR* directory = opendir(kProcDirectory.c_str());
    struct dirent* file;
    while ((file = readdir(directory)) != nullptr) {
      // Is this a directory?
      if (file->d_type == DT_DIR) {
        // Is every character of the name a digit?
        string filename(file->d_name);
        if (std::all_of(filename.begin(), filename.end(), isdigit)) {
          int pid = stoi(filename);
          pids.push_back(pid);
        }
      }
    }
    closedir(directory);
    return pids;
  }
  */

  // implementation with filesystem:
  //--------------------------------
  for (const auto& dir_entry :
       std::filesystem::directory_iterator((kProcDirectory.c_str()))) {
    // Is this a directory?
    if (std::filesystem::is_directory(dir_entry)) {
      string filename = dir_entry.path().filename().string();
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string kB;
  float Mem{0.f};
  float MemTotal{0.f};
  float MemFree{0.f};
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> Mem >> kB) {
        if (key == "MemTotal") {
          MemTotal = Mem;
        } else if (key == "MemFree") {
          MemFree = Mem;
        }
      }
    }
  }
  return (MemTotal - MemFree) / MemTotal;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime{0}, v2{0};
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> v2;
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::UpTime() * sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  vector<string> all_values;
  string key, line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> key) {
      all_values.push_back(key);
    }
  }
  return (std::stoi(all_values[13]) + std::stoi(all_values[16]));
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  // I don't know why this implementation doesn't work
  // I tried to stock all values in a vector and add them up
  // to get total Jiffies. Any ideas?
  //---------------------------------------------------------
  // vector<int> all_values;
  // int value;
  // string key, line;
  // std::ifstream filestream(kProcDirectory + kStatFilename);
  // if (filestream.is_open()) {
  //   while (std::getline(filestream, line)) {
  //     std::istringstream linestream(line);
  //     while (linestream >> key) {
  //       if (key == "cpu") {
  //         all_values.push_back(value);
  //       }
  //     }
  //   }
  // }
  // return (std::accumulate(all_values.begin(), all_values.end(),
  //                         decltype(all_values)::value_type(0)));

  // This code works:
  long active{0};
  string user, nice, system, irq, softirq, steal, other, key, line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> key) {
      if (key == "cpu") {
        for (int i = 0; i <= 9; ++i) {
          if (i == CPUStates::kUser_)
            linestream >> user;
          else if (i == CPUStates::kNice_)
            linestream >> nice;
          else if (i == CPUStates::kSystem_)
            linestream >> system;
          else if (i == CPUStates::kIRQ_)
            linestream >> irq;
          else if (i == CPUStates::kSoftIRQ_)
            linestream >> softirq;
          else if (i == CPUStates::kSteal_)
            linestream >> steal;
          else
            linestream >> other;
        }
      }
    }
    active = stoi(user) + stoi(nice) + stoi(system) + stoi(irq) +
             stoi(softirq) + stoi(steal);
  }
  return active;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  // Same as ActiveJiffies(), I don't know why this implementation
  // doesn't work. Any ideas?
  //---------------------------------------------------------
  /*
    vector<int> all_values;
    int value;
    string key, line;
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
        std::istringstream linestream(line);
        while (linestream >> key) {
          if (key == "cpu") {
            while (linestream >> value)
            all_values.push_back(value);
          }
        }
      }
    }
    return (all_values.at(3) + all_values.at(4));
}
*/
  // This code works:
  long inactive{0};
  string idle, iowait, other, key, line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> key) {
      if (key == "cpu") {
        for (int i = 0; i <= 9; ++i) {
          if (i == CPUStates::kIdle_)
            linestream >> idle;
          else if (i == CPUStates::kIOwait_)
            linestream >> iowait;
          else
            linestream >> other;
        }
      }
    }
    inactive = stoi(idle) + stoi(iowait);
  }
  return inactive;
}

// TODO: Read and return CPU utilization
float LinuxParser::CpuUtilization(int pid) {
  float cpuUtil{0.f};
  float uptime = LinuxParser::UpTime();

  vector<string> all_values;
  string key, line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);

    std::istream_iterator<string> beginning(linestream), end;
    std::vector<string> line_content(beginning, end);
    float utime = stof(line_content[13]);
    float stime = stof(line_content[14]);
    float cutime = stof(line_content[15]);
    float cstime = stof(line_content[16]);
    float starttime = LinuxParser::UpTime(pid);
    float freq = sysconf(_SC_CLK_TCK);
    float total_time = utime + stime + cutime + cstime;
    float seconds = uptime - starttime;
    cpuUtil = (total_time / freq) / seconds;
  }
  return cpuUtil;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int value{0};
  string key, line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") return value;
      }
    }
  }
  return value;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int value{0};
  string key, line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") return value;
      }
    }
  }
  return value;
}

// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }
  return line;
}

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string key, value, line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") {
          if (stoi(value))
            return to_string(stoi(value) / 1000);
          else
            return to_string(0);
        }
      }
    }
  }
  return to_string(stoi(value) / 1000);
}

// TODO: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string key, value, line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid") {
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string user, x, id, line;
  string uid = LinuxParser::Uid(pid);

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> x >> id) {
        if (uid == id) {
          return user;
        }
      }
    }
  }
  return user;
}

// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  vector<string> all_values;
  string uptime, line, other;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i <= 21; ++i) {
      if (i == CPUStates::kStarttime_) {
        linestream >> uptime;
      } else {
        linestream >> other;
      }
    }
  }
  return stoi(uptime) / sysconf(_SC_CLK_TCK);
}
