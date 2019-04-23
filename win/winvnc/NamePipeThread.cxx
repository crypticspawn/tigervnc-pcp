//
// Created by CroxWire on 10/7/2017.
//


#include <winvnc/NamePipeThread.h>
#include <rfb/LogWriter.h>

using namespace rfb;

static LogWriter vlog("NamedPipeThread");

winvnc::NamePipeThread::NamePipeThread(VNCServerWin32 &server) : Thread("NamePipeThread"), server(server){
  start();
}


DWORD winvnc::messageThread(LPVOID param) {
  HANDLE heap = GetProcessHeap();
  TCHAR* request = (TCHAR*) HeapAlloc(heap, 0, BUFFER_SIZE * sizeof(TCHAR));

  DWORD bytesRead = 0;
  BOOL success = FALSE;
  HANDLE pipe = NULL;

  if (param == NULL) {
    vlog.debug("pipe server failed");
    return (DWORD) -1;
  }

  if (request == NULL) {
    vlog.debug("was unable to allocate heap.");
    return (DWORD) -1;
  }

  NamePipeThread* thread = (NamePipeThread*) param;
  pipe = thread->getPipe();

  while(1) {
    success = ReadFile(pipe, request, BUFFER_SIZE, &bytesRead, NULL);

    if (!success || bytesRead == 0) {
      if (GetLastError() == ERROR_BROKEN_PIPE) {
        vlog.debug("client disconnected: %ld", GetLastError());
      } else {
        vlog.debug("read file failed: %ld", GetLastError());
      }
      break;
    }
  }

  // Process the message
  vlog.debug("Message Received: %s", request);

  char* command;
  char* data;
  strSplit(request,'|', &command, &data);

  vlog.debug("Command: %s", command);
  vlog.debug("Data: %s", data);

  if (strncasecmp(command, "AddClient", 9) == 0) {
    thread->server.addNewClient(data);
  }

  FlushFileBuffers(pipe);
  DisconnectNamedPipe(pipe);
  CloseHandle(pipe);

  HeapFree(heap, 0, request);

  vlog.debug("exiting WorkerThread");
  return (DWORD)1;
}

HANDLE winvnc::NamePipeThread::getPipe() {
  return this->pipe;
}


void winvnc::NamePipeThread::run() {
  // Name Pipe Code goes here
  BOOL fConnected = FALSE;
  HANDLE workerThread = NULL;
  pipe = INVALID_HANDLE_VALUE;
  DWORD workerThreadId = 0;
  vlog.debug("before making the named pipe");
  TCHAR namePipeName[] = TEXT("\\\\.\\pipe\\VNCNamedPipe");
  while(1) {
    vlog.debug("waiting for client connection [PipeName: %s]", namePipeName);

    PSECURITY_DESCRIPTOR psd = NULL;
    BYTE  sd[SECURITY_DESCRIPTOR_MIN_LENGTH];
    psd = (PSECURITY_DESCRIPTOR)sd;
    InitializeSecurityDescriptor(psd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(psd, TRUE, (PACL)NULL, FALSE);
    SECURITY_ATTRIBUTES sa = {sizeof(sa), psd, FALSE};

    pipe = CreateNamedPipe(namePipeName, PIPE_ACCESS_INBOUND, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES,
            // NULL, BUFFER_SIZE, 0, &sa);
                          NULL, BUFFER_SIZE, 0, NULL);

    if (pipe == INVALID_HANDLE_VALUE) {
      vlog.debug("invalid handle value");
      return;
    }

    fConnected = ConnectNamedPipe(pipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (fConnected) {
      vlog.debug("We are now connected.");

      workerThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) messageThread, this, 0, &workerThreadId);

      if (workerThreadId == NULL) {
        vlog.debug("Was unable to create a worker thread: %ld", GetLastError());
        return;
      }
    } else {
      CloseHandle(workerThread);
    }
  }
}