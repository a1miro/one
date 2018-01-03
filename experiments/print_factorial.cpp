#include<unistd.h>
#include<iostream>

using namespace std;
uint64_t s = 1;
uint64_t f = 1;
int main() {

    while(true) {
        cout << "factorial " << s+1 << "=";
        f *= ++s;
        cout << f << endl;
        sleep(1);
    }

    return 0;
}
