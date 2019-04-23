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

// -=- Logger_stdio.cxx - Logger instances for stderr and stdout

#include <rfb/Logger_stdio.h>
#include <sys/stat.h>

using namespace rfb;

static FILE* vncLog = 0;

bool rfb::initStdIOLoggers() {
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


  static Logger_StdIO logStdErr("stderr", vncLog);
  static Logger_StdIO logStdOut("stdout", vncLog);

  logStdErr.registerLogger();
  logStdOut.registerLogger();
  return true;
}
