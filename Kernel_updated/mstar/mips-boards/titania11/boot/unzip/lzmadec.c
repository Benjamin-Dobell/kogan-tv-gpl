///////////////////////////////////////////////////////////////////////////////////////////////////
// @file   LzmaDec.c
// @author MStar Semiconductor Inc.
// @brief  Decompression Module
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------------------------------
#include "lzmadecode.h"

//-------------------------------------------------------------------------------------------------
// Local Defines
//-------------------------------------------------------------------------------------------------
//Test on host using Visual C++; Otherwise, work on target
//#define TEST_DECOMPRESS_ON_HOST

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Local Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Local Function Prototypes
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Decompress API
// @param inStream  \b IN: ptr to compressed data
// @param outStream \b OUT: ptr to decompressed data
// @param internalMem  \b IN: ptr decompression buffer (4-Byte aligned). see below for required size
// @return <0 : error
// @return otherwise: decompressed data size
// @note   Assign a0, a1, a2 and Set sp first before calling this subroutine.
// @note   Set optimization level = ?
//-------------------------------------------------------------------------------------------------
int LzmaDec(const unsigned char *inStream, unsigned char *outStream, unsigned char *internalMem)
{
    CLzmaDecoderState state;
    unsigned int outSize, inProcessed, outProcessed; //handle only data of size < 2^32 = 4GB
    int ret;


    ///// Check parameters /////
    if ( (unsigned int)internalMem % 4)
    {
        return -1;
    }


    ///// Check header /////
    if ( LzmaDecodeProperties( &state.Properties, inStream, LZMA_PROPERTIES_SIZE) != LZMA_RESULT_OK )
    {
        return -1;
    }

    inStream += LZMA_PROPERTIES_SIZE;
    outSize = ((unsigned int)inStream[3] << 24) | ((unsigned int)inStream[2] << 16) | ((unsigned int)inStream[1] << 8) | (unsigned int)inStream[0];
    if ( outSize == 0)
    {
        return -1;
    }
    #define LZMA_UNCOMPRESSED_SIZE 8
    inStream += LZMA_UNCOMPRESSED_SIZE;


    ///// Allocate memory block for internal structures /////
    //
    //in LzmaDecode.h
    //#define LZMA_BASE_SIZE 1846
    //#define LZMA_LIT_SIZE 768
    //#define LzmaGetNumProbs(Properties) (LZMA_BASE_SIZE + (LZMA_LIT_SIZE << ((Properties)->lc+ (Properties)->lp)))
    //
    //in LZMA.exe <e|d> inputFile outputFile [<switches>...]
    //<Switches>
    //  ...
    //  -lc{N}: set number of literal context bits - [0, 8], default: 3
    //          Sometimes lc=4 gives gain for big files.
    //  -lp{N}: set number of literal pos bits - [0, 4], default: 0
    //          lp switch is intended for periodical data when period is
    //          equal 2^N. For example, for 32-bit (4 bytes)
    //          periodical data you can use lp=2. Often it's better to set lc0,
    //          if you change lp switch.
    //state.Probs = (CProb *)malloc(LzmaGetNumProbs(&state.Properties) * sizeof(CProb)); //sizeof(CProb) = 2(default) or 4
    //e.g. LZMA.exe inputFile outputFile -lc0 -lp0  => needs 5228 Bytes of internal memory
    //e.g. LZMA.exe inputFile outputFile -lc0 -lp2  => needs 9836 Bytes of internal memory
    //e.g. LZMA.exe inputFile outputFile -lc3 -lp0 (default) => needs 15980 Bytes of internal memory
    state.Probs = (CProb *)internalMem; //caller decides to use static memory or dynamic memory


    ///// Start decoding /////
    ret = LzmaDecode(&state, inStream, outSize/*0*/, &inProcessed, outStream, outSize, &outProcessed); //inSize is unknow, so asssume inSize=outSize

    if (ret == LZMA_RESULT_OK)
    {
        if (outProcessed != outSize || outProcessed==0 )
        {
            ret = -1;
        }
        else
        {
            ret = outProcessed;
        }
    }
    else //LZMA_RESULT_DATA_ERROR
    {
        ret = -1;
    }

    return ret;
}


//-------------------------------------------------------------------------------------------------
// Application program to test the Decompression API on Host using Visual C++
// @param  arg1 \b IN: compressed file name
// @param  arg2 \b OUT: decompressed file name
// @return 0: succeed
// @return -1: fail
//-------------------------------------------------------------------------------------------------
#ifdef TEST_DECOMPRESS_ON_HOST

#include <stdio.h>
#include <stdlib.h>
int main(int numArgs, const char *args[])
{
    FILE *inFile = 0;
    FILE *outFile = 0;
    unsigned char *internalMem, *inStream, *outStream;
    unsigned int inFilesize, outProcessed;

    if (numArgs != 3)
    {
        printf("\nUsage: LzmaDec inFile outFile\n");
        return -1;
    }

    inFile = fopen(args[1], "rb");
    if (inFile == 0)
    {
        printf("\nCan not open input file");
        return -1;
    }

    outFile = fopen(args[2], "wb+");
    if (outFile == 0)
    {
        printf("\nCan not open output file");
        return -1;
    }

    fseek(inFile, 0, SEEK_END);
    inFilesize = ftell(inFile);
    fseek(inFile, 0, SEEK_SET);

    //data in memory
    inStream = (unsigned char *)malloc(inFilesize);
    outStream = (unsigned char *)malloc(10*inFilesize); //compression ratio < 1/10
    internalMem = (unsigned char *)malloc(5228); //Note: refer to LzmaDec() to set the size

    if ( fread(inStream, 1, inFilesize, inFile) != inFilesize )
    {
        printf("\nread input file error!");
        return -1;
    }

    if ( (outProcessed = LzmaDec(inStream, outStream, internalMem) ) ==  LZMA_RESULT_DATA_ERROR )
    {
        printf("\nDecode error!");
        return -1;
    }

    if ( fwrite(outStream, 1, outProcessed, outFile) != outProcessed )
    {
        printf("\nwrite output file error!");
        return -1;
    }

    fclose(outFile);
    fclose(inFile);
    return 0;
}

#else

/*
//-------------------------------------------------------------------------------------------------
// Application program to test the Decompression API on target
// @return None
//-------------------------------------------------------------------------------------------------
int main(void)
{
	LzmaDec((const unsigned char *)0xa0400000, (unsigned char *)0xa0500000, (unsigned char *)0xa0600000);
	return 0;
}
*/

#endif
