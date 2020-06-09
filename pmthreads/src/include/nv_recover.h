/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef __NV_RECOVER__
#define __NV_RECOVER__

// open()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>

// opendir()
#include <unistd.h>
#include <vector>

// read file
#include <iostream>
#include <fstream>

// std map<>
#include <string>
#include <map>

// readlink
#include <unistd.h>

// rand()
#include <time.h>

enum RECOVER_STATUS {
    RECOVER_SUCCESS,
    RECOVER_FAIL
};

/* NVthreads API */
extern "C"
{
    bool isCrashed(void);
    //unsigned long nvrecover(void *dest, size_t size, char *name);
    //void* nvmalloc(size_t size, char *name);
    //void nvcheckpoint(void);
}

class nvrecovery {

public:
    nvrecovery() {
	}
    ~nvrecovery() {
    }

    int nvid;
    int nvlib_linenum;
    char nvlib_crash[FILENAME_MAX];
    char logPath[FILENAME_MAX];
    char memLogPath[FILENAME_MAX];
    bool crashed;
    bool _initialized;
		
	void initialize(){
	
	}

    int LookupCrashRecord(void) {
        char exe[FILENAME_MAX];
        int ret = 0;
        int line = 0;

        // Get absolute exe name
        ret = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
        if ( ret == -1 ) {
            fprintf(stderr, "readlink() error when lookup nvid.\n");
            abort();
        }
        exe[ret] = 0;
        lprintf("exe: %s\n", exe);

        // Lookup in crash record file at /tmp/nvlib.crash
        FILE *fp = fopen(nvlib_crash, "r+");

        // nvlib.crash doesn't exist, create a new one.
        if ( fp == NULL ) {
            lprintf("%s doesn't exist, create it now\n", nvlib_crash);
            fp = fopen(nvlib_crash, "w");
        }
        // Lookup absolute exe name in nvlib.crash
        else {
            char buf[FILENAME_MAX];
            while (fgets(buf, FILENAME_MAX, fp) != NULL) {
                char *token;
                token = strtok(buf, ", \n");
                // Found record, program has crashed before
                if ( strcmp(token, exe) == 0 ) {
                    crashed = true;
                    token = strtok(NULL, ", \n");
                    // Read old NVID
                    nvid = atoi(token);
                    break;
                }
                line++;
            }
        }
        // Record line number for the entry
        nvlib_linenum = line;

        if ( crashed ) {
            lprintf("%s CRASHED before, Found nvid at line %d in nvlib.crash: %d\n", exe, nvlib_linenum, nvid);
        } else {
            // Generate a new NVID
            nvid = rand();
            lprintf("%s did not crash in previous execution\n", exe);
            fprintf(fp, "%s, %d\n", exe, nvid);
            line = -1;
        }
        fclose(fp);

        // Set up log path after we get nvid
        if ( log_dest == SSD ) {
            sprintf(logPath, "/mnt/ssd2/nvthreads/%d/", nvid);
            sprintf(memLogPath, "/mnt/ssd2/nvthreads/%d/logs/", nvid);
        } else if ( log_dest == NVM_RAMDISK ) {
            sprintf(logPath, "/mnt/ramdisk/nvthreads/%d/", nvid);
            sprintf(memLogPath, "/mnt/ramdisk/nvthreads/%d/logs/", nvid);
        }
        lprintf("Assigned NVID: %d at line %d to current process\n", nvid, nvlib_linenum);
        return line;
    }

    // Set crashed flag to true if we can find a crash record in nvlib.crash
    void SetCrashedFlag(void) {
        bool ret = false;
        int line;

        // Lookup the line number that contains current exe.
        // line >= 0 means there exists an entry in nvlib.crash
        line = LookupCrashRecord();
        if ( line >= 0 ) {
            lprintf("Your program CRASHED before.  Please recover your progress using libnvthread API\n");
            crashed = true;
        } else {
            lprintf("Your program did not crash before.  Continue normal execution\n");
            CreateLogPath();
            crashed = false;
        }
    }

    // Return the flag indicating whether the current program crashed before
    bool isCrashed(void) {
        return crashed;
    }
};

#endif
