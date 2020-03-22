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
  user_ = LinuxParser::User(pid);
  ram_ = LinuxParser::Ram(pid_);
  cpu_util = LinuxParser::ProcessUtilization(pid_);
  uptime_ = LinuxParser::UpTime(pid_);
}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() { return cpu_util; }

// TODO: Return the command that generated this process
string Process::Command() { return string(); }

string Process::Ram() { return std::to_string(ram_); }

string Process::User() { return user_; }

long int Process::UpTime() { return uptime_; }

bool Process::operator<(Process const& a) const { return ram_ > a.ram_; }