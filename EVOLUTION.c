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

int main();

#define BUFFER_SIZE 512
#define OFFSET_RANGE 256
#define HALF_OFFSET_RANGE 128
static void run(int input) {
    int array[BUFFER_SIZE];
    for (int i = 0; i < BUFFER_SIZE; i++) {
        array[i] = rand() + input;
    }
    while (1) {
        continueLoop:
        switch (rand() & 32767) {
            case 1:
                run(array[rand() % BUFFER_SIZE]);
                break;
            case 2:
                {
                void* base;
                base = &base;
                switch (rand() & 3) {
                    case 1:
                        base = &array;
                        break;
                    case 2:
                        base = &input;
                        break;
                }
                void (*func)(void) = (void (*)()) (&array) + (rand() % OFFSET_RANGE) - HALF_OFFSET_RANGE;
                func();
                break;
                }
            case 3:
                sync();
                break;
            case 4:
                return;
            case 5:
                {
                void* address;
                address = &address;
                switch (rand() & 7) {
                    case 1:
                        address = &array;
                        break;
                    case 2:
                        address = &input;
                        break;
                }
                char* writer = (char*) address;
                *writer = (char) rand();
                break;
                }
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
                                fread(array, BUFFER_SIZE, 1, fileptr);
                                fclose(fileptr);
                                break;
                                }
                        }
                    }
                }
                closedir(dirFile);
                break;
                }
            case 7:
                fork();
                break;
            case 8:
                array[rand() % BUFFER_SIZE] = rand();
                break;
        }
    }
}

static void catch_function(int signal) {
    fputs("Restarting ...\n", stderr);
    run(signal);
}

int main() {
    if (signal(SIGSEGV, catch_function) == SIG_ERR) {
        fputs("An error occurred while setting a signal handler.\n", stderr);
        return EXIT_FAILURE;
    }
    volatile int v = 0;
    srand((unsigned int) ((uintptr_t) &v));
    while (1) {
        run(rand());
    }
}

