#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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

float LinuxParser::MemoryUtilization() {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  float memTotal = -1;
  float memFree = -1;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          memTotal = std::stof(value);
        } else if (key == "MemFree:") {
          memFree = std::stof(value);
        }
      }
    }
  }
  return memTotal >= 0 && memFree >= 0 ? (memTotal - memFree) / memTotal : 0;
}

long LinuxParser::UpTime() {
  string totalTime, idleTime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> totalTime >> idleTime;
  }
  return std::stol(totalTime);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<float> LinuxParser::CpuUtilization() {
  string line, key, user, nice, sys, idle, iowait, irq, softirq, steal, guest,
      guest_nice;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  vector<float> result;
  if (filestream.is_open()) {
    std::getline(filestream, line);  // skip first line
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> user >> nice >> sys >> idle >> iowait >>
             irq >> softirq >> steal >> guest >> guest_nice) {
        if (key.rfind("cpu", 0) == 0) {
          float totalNonIdle = std::stof(user) + std::stof(nice) +
                               std::stof(sys) + std::stof(irq) +
                               std::stof(softirq) + std::stof(steal);
          float totalIdle = std::stof(idle) + std::stof(iowait);
          float total = totalNonIdle + totalIdle;
          float percentage = (total - totalIdle) / total;
          result.push_back(percentage);
        }
      }
    }
  }
  return result;
}

int LinuxParser::TotalProcesses() {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return std::stoi(value);
        }
      }
    }
  }
  return -1;  // error
}

int LinuxParser::RunningProcesses() {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return std::stoi(value);
        }
      }
    }
  }

  return -1;  // error
}

string LinuxParser::Command(int pid) {
  string value;
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    return line;
  }
  return "error";
}

float LinuxParser::Ram(int pid) {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          return std::stof(value);
        }
      }
    }
  }
  return 0;
}

string LinuxParser::Uid(int pid) {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return "-1";
}

std::vector<std::string> LinuxParser::Split(std::string strToSplit,
                                            char delimeter) {
  std::stringstream ss(strToSplit);
  std::string item;
  std::vector<std::string> splittedStrings;
  while (std::getline(ss, item, delimeter)) {
    splittedStrings.push_back(item);
  }
  return splittedStrings;
}

string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  if (uid == "-1") return "uid=-1";

  string line, value;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      std::vector<string> vals = Split(line, ':');
      if (vals[2] == uid) return vals[0];
    }
  }
  return uid;
}

long LinuxParser::UpTime(int pid) {
  string value;
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    std::vector<string> vals;
    while (linestream >> value) vals.push_back(value);

    return std::stol(vals[21]) / sysconf(_SC_CLK_TCK);
  }
  return -1;  // error
}

float LinuxParser::ProcessUtilization(int pid) {
  string value;
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    string vals[52];
    for (int i = 0; linestream >> value; i++) vals[i] = value;

    float total = std::stol(vals[13]) +    // user time
                  std::stol(vals[14]) +    // kernel time
                  std::stol(vals[15]) +    // cutime - child utime.
                  std::stol(vals[16]);     // cstime  - child stime.
    total = total / sysconf(_SC_CLK_TCK);  // covert to secs

    long uptime = UpTime();
    float start_time = (std::stol(vals[21]) / sysconf(_SC_CLK_TCK));
    float seconds = uptime - start_time;
    return seconds > 0 ? 100 * (total / seconds) : 0;
  }

  return -1;  // error
}
