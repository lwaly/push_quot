#include <string.h>
#include <algorithm>

#include "File.h"
#include "AutoTable.h"

int main(int argc, char* argv[])
{
    if (2 != argc)
    {
        printf("Invalid argument.\n 1:sql file\n");
        return -1;
    }

    AutoTable obj;
    obj.Analysis(argv[1]);

    return 0;
}
