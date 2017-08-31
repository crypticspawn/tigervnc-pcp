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

#ifndef __RFB_HOSTNAME_H__
#define __RFB_HOSTNAME_H__

#include <assert.h>
#include <stdlib.h>
#include <rdr/Exception.h>
#include <rfb/util.h>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iostream>

namespace rfb {

  char *strsep(char **stringp, const char *delim){
    char *s;
    const char *spanp;
    int c, sc;
    char *tok;
    if ((s = *stringp) == NULL)
      return (NULL);
    for (tok = s;;) {
      c = *s++;
      spanp = delim;
      do {
        if ((sc = *spanp++) == c) {
          if (c == 0)
            s = NULL;
          else
            s[-1] = 0;
          *stringp = s;
          return (tok);
        }
      } while (sc != 0);
    }
  }

  static void getHostAndPort(const char* hi, char** host, char** reflectorString, int* port, int basePort=5900) {
    char *section = NULL;
    int index = 0;
    char *hostString = strdup(hi);
    //reflectorString = NULL;
    while ((section = rfb::strsep(&hostString, ":")) != NULL) {
      if (index == 0) {
        *host = section;
      } else if (index == 2) {
        *port = atoi(section);
        if (*port == 0) {
          *port += basePort;
        }
      } else if (index == 3 && strcmp(section, "ID") != 0) {
        break;
      } else if (index == 4 ) {
        *reflectorString = section;
      }
      index++;
    }
    /*
    // hostname::port:ID:reflectorString
    std::vector<std::string> sections;
    std::string connectionString(hi);
    boost::algorithm::split(sections, connectionString, boost::is_any_of(":"));



    // Setting defaults
    *host = (char *)"localhost";  // I like 127.0.0.1 but old code had localhost, so making sure I stay consistent.
    *port = 0;

    // Override defaults
    for (unsigned i = 0; i < sections.size(); i++){
      if (i == 0) {
        *host = (char *)sections[i].c_str();
      } else if (i == 2) {
        *port = atoi((char *)sections[i].c_str());
        if (*port == 0) {
          *port += basePort;
        }
      } else if (i == 3 && sections[i].compare("ID") == 0) {
        if (i+1 < sections.size()) {
          *reflectorString = (char *)sections[i+1].c_str();
          i++;
        }
      }
    } */
  }

};

#endif // __RFB_HOSTNAME_H__
