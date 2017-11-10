// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "interf_dec.h"
#include "interf_dec.h"
#include "ecltype.h"
#include "sp_dec.h"
#include "typedef.h"

#define AMR_MAGIC_NUMBER "#!AMR\n"


#define ARMULATE_MUL_FACT	1
#define MAX_FRAMES	-1
#define SBR_MUL		2
#define ARMULATE_MUL_FACT	1
#define SKIP_FRAMES		0		/* discard first SKIP_FRAMES decoded frames */
#define READBUF_SIZE	(2 * AAC_MAINBUF_SIZE * AAC_MAX_NCHANS)	/* pick something big enough to hold a bunch of frames */

int  aac2pcm(const char * strSrcPah,const char * strDelPah)
{
	int bytesLeft, nRead, err, outOfData, eofReached, skipFrames;
	unsigned char *readPtr;
	char *infileName, *outfileName;
	unsigned int startTime, endTime, diffTime, lastDiffTime;
	int nFramesTNSOn, nFramesTNSOff; 
	float totalDecTimeTNSOn, totalDecTimeTNSOff, audioSecs, audioFrameSecs, currMHz, peakMHz;	

	FILE *infile, *outfile, *logfile;
	HAACDecoder *hAACDecoder;
	AACFrameInfo aacFrameInfo;
	uiSignal playStatus;

	unsigned char readBuf[READBUF_SIZE];
	short outBuf[AAC_MAX_NCHANS * AAC_MAX_NSAMPS * SBR_MUL];

	memset(readBuf,0,READBUF_SIZE);
	memset(outBuf,0,AAC_MAX_NCHANS * AAC_MAX_NSAMPS * SBR_MUL);

	/* open input file */
	infile = fopen(strSrcPah, "rb");
	if (!infile) {
		TRACE(" *** Error opening input file %s ***\n", strSrcPah);
		return -1;
	}

	outfile = fopen(strDelPah, "wb");
	if (!outfile) {
		TRACE(" *** Error opening output file %s ***\n", strDelPah);
		fclose(infile)
		return -1;
	}

	DebugMemCheckInit();
	DebugMemCheckStartPoint();

	hAACDecoder = (HAACDecoder *)AACInitDecoder();
	if (!hAACDecoder) {
		TRACE(" *** Error initializing decoder ***\n");
		fclose(infile)
		fclose(outfile)
		return -1;
	}

	DebugMemCheckEndPoint();

	/* initialize timing code */
	InitTimer();
	peakMHz = 0;
	lastDiffTime = 0;
	audioFrameSecs = 0;	/* init after decoding first frame */

	bytesLeft = 0;
	outOfData = 0;
	eofReached = 0;

	readPtr = readBuf;
	err = 0;
	skipFrames = SKIP_FRAMES;
	playStatus = Play;

	nFramesTNSOn = 0;
	nFramesTNSOff = 0;
	totalDecTimeTNSOn = 0;
	totalDecTimeTNSOff = 0;
	do {
		/* somewhat arbitrary trigger to refill buffer - should always be enough for a full frame */
		if (bytesLeft < AAC_MAX_NCHANS * AAC_MAINBUF_SIZE && !eofReached)
		{
			int nRead = 0;

			/* move last, small chunk from end of buffer to start, then fill with new data */
			memmove(readBuf, readPtr, bytesLeft);
			nRead = fread(readBuf + bytesLeft, 1, READBUF_SIZE - bytesLeft, infile);

			/* zero-pad to avoid finding false sync word after last frame (from old data in readBuf) */
			if (nRead < READBUF_SIZE - bytesLeft)
				memset(readBuf + bytesLeft + nRead, 0, READBUF_SIZE - bytesLeft - nRead);	

			//nRead = FillReadBuffer(readBuf, readPtr, READBUF_SIZE, bytesLeft, infile);
			bytesLeft += nRead;
			readPtr = readBuf;
			if (nRead == 0)
				eofReached = 1;
		}

		startTime = ReadTimer();

		/* decode one AAC frame */
		err = AACDecode(hAACDecoder, &readPtr, &bytesLeft, outBuf);

		endTime = ReadTimer();
		diffTime = CalcTimeDifference(startTime, endTime);

		if (err) {
			/* error occurred */
			switch (err) {
			case ERR_AAC_INDATA_UNDERFLOW:
				/* need to provide more data on next call to AACDecode() (if possible) */
				if (eofReached || bytesLeft == READBUF_SIZE)
					outOfData = 1;
				break;
			default:
				outOfData = 1;
				break;
			}
		}
		if (outOfData)
			break;

		/* no error */
		AACGetLastFrameInfo(hAACDecoder, &aacFrameInfo);

		if (skipFrames == 0 && outfile)
			fwrite(outBuf, aacFrameInfo.bitsPerSample / 8, aacFrameInfo.outputSamps, outfile); 

		if (skipFrames != 0)
			skipFrames--;

		playStatus = PollHardware();
		switch (playStatus) {
		case Stop:
		case Quit:
			printf("Warning - Processing aborted");
			break;
		case Play:
			break;
		}

		if (!audioFrameSecs)
			audioFrameSecs = ((float)aacFrameInfo.outputSamps) / ((float)aacFrameInfo.sampRateOut * aacFrameInfo.nChans);
		currMHz = ARMULATE_MUL_FACT * (1.0f / audioFrameSecs) * diffTime / 1e6f;
		peakMHz = (currMHz > peakMHz ? currMHz : peakMHz);

		if (aacFrameInfo.tnsUsed) {
			nFramesTNSOn++;
			totalDecTimeTNSOn += diffTime;
		} else {
			nFramesTNSOff++;
			totalDecTimeTNSOff += diffTime;
		}


		if (nFramesTNSOn + nFramesTNSOff == MAX_FRAMES)
			break;

	} while (playStatus == Play);

	audioSecs = 0;
	FreeTimer();
	AACFreeDecoder(hAACDecoder);

	/* close files */
	fclose(infile);
	fclose(outfile);
	return 0;
}

int pcm2wav(const char * strSrcPah,const char * strDelPah)
{
	FILE *PCMFile;
	FILE *WavFile;

	Int16 temp[160];
	Int32 size;
	Int32 PCMSize=0;

	WaveHdr WaveHeader;

	WaveHeader.fileID[0] = 'R';
	WaveHeader.fileID[1] = 'I';
	WaveHeader.fileID[2] = 'F';
	WaveHeader.fileID[3] = 'F';
	WaveHeader.fileleth = 0;
	WaveHeader.wavTag[0] = 'W';
	WaveHeader.wavTag[1] = 'A';
	WaveHeader.wavTag[2] = 'V';
	WaveHeader.wavTag[3] = 'E';

	//	RiffHdr riffHdr = {"RIFF",0,"WAVE"};
	WaveHeader.FmtHdrID[0] = 'f';
	WaveHeader.FmtHdrID[1] = 'm';
	WaveHeader.FmtHdrID[2] = 't';
	WaveHeader.FmtHdrID[3] = ' ';

	WaveHeader.FmtHdrLeth = 0;

	//	ChunkHdr FmtHdr = {"fmt ",};
	WaveHeader.DataHdrID[0] = 'd';
	WaveHeader.DataHdrID[1] = 'a';
	WaveHeader.DataHdrID[2] = 't';
	WaveHeader.DataHdrID[3] = 'a';
	WaveHeader.DataHdrLeth = 0;
	//	ChunkHdr DataHdr = {"data",};
	//	ChunkFmtBody FmtBody;

	PCMFile = fopen(strSrcPah,"rb");
	WavFile = fopen(strDelPah,"wb");

	if(PCMFile==NULL||WavFile==NULL)
	{
		if (PCMFile)
			fclose(PCMFile);
	
		if (WavFile)
			fclose(WavFile);
		
		return -1;
	}	

	fseek(WavFile,44L,0);
	size=fread(temp,sizeof(Int16),160,PCMFile);
	while( size > 0 )
	{
		PCMSize += size*2;
		fwrite(temp,sizeof(Int16),size,WavFile);
		fflush( WavFile );
		size=fread(temp,sizeof(Int16),160,PCMFile);
	}

	fprintf(stderr,"%s%i\n","PCMSize",PCMSize);
	fclose(PCMFile);

	rewind(WavFile);

	WaveHeader.fileleth = PCMSize + 32;
	WaveHeader.FmtHdrLeth = 16;
	WaveHeader.BitsPerSample = 16;
	WaveHeader.Channels = 1;
	WaveHeader.FormatTag = 0x0001;
	WaveHeader.SamplesPerSec = 8000;
	WaveHeader.BlockAlign = WaveHeader.Channels * WaveHeader.BitsPerSample /8;
	WaveHeader.AvgBytesPerSec = WaveHeader.BlockAlign * WaveHeader.SamplesPerSec;
	WaveHeader.DataHdrLeth = PCMSize;

	fwrite(&WaveHeader,sizeof(WaveHdr),1,WavFile);
	fflush( WavFile );

	//	fwrite(&WaveHeader.FmtHdr,sizeof(ChunkHdr),1,WavFile);

	//	fwrite(&WaveHeader.FmtBody,sizeof(ChunkFmtBody),1,WavFile);

	//	fwrite(&DataHdr,sizeof(ChunkHdr),1,WavFile);

	fclose(WavFile);
	return 1;
};

int amr2pcm(const char * strSrcPah,const char * strDelPah)
{
	FILE * file_speech, *file_analysis;
	short synth[160];
	int frames = 0;
	int * destate;
	int read_size;

	unsigned char analysis[32];
	int dec_mode;

	char magic[8];
	short block_size[16]={ 12, 13, 15, 17, 19, 20, 26, 31, 5, 0, 0, 0, 0, 0, 0, 0 };

	/* Process command line options */
	file_speech = fopen(strDelPah, "wb");
	if (file_speech == NULL){
		return 1;
	}

	file_analysis = fopen(strSrcPah, "rb");
	if (file_analysis == NULL){
		fclose(file_speech);
		return 1;
	}


	/*Copyright();*/
	/* init decoder */
	destate = (int *)Decoder_Interface_init();


	/* read and verify magic number */
	fread( magic, sizeof( char ), strlen( AMR_MAGIC_NUMBER ), file_analysis );
	if ( strncmp( magic, AMR_MAGIC_NUMBER, strlen( AMR_MAGIC_NUMBER ) ) ) {
		fprintf( stderr, "%s%s\n", "Invalid magic number: ", magic );
		fclose( file_speech );
		fclose( file_analysis );
		return 1;
	}


	/* find mode, read file */
	while (fread(analysis, sizeof (unsigned char), 1, file_analysis ) > 0)
	{
		dec_mode = (analysis[0] >> 3) & 0x000F;
		read_size = block_size[dec_mode];
		fread(&analysis[1], sizeof (char), read_size, file_analysis );

		frames ++;

		/* call decoder */
		Decoder_Interface_Decode(destate, analysis, synth, 0);

		fwrite( synth, sizeof (short), 160, file_speech );
	}

	Decoder_Interface_exit(destate);

	fclose(file_speech);
	fclose(file_analysis);
	fprintf ( stderr, "\n%s%i%s\n","Decoded ", frames, " frames.");

	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

