#ifndef __TTY__Y_TERMINAL__HH__
#define __TTY__Y_TERMINAL__HH__
#define _XOPEN_SOURCE 600
#include <fcntl.h>
#include <unistd.h>
#include <pty.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstdlib>
#include <string_view>
#include <utility>
#include <string>
#include "pty.h"
#define TERM  "xterm-256color"
#define SHELL getenv("SHELL")

int Spawn(struct PTY*);
int Send(struct PTY*, std::string_view);
// char* Recv(struct PTY*);
void Close(struct PTY*);
std::pair<std::string, int> Recv(struct PTY*);

#endif
