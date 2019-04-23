/* Copyright (C) 2002-2005 RealVNC Ltd.  All Rights Reserved.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 */

// -=- LogWriter.cxx - client-side logging interface

#include <string.h>

#include <rfb/LogWriter.h>
#include <rfb/Configuration.h>
#include <rfb/util.h>
#include <stdlib.h>
#include <ctime>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>

rfb::LogParameter rfb::logParams;

using namespace rfb;


FILE* LogWriter::vncLog = NULL;
//RegKey LogWriter::pcpKey = RegKey();

LogWriter::LogWriter(const char* name) : m_name(name), m_level(0), m_log(0), m_next(log_writers) {
  /*pcpKey.openKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\WOW6432Node\\PCPitstop\\Nirvana"));
  TCharArray pcpLogPath;
  pcpLogPath.buf = pcpKey.getString("lfpath");
  if (strlen(pcpLogPath.buf) > 0) {
    time_t now = time(0);
    tm *ltime = localtime(&now);
    char pcpFullLogPath[1000];
    snprintf(pcpFullLogPath, 1000, "%s\\remote-desktop-%d.%02d.%02d.log", pcpLogPath.buf, 1970 + ltime->tm_year,
             1 + ltime->tm_mon, ltime->tm_mday);
    vncLog = fopen(pcpFullLogPath, "a");
  } else {
    vncLog = stderr;
  }*/

  time_t now = time(0);
  tm *ltime = localtime(&now);
  const int pathLimit = 260;
  char pcpFullLogPath[pathLimit];
  snprintf(pcpFullLogPath, pathLimit, "C:\\ProgramData\\PCPitstop\\remote-desktop-%d.%02d.%02d.log", 1970 + ltime->tm_year,
           1 + ltime->tm_mon, ltime->tm_mday);
  char pcpLogDirectory[30] = "C:\\ProgramData\\PCPitstop";
  struct stat info;
  if (stat(pcpLogDirectory, &info) == 0) {
    vncLog = fopen(pcpFullLogPath, "a");
  } else {
    vncLog = stderr;
  }

  log_writers = this;
}

LogWriter::~LogWriter() {
  // *** Should remove this logger here!
}

void LogWriter::setLog(Logger *logger) {
  m_log = logger;
}

void LogWriter::setLevel(int level) {
  m_level = level;
}

void
LogWriter::listLogWriters(int width) {
  // *** make this respect width...
  LogWriter* current = log_writers;
  fprintf(vncLog, "  ");
  while (current) {
    fprintf(vncLog, "%s", current->m_name);
    current = current->m_next;
    if (current) fprintf(vncLog, ", ");
  }
  fprintf(vncLog, "\n");
}

LogWriter* LogWriter::log_writers;

LogWriter*
LogWriter::getLogWriter(const char* name) {
  LogWriter* current = log_writers;
  while (current) {
    if (strcasecmp(name, current->m_name) == 0) return current;
      current = current->m_next;
    }
  return 0;
}

bool LogWriter::setLogParams(const char* params) {
  CharArray logwriterName, loggerName, logLevel;
  logwriterName.buf = strDup("*");
  loggerName.buf = strDup("file");
  logLevel.buf = strDup("30");

  fprintf(vncLog, "logWriterName: %s\n", logwriterName.buf);
  fprintf(vncLog, "loggerName: %s\n", loggerName.buf);
  fprintf(vncLog, "loggerLevel: %s\n", logLevel.buf);

  int level = atoi(logLevel.buf);
  Logger* logger = 0;
  if (strcmp("", loggerName.buf) != 0) {
    logger = Logger::getLogger(loggerName.buf);
    if (!logger) fprintf(vncLog,"no logger found! %s\n",loggerName.buf);
  }
  if (strcmp("*", logwriterName.buf) == 0) {
    LogWriter* current = log_writers;
    while (current) {
      current->setLog(logger);
      current->setLevel(level);
      current = current->m_next;
    }
    return true;
  } else {
    LogWriter* logwriter = getLogWriter(logwriterName.buf);
    if (!logwriter) {
      fprintf(vncLog,"no logwriter found! %s\n",logwriterName.buf);
    } else {
      logwriter->setLog(logger);
      logwriter->setLevel(level);
      return true;
    }
  }
  return false;
}


LogParameter::LogParameter()
  : StringParameter("Log",
    "Specifies which log output should be directed to "
    "which target logger, and the level of output to log. "
    "Format is <log>:<target>:<level>[, ...].",
    "") {
}

bool LogParameter::setParam(const char* v) {
  if (immutable) return true;
  LogWriter::setLogParams("*::0");
  StringParameter::setParam(v);
  CharArray logParam;
  CharArray params(getData());
  while (params.buf) {
    strSplit(params.buf, ',', &logParam.buf, &params.buf);
    if (strlen(logParam.buf) && !LogWriter::setLogParams(logParam.buf))
      return false;
  }
  return true;
}

void LogParameter::setDefault(const char* d) {
  def_value = d;
  setParam(def_value);
}
