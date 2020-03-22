#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {
  user_ = LinuxParser::User(pid_);
  ram_ = (int)LinuxParser::Ram(pid_) / 1000;  // KBs -> MBs
  cpu_util_ = LinuxParser::ProcessUtilization(pid_);
  uptime_ = LinuxParser::UpTime(pid_);
  command_ = LinuxParser::Command(pid_);
}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() { return cpu_util_; }

string Process::Command() { return command_; }

string Process::Ram() { return std::to_string(ram_); }

string Process::User() { return user_; }

long int Process::UpTime() { return uptime_; }

bool Process::operator<(Process const& a) const { return ram_ < a.ram_; }