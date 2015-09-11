// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "process.h"

#include <cstdlib>
#include <limits.h>
#include <signal.h>
#include <sstream>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

#include "printer.h"

#define CIN 0
#define COUT 1
#define CERR 2

#define READ 0
#define WRITE 1

namespace thilenius {
namespace external {
namespace vanguard {

Process Process::FromFork(std::function<int()> child_task) {
  Process process;
  process.child_task_ = child_task;
  process.pid_ = 0;
  return std::move(process);
}

bool Process::Execute() {
  // pipes for parent to write and read
  pipe(pipes_[CIN]);
  pipe(pipes_[COUT]);
  pipe(pipes_[CERR]);
  pid_ = fork();
  if (pid_ < 0) {
    return false;
  }
  if (pid_ == 0) {
    dup2(pipes_[CIN][READ], STDIN_FILENO);
    dup2(pipes_[COUT][WRITE], STDOUT_FILENO);
    dup2(pipes_[CERR][WRITE], STDERR_FILENO);
    exit(child_task_());
  } else {
    close(pipes_[CIN][READ]);
    close(pipes_[COUT][WRITE]);
    close(pipes_[CERR][WRITE]);
    start_time_ = std::chrono::system_clock::now();
    return true;
  }
}

int Process::Wait(int timeout_ms) {
  // This is a simple spin-wait because i'm too lazy to do anything better
  bool warning_issued = false;
  while (true) {
    int status;
    int rc = waitpid(pid_, &status, WNOHANG);
    if (rc == -1) {
      // Assume exit failure. I guess...
      return -2;
    }
    if (WIFEXITED(status)) {
      return WEXITSTATUS(status);
    }
    // Warning time
    if (!warning_issued &&
        (start_time_ + std::chrono::milliseconds(1000)) <=
            std::chrono::system_clock::now()) {
      std::cout << Red << "A child process is taking an unusually long time. "
                          "Killing it in a few seconds..." << White
                << std::endl;
      warning_issued = true;
    }
    // Terminate time (timeout)
    if ((start_time_ + std::chrono::milliseconds(timeout_ms)) <=
        std::chrono::system_clock::now()) {
      kill(pid_, SIGKILL);
      waitpid(pid_, 0, 0);
      return -1;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
}

bool Process::WriteCin(const std::string& value) {
  return write(pipes_[CIN][WRITE], value.c_str(), value.length()) ==
         value.length();
}

std::string Process::GetOut() {
  std::stringstream string_stream;
  char buffer[1024];
  while (true) {
    int count = read(pipes_[COUT][READ], buffer, sizeof(buffer));
    if (count <= 0) {
      break;
    }
    string_stream.write(buffer, count);
  }
  return string_stream.str();
}

std::string Process::GetErr() {
  std::stringstream string_stream;
  char buffer[1024];
  while (true) {
    int count = read(pipes_[CERR][READ], buffer, sizeof(buffer));
    if (count <= 0) {
      break;
    }
    string_stream.write(buffer, count);
  }
  return string_stream.str();
}

}  // namespace vanguard
}  // namespace external
}  // namespace thilenius
