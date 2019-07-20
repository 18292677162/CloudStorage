#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "fdfs_api.h"


int main(int argc, char* argv[])
{
    char fileid[1024] = {0};
    fdfs_upload_file("/etc/fdfs/client.conf", argv[1], fileid);
    printf("fileID = %s\n", fileid);
}