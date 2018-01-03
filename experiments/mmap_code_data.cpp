// C++ headers
#include <iostream>
#include <array>
#include <limits>

// C header
#include <sys/mman.h>
#include <unistd.h>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <fcntl.h>


#define MEM_SIZE 4096*2 


uint64_t* factorial = nullptr;
uint64_t* number = nullptr;
int main(int argc, char *argv[])
{
    using std::endl;
    using std::cout;
    using std::cerr;

    char *addr;
    int fd;
    bool first = false;
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        cout << argv[0] << " file [new-value]" << endl;
        exit(EXIT_FAILURE);
    }
    fd = open(argv[1], O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        cout << "The file " << argv[1] << " does not exist, creating a new one" << endl;
        fd = open(argv[1], O_CREAT|O_RDWR, S_IRUSR | S_IWUSR);
        if (fd == -1) {
           cerr << "Failed to create " << argv[1] << " file" << endl; 
           exit(EXIT_FAILURE);
        }
        first = true;

        std::array<char,MEM_SIZE> buf;
        buf.fill(0);
        write(fd, buf.data(), buf.size());
    }
    

    addr = (char*)mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        cerr << "Memory mapping file has failed" << endl;
        exit(EXIT_FAILURE);
    }

    factorial = (uint64_t*) addr;
    number = (uint64_t*) (addr + sizeof(uint64_t));

    if (first)
    {
        *factorial = (uint64_t)1;
        *number = (uint64_t)1; 
    }
    

    while (*factorial < std::numeric_limits<uint64_t>::max() && *factorial != 0) {
        *factorial *= (*number)++ ;

        cout <<  *number << "! = " << *factorial << endl;
        if(msync(addr, MEM_SIZE, MS_SYNC) == -1) {
            cerr << "Mem sync error " << endl;
            exit(EXIT_FAILURE);
        }
    }



#if 0
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
#endif
}

