
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DATA "/proc/msgio/data"
#define TAG "MSGIO-TEST:"
#define BUFSIZE 2048
static char buf[BUFSIZE];

int main(int argc, char *argv[])
{
    int fd;
    ssize_t len;

    printf(TAG "READY to open %s\n", DATA);
    fd = open(DATA, O_RDONLY);

    if(fd < 0)
    {
	printf(TAG "FAIL to open\n");
	exit(-2);
    }

    while(1)
    {
	lseek(fd, 0, SEEK_SET);
	printf(TAG "READY to read %s\n", DATA);
	len = read(fd, buf, BUFSIZE);
	printf(TAG "len = %d\n", len);
	usleep(3 * 100000);
    }

    printf(TAG "READY to close %s\n", DATA);
    close(fd);
    return 0;
}
