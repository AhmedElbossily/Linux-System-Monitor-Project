#include "linux_parser.h"
#include <dirent.h>
#include <unistd.h>
#include <iterator>
#include <string>
#include <vector>
#include "Util.h"
#include "iostream"

using std::stof;
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
        linestream >> key >> value;
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
    linestream >> os >> kernel >> version;
  }
  return version;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string x = "MemTotal:";
  string mem_free = "MemFree:";
  string mem_buffers = "Buffers:";

  string value;
  // int result;
  string path = kProcDirectory + kMeminfoFilename;
  std::ifstream stream;
  Util::getStream(path, stream);
  float total_memory = 0;
  float free_memory = 0;
  float buffers = 0;
  while (std::getline(stream, line)) {
    if (total_memory != 0 && free_memory != 0) break;
    if (line.compare(0, x.size(), x) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      total_memory = stof(values[1]);
    }
    if (line.compare(0, mem_free.size(), mem_free) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      free_memory = stof(values[1]);
    }
    if (line.compare(0, mem_buffers.size(), mem_buffers) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      buffers = stof(values[1]);
    }
  }

  return float(1.0 - (free_memory / (total_memory - buffers)));
  // return float((total_memory -free_memory )/ total_memory);
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string path = kProcDirectory + kUptimeFilename;
  std::ifstream stream;
  Util::getStream(path, stream);
  string line;
  std::getline(stream, line);
  std::istringstream buffer(line);
  std::istream_iterator<string> begin(buffer), end;
  vector<string> line_content(begin, end);
  return stoi(line_content[0]);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> values = LinuxParser::CpuUtilization();
  return (stof(values[kUser_]) + stof(values[kNice_]) + stof(values[kSystem_]) +
          // stof(values[kIdle_]) +
          stof(values[kSoftIRQ_]) + stof(values[kSteal_]) +
          stof(values[kGuest_]) + stof(values[kGuestNice_]));
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> values = LinuxParser::CpuUtilization();
  return (stof(values[kIdle_]) + stof(values[kIOwait_]));
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization(string coreNumber) {
  string line;
  string name = "cpu" + coreNumber;
  std::ifstream stream;
  string path = kProcDirectory + kStatFilename;
  Util::getStream(path, stream);
  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::istringstream buffer(line);
      std::istream_iterator<string> begin(buffer), end;
      vector<string> line_content(begin, end);
      return line_content;
    }
  }
  return (vector<string>());
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string path = kProcDirectory + kStatFilename;
  string line;
  int result = 0;
  string name = "processes";
  std::ifstream stream;
  Util::getStream(path, stream);
  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::istringstream buffer(line);
      std::istream_iterator<string> begin(buffer), end;
      vector<string> values(begin, end);
      result += stoi(values[1]);
      break;
    }
  }
  return result;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string path = kProcDirectory + kStatFilename;
  string line;
  int result = 0;
  string name = "procs_running";
  std::ifstream stream;
  Util::getStream(path, stream);
  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::istringstream buffer(line);
      std::istream_iterator<string> begin(buffer), end;
      vector<string> values(begin, end);
      result += stoi(values[1]);
      break;
    }
  }
  return result;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string path = kProcDirectory + to_string(pid) + kCmdlineFilename;
  std::ifstream cmd_stream;
  Util::getStream(path, cmd_stream);
  string line;
  std::getline(cmd_stream, line);  // Error handling in Util class
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
/*string LinuxParser::Ram(int pid) {
  float vm_size;
  string name = "VmData";
  string path = kProcDirectory + to_string(pid) + kStatusFilename;
  std::ifstream stream;
  Util::getStream(path, stream);  // Error handling inside Util class
  string line;
  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      // Converting line to string stream and then vector
      // The specific line looks like this: "VmData: 88408 kB"
      std::istringstream buffer(line);
      std::istream_iterator<string> beginning(buffer), end;
      std::vector<string> line_values(beginning, end);
      string data_kilobytes = line_values[1];
      vm_size = stof(data_kilobytes) / float(1024);  // convert
      break;
    }
  }
  return to_string(vm_size).substr(0, 5);
}
*/
string LinuxParser::Ram(int pid) {
  std::string name;
  std::string size;
  std::string ram = "VmSize:";
   int in_megaByte = 0;
  std::ifstream file(kProcDirectory + to_string(pid) + kStatusFilename);
  if (file) {
    while (file >> name >> size) {
      if (name.compare(ram) == 0) {
         in_megaByte = static_cast<int>(stoi(size) / 1000);
      }
    }
  }
   return to_string(in_megaByte);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  // Selecting UID for the current user
  string path = kProcDirectory + to_string(pid) + kStatusFilename;
  string name = "Uid:";
  std::ifstream stream;
  Util::getStream(path, stream);
  string line;
  string uid_result;
  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::istringstream buffer(line);
      std::istream_iterator<string> begin(buffer), end;
      std::vector<string> line_content(begin, end);
      uid_result = line_content[1];
      break;
    }
  }
  return uid_result;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string result, line;
  string passwd_path = "/etc/passwd";
  std::ifstream passwd_stream;
  Util::getStream(passwd_path, passwd_stream);
  string name = ("x:" + LinuxParser::Uid(pid));
  // Searching for name of the user with selected UID, for example root:x:0
  while (std::getline(passwd_stream, line)) {
    if (line.find(name) != string::npos) {
      int position = line.find(":");
      result = line.substr(0, position);
      return result.substr(0, 5);  // for better display
    }
  }
  return "";
}
// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string path = kProcDirectory + to_string(pid) + kStatFilename;
  std::ifstream stream;
  Util::getStream(path, stream);
  string line;
  std::getline(stream, line);
  std::istringstream buffer(line);
  std::istream_iterator<string> begin(buffer), end;
  vector<string> line_content(begin, end);
  // reading system clock ticks from the host machine
  return (LinuxParser::UpTime() -
          float(stof(line_content[21]) / sysconf(_SC_CLK_TCK)));
}

// This method reads from pid/stat file
// Converts the times in the line to float
/*float LinuxParser::getCpuPercent(int pid) {
  std::string path = kProcDirectory + to_string(pid) + kStatFilename;
  std::ifstream stream;
  Util::getStream(path, stream);
  string line;
  std::getline(stream, line);  // file contains only one line
  std::istringstream buffer(line);
  std::istream_iterator<string> beginning(buffer), end;
  std::vector<string> line_content(beginning, end);
  float utime = LinuxParser::UpTime(pid);
  float stime = stof(line_content[14]);
  float cutime = stof(line_content[15]);
  float cstime = stof(line_content[16]);
  float starttime = stof(line_content[21]);
  float uptime = LinuxParser::UpTime();
  float freq = sysconf(_SC_CLK_TCK);
  float total_time = utime + stime + cutime + cstime;
  float seconds = uptime - (starttime / freq);
  float result = 100.0 * ((total_time / freq) / seconds);
  return (result);
}*/

float LinuxParser::getCpuPercent(int pid) {
    string path = kProcDirectory + to_string(pid) + kStatFilename;

      std::ifstream stream(path);
      string line;
      std::getline(stream, line);

      std::istringstream buffer(line);
    std::istream_iterator<string> begin(buffer), end;
    vector<string> line_content(begin, end);

      float uptime = LinuxParser::UpTime();
      float utime = stof(line_content[13]);
      float stime = stof(line_content[14]);
      float cutime = stof(line_content[15]);
      float cstime = stof(line_content[16]);
      float starttime = stof(line_content[21]);

      float heartz = sysconf(_SC_CLK_TCK);
      float totaltime = utime + stime + cutime + cstime;
      float seconds = uptime - (starttime / heartz);


      /*std::cout<< totaltime << std::endl;
      std::cout<< seconds << std::endl;
      std::cout << heartz << std::endl;
      throw 1;*/

      return 100 * ((totaltime / heartz) / seconds);
}