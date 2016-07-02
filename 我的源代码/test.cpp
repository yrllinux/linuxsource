#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
     int p[8] = {0};
     p[8] = 10;
     delete[] p;
     return 0;
}
