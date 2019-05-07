//
// Created by CroxWire on 10/7/2017.
//

#ifndef TIGERVNC_NAMEPIPETHREAD_H
#define TIGERVNC_NAMEPIPETHREAD_H

#include <winvnc/VNCServerWin32.h>
#include <rfb_win32/TCharArray.h>
#include <rfb/Configuration.h>
#include <rfb/Threading.h>
#include <cstdio>

#define BUFFER_SIZE 600

namespace winvnc {
  class NamePipeThread : rfb::Thread {
  protected:
    HANDLE pipe;
  public:
    VNCServerWin32& server;
    NamePipeThread(VNCServerWin32& server);
    int primePipe();
    HANDLE getPipe();
    void run();

  };

  DWORD messageThread(LPVOID param);
}

#endif //TIGERVNC_NAMEPIPETHREAD_H
