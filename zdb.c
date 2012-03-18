/**
Copyright (C) 2012, Edward Chernysh
All rights reserved.

This file is part of Zenon. 
The Zenon Debug Reporter (ZDB).

This creates a program that checks for errors in a file during execution.
This program depends on ZENON and is distributed with it.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the author nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <EDWARD CHERNYSH> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

**/

// <begin preprocessing>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef _WIN32
       #include <windows.h>
       #include <direct.h>
       #include <conio.h>
       const char *zenon = "zenon.exe ";
#else
       #include <unistd.h>
       const char *zenon = "./zenon.out ";
#endif

#ifndef END
#define END (0)
#endif

// <end preprocessing>

int exec(char* cmd, char *filename, int line);   
int main(int argc, char *argv[]) {    
      if (argc < 2)
         printf("zdb: no input files.\n"); 
      else {
           FILE *script = fopen(argv[1], "r");  // open file for reading
           if (script == NULL)           // file does not exist
              printf("error: could not open file.\n");
           else {                
                char line[128];  // 128 is a SAFE line count...or is it?
                char *fpath, *result;   
                static int ln = 0;          
                while (fgets(line,sizeof line, script) != END)  { /* while streamer has not reached file end : */
                    strcat(fpath, zenon);      // copy (const char *)zenon to fpath.
                    strcat(fpath, line);       // add script to end of fpath...
                    if(!exec(fpath, argv[1], ln))  // execute and return result...
                            ln++;       // do nothing
                    else {
                         printf("There were errors in file.\n");
                         break;
                    }
                }
           }
           fclose(script);     // close file
      }      
      return END;              // return 0
}

int exec(char* cmd, char *filename, int line) {
    FILE* pipe = popen(cmd, "r");   // open file for reading
    if (!pipe) return (EOF);        // check for any errors,
    char buffer[80];                // buffer    
    int ret = 0;   
    char *p, *functionerror;
    while(!feof(pipe)) {    // stream
        if(fgets(buffer, sizeof buffer, pipe) != END) {                // copy to buffer[80]
                       p = strstr(buffer, "error");                    // check for errors                      
                       if (p != NULL) { 
                          /* AN ERROR WAS FOUND! */  
                          ret = EOF;
                          printf("error in file [%s] : line %d\n\t'%s' is not valid.\nRESULT: {\n%s}\n", filename, (line + 1), cmd, buffer);                                                    
                          break;
                       }  
           buffer[0] = NULL;                   
           free(buffer);
        }                
    }    
    pclose(pipe); // close pipe, 
    return ret;   // return (-1) if an error was found, 0 if not.
}
