// C++ headers
#include <iostream>

// C header
#include <sys/mman.h>
#include <unistd.h>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <fcntl.h>


#define MEM_SIZE 4096*2 
int main(int argc, char *argv[])
{
    using std::endl;
    using std::cout;
    using std::cerr;

    char *addr;
    int fd;
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        cout << argv[0] << " file [new-value]" << endl;
        exit(EXIT_FAILURE);
    }
    fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        cerr << "Can not open file " << argv[1] << endl;
        exit(EXIT_FAILURE);
    }

    addr = (char*)mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        cerr << "Memory mapping file has failed" << endl;
        exit(EXIT_FAILURE);
    }
    if (close(fd) == -1) {
        cerr << "Closing file has failed" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "No longer need 'fd'" << endl;
    //printf("Current string=%.*s\n", MEM_SIZE, addr);
    cout << "Current string:" << addr << endl;

    /* Secure practice: output at most MEM_SIZE bytes */
    if (argc > 2) {
        /* Update contents of region */
        if (strlen(argv[2]) >= MEM_SIZE) {
            cout << "new-value size " << strlen(argv[2]) << "is larger, than max " << MEM_SIZE << endl;
            exit(EXIT_FAILURE);
        }
        memset(addr, 0, MEM_SIZE);

        /* Zero out region */
        strncpy(addr, argv[2], strlen(argv[2]));
        if (msync(addr, MEM_SIZE, MS_SYNC) == -1) {
           cerr << "msync has failed " << endl; 
           exit(EXIT_FAILURE);
        }
        printf("Copied \"%s\" to shared memory\n", argv[2]);
    }
    exit(EXIT_SUCCESS);
}

