#include <stdio.h>
#include <string.h>
#include <time.h>

//    #define RIU_MAP 0xA0000000  //AEON
    #define RIU_MAP 0xBF000000  //MIPS
    #define RIU     ((unsigned short volatile *) RIU_MAP)



void test_cache1();
void test_cache2();
int test_dram();


#define U32 unsigned long


int funct1(void)
{
    int i=0;

    i++;
    i++;
    i++;
    i++;
    i++;
    return i;
}


int main(void)
{
    int i;
    int j;
    char buf[256];
    //FILE *fp;

    i = 0;
    j = 0;
    RIU[0x103390]=0x9999;
    j = funct1();

    RIU[0x1033a0]=0xAAAA;
    i = 1;
    j = 1;

    RIU[0x1033b0]=0xbbbb;
    //*(volatile U32*)(0x88000000) = 0x11111111;
    //*(volatile U32*)(0x88000004) = 0x22222222;
    //*(volatile U32*)(0x88000008) = 0x33333333;

    while(i)
    {
       RIU[0x103388]++;
    }
    j = 0;
    j = test_dram();

    while(i)
    {
        RIU[0x103380]++;

        test_cache1();

        test_cache2();

        test_cache1();

        test_cache2();
    }

    while(i)
    {
        int a, b;

        a=0;
        b=0;
        a= a/b;

    }
    return 0;
}



void test_cache1()
{
    int i;

    for (i=0; i< 1000; i++);

}

void test_cache2()
{
    int i,j;

    for (i=0; i< 1000; i++)
    for (j=0; j< 1000; j++);

}

#define BUF_SIZE        128 //30*1024*1024
static unsigned char data[BUF_SIZE];

int test_dram()
{

    unsigned long i,j;

    for (i=0; i<BUF_SIZE; i++)
    {
        data[i] = i&0xFF;

    }

    for (i=0; i<BUF_SIZE; i++)
    {
        if (data[i] != (i&0xFF))
            return 0;
    }

    return 1;
}
