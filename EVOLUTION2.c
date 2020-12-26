/*
    EVOLUTION
    Copyright (C) 2020  tsao-chi

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <stdio.h>
#include <stddef.h>
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int main();

#define BUFFER_SIZE 65536
static char memory[BUFFER_SIZE];

static void catch_function(int signal) {
    main();
}

int main() {
    if (signal(SIGSEGV, catch_function) == SIG_ERR) {
        fputs("An error occurred while setting a signal handler.\n", stderr);
        return EXIT_FAILURE;
    }
    if (signal(SIGILL, catch_function) == SIG_ERR) {
        fputs("An error occurred while setting a signal handler.\n", stderr);
        return EXIT_FAILURE;
    }
    volatile int v = 0;
    srand((unsigned int) ((uintptr_t) &v));
    while (1) {
        continueLoop:
        switch (rand() & 32767) {
            case 1:
                {
                if (fork() == 0) {
                    void (*func)(void) = (void*) &memory[rand() % BUFFER_SIZE];
                    func();
                }
                break;
                }
            case 2:
                fork();
                break;
            case 3:
                break;
            case 4:
                srand(rand() + (unsigned int) memory[rand() % BUFFER_SIZE]);
                break;
            case 5: ;
                size_t s1 = rand() % BUFFER_SIZE;
                size_t s2 = rand() % BUFFER_SIZE;
                size_t n = rand() % (BUFFER_SIZE - MAX(s1, s2));
                if (n != 0) {
                    memcpy(&memory[s1], &memory[s2], n);
                }
                break;
            case 6:
                {
                DIR* dirFile = opendir( "." );
                struct dirent* hFile;
                if (!dirFile) break;
                while (( hFile = readdir( dirFile )) != NULL ) {
                    DIR* dirFile_inner = opendir(hFile->d_name);
                    if (dirFile_inner) closedir(dirFile_inner);
                    if (dirFile_inner) {
                        switch (rand() & 7) {
                            case 1:
                                chdir(hFile->d_name);
                                closedir(dirFile);
                                goto continueLoop;
                            case 2:
                                closedir(dirFile);
                                dirFile = opendir(hFile->d_name);
                                if(!dirFile) goto continueLoop;
                        }
                    } else {
                        switch (rand() & 7) {
                            case 1:
                                {
                                FILE *fileptr = fopen(hFile->d_name, "rb");
                                fread(memory, rand() % BUFFER_SIZE, 1, fileptr);
                                fclose(fileptr);
                                break;
                                }
                        }
                    }
                }
                closedir(dirFile);
                break;
                }
        }
    }
    return 0;
}

