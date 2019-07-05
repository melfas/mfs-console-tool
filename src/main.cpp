/*
 * melfas_update_tool
 *
 * Copyright (C) 2019 MELFAS Inc.
 *
 *
 * main.c
 *
 * Version : 2019.06.12
 */

#include <stdio.h>
#include <libgen.h>
#include <wchar.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <dirent.h>  // _getcwd()
#include <time.h>
#include <pthread.h>

#include "hidapi.h"
#include "hiddef.h"
#include "mip4_reg.h"
#include "mfsConfig.h"

// Headers needed for sleeping.
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

#define APP_PRINT_PACKET_ENABLE         0
#define DEBUG_LOG 						0
#define getAddress(R0, R1)      ((R0<<8)|R1)


/* firmware info */
#define MIP4_BL_PAGE_SIZE 512
#define NIP4_BL_PACKET_SIZE 512

/*
 * Firmware binary tail info
 */
struct mip4_bin_tail
{
	uint8_t tail_mark[4];
	uint8_t chip_name[4];
	uint32_t bin_start_addr;
	uint32_t bin_length;
	uint16_t ver_boot;
	uint16_t ver_core;
	uint16_t ver_app;
	uint16_t ver_param;
	uint8_t boot_start;
	uint8_t boot_end;
	uint8_t core_start;
	uint8_t core_end;
	uint8_t app_start;
	uint8_t app_end;
	uint8_t param_start;
	uint8_t param_end;
	uint8_t checksum_type;
	uint8_t hw_categoty;
	uint16_t param_id;
	uint32_t param_length;
	uint32_t build_date;
	uint32_t build_time;
	uint32_t reserved1;
	uint32_t reserved2;
	uint16_t reserved3;
	uint16_t tail_size;
	uint32_t crc;
} __packed;

HID_CUSTOM_PACKET mPacket;
hid_device *mHandle;
bool bConnected = false;

mfsConfig mConfig;

// download function
int binfile_length;
int dl_step = 0;
int dl_total = 0;
int error_log = 1;
int thread_flag = 1;
bool bIsWaitClose;

#define MIP4_BIN_TAIL_MARK	"MBT\001"
#define MIP4_BIN_TAIL_SIZE	(sizeof(struct mip4_bin_tail))

unsigned char *pfwBuf = NULL;
struct mip4_bin_tail fw_tail;
uint16_t buf_addr;

void (*breakCapture)(int);

int excute_fw_update();
int PageWriteAndVerify(int _nAddr, uint8_t* _pData, int _nLength, int _bVerify );
int PageRead(int _nAddr, uint8_t* _pBuffer, int _nLength );
int PageWrite(int _nAddr, uint8_t* _pData, int _nLength);
int _ReadFromBuf(uint8_t* _pBuf, int _nLength);
int _WriteIntoBuf(uint8_t* _pData, int _nLength);
int _WriteTargetAddrAndSize(int _nAddr, int _nSize);
int change_mode_boot(unsigned char boot_mode);
int bl_read_status();
int fw_parser_tail(char *path);

// progress thread
void *t_function(void *data)
{
	static int retval = 999;
	char *thread_name = (char *)data;
	const char bar = '#';
	const char blank = '.';
	const int LEN = 70;
	float tick = (float)100/LEN;
	int bar_count;
	float percent;
	while (1)
	{
		percent = (float)(dl_step * 100) / dl_total;
		bar_count = percent / tick;
		if (0 < percent)
		{
			if (0 == thread_flag)
				break;

			printf("\r%s [", thread_name);
			for (int i = 0; i < LEN; i++)
			{ 
				if (bar_count > i)
					printf("%c", bar);
				else
					printf("%c", blank);
			}
			printf("] %0.2f%%    ", percent);
		}
		
		if (100 <= percent)
		{
			pthread_exit((void *)&retval);
			break;
		}

		usleep(100);
	}

	printf("\n");
	return 0;
}

void signalingHandler(int signo) {

  thread_flag = 0;
  usleep(500*1000);

  printf("wait for closing...\n");
}

void PrintPacket(HID_CUSTOM_PACKET *_pPacket)
{
	printf(" -----------------------------------------------------------------------------------------\r\n");

	if (_pPacket->ReportId == HID_REPORT_ID_CUSTOM_OUT)
		printf(" Host --> Device ");
	else
		printf(" Device --> Host ");

	if (_pPacket->ReadWrite == HID_RW_WRITE)
		printf("For WRITE\r\n");
	else
		printf("For READ\r\n");

	printf(" RW  Last  Addr  Len  Data                                             Route  ExtID  RptID\r\n");

	printf(" %-2d  %-4d  %04X  %-3d  ", _pPacket->ReadWrite, _pPacket->Last, _pPacket->Address, _pPacket->Length);

	printf("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X  ", _pPacket->Data[0], _pPacket->Data[1], _pPacket->Data[2], _pPacket->Data[3], _pPacket->Data[4], _pPacket->Data[5], _pPacket->Data[6], _pPacket->Data[7], _pPacket->Data[8], _pPacket->Data[9], _pPacket->Data[10], _pPacket->Data[11], _pPacket->Data[12], _pPacket->Data[13], _pPacket->Data[14], _pPacket->Data[15]);

	printf("%-5d  %-5d  %02X\r\n", _pPacket->RouteIdx, _pPacket->ExtId, _pPacket->ReportId);
	printf(" -----------------------------------------------------------------------------------------\r\n");
}

bool WriteTo(uint16_t _nSubAddress, uint8_t* _pData, int _nWriteLength)
{
	bool bRet = false;

	if (_nWriteLength > HID_DATA_LENGTH_MAXIMUM )
	{
		return false;
	}

	int nLength;
	int nOffset;
	int nDataLength;
	uint8_t nExtId;
	bool bLast;

	HID_CUSTOM_PACKET packet_write;

	nExtId = 0;
	nDataLength = HID_CUSTOM_PACKET_DATA_SIZE;

	nOffset = 0;
	bLast = false;

	unsigned char writeBuf[HID_CUSTOM_PACKET_SIZE];

RETRY:
	while(!bLast)
	{
		// prepare packet

		memset(&packet_write, 0x00, sizeof(HID_CUSTOM_PACKET));

		if ( (_nWriteLength - nOffset) <= nDataLength)
		{
			bLast = true;
			nDataLength = (_nWriteLength - nOffset);
		}

		packet_write.ReportId = HID_REPORT_ID_CUSTOM_OUT;
		packet_write.ReadWrite = HID_RW_WRITE;
		packet_write.Last = bLast;
		packet_write.ExtId = (uint8_t)nExtId;
		packet_write.RouteIdx = 0;
		packet_write.Address = (uint16_t)(_nSubAddress + nOffset);
		packet_write.Length = (uint16_t)nDataLength;

		if (nExtId == HID_EXT_ID_ADDITIONAL_WRITE_AND_READ)
		{
			packet_write.Data[0] = (uint8_t)nDataLength;
			memcpy(&packet_write.Data[1], &_pData[nOffset], nDataLength);
		}
		else
		{
			memcpy(&packet_write.Data[0], &_pData[nOffset], nDataLength);
		}

		memset(writeBuf,0,sizeof(writeBuf));
		memcpy(&writeBuf, &packet_write, sizeof(HID_CUSTOM_PACKET));

		#if APP_PRINT_PACKET_ENABLE
			PrintPacket(&packet_write);
		#endif

		nLength = hid_write(mHandle, writeBuf, sizeof(HID_CUSTOM_PACKET));

		if (0 > nLength)
		{
			if (1 == error_log)
				printf("hid_writeTo error [%d]\r\n", nLength);
			goto RETRY;
		}

		nOffset += nDataLength;
	}

	bRet = true;

	return bRet;
}

bool WriteForRead(uint16_t _nSubAddress, uint8_t* _pData, int _nWriteLength, int _nReadLength)
{
	bool bRet = false;

	int nLength;
	int nOffset;
	int nDataLength;
	uint8_t nExtID;
	bool bLast;

	unsigned char buf[HID_CUSTOM_PACKET_SIZE];

	HID_CUSTOM_PACKET packet_write;

	if (0 < _nWriteLength)
		nExtID = HID_EXT_ID_ADDITIONAL_WRITE_AND_READ;
	else
		nExtID = 0;

	if (nExtID == HID_EXT_ID_ADDITIONAL_WRITE_AND_READ)
	{
		nDataLength = HID_CUSTOM_PACKET_DATA_SIZE - 1;
	}
	else
	{
		nDataLength = HID_CUSTOM_PACKET_DATA_SIZE;
	}

	nOffset = 0;
	bLast = false;

	while(!bLast)
	{
		memset(&packet_write, 0x00, sizeof(HID_CUSTOM_PACKET));

		if ((_nWriteLength - nOffset) <= nDataLength)
		{
			bLast = true;
			nDataLength = (_nWriteLength - nOffset);
		}

		packet_write.ReportId = HID_REPORT_ID_CUSTOM_OUT;
		packet_write.ReadWrite = HID_RW_READ;
		packet_write.Last = bLast;
		packet_write.ExtId = (uint8_t)nExtID;
		packet_write.RouteIdx = 0;
		packet_write.Address = (uint16_t)(_nSubAddress + nOffset);
		packet_write.Length = (uint16_t)_nReadLength;

		if (nExtID == HID_EXT_ID_ADDITIONAL_WRITE_AND_READ)
		{
			packet_write.Data[0] = (uint8_t)nDataLength;
			memcpy(&packet_write.Data[1], &_pData[nOffset], nDataLength);
		}
		else
		{
			memcpy(&packet_write.Data[0], &_pData[nOffset], nDataLength);
		}
		
		memset(buf,0,sizeof(buf));
		memcpy(buf, &packet_write, sizeof(HID_CUSTOM_PACKET));

		#if APP_PRINT_PACKET_ENABLE
			PrintPacket(&packet_write);
		#endif

		nLength = hid_write(mHandle, buf, sizeof(HID_CUSTOM_PACKET));

		if (0 > nLength)
		{
			if (1 == error_log)
				printf("hid_write(for read) error [%d]\r\n", nLength);
			goto FIN;
		}

		nOffset += nDataLength;
	}

	bRet = true;
	
FIN:
	return bRet;
}

bool Read(uint8_t* _pBuffer, int _nReadLength)
{	
	bool bRet = false;

	int nLength;
	bool bLast;
	int nOffset;
	int nDataLength;

	HID_CUSTOM_PACKET packet_read;// = &mPacket;
	uint8_t readBuf[HID_CUSTOM_PACKET_SIZE];

	nOffset = 0;
	bLast = false;

	while(!bLast)
	{
		// Read

		memset(&packet_read, 0, sizeof(HID_CUSTOM_PACKET));
		memset(readBuf, 0, sizeof(readBuf));

		nLength = hid_read(mHandle, readBuf, _nReadLength);

		if (0 >= nLength)
		{
			printf("hid_read error [%d]\r\n", nLength);
			goto FIN;
		}

		memcpy(&packet_read, &readBuf, sizeof(HID_CUSTOM_PACKET));

		#if APP_PRINT_PACKET_ENABLE
			PrintPacket(&packet_read);
		#endif

		bLast = packet_read.Last;
		nDataLength = packet_read.Length;

		if (bLast)
		{
			if (nDataLength == _nReadLength)
				nDataLength = _nReadLength - nOffset;
		}

		// error check
		if ( (_nReadLength - nOffset) <= HID_CUSTOM_PACKET_SIZE )
		{
			if (!bLast || (_nReadLength - nOffset)!=nDataLength )
			{
				printf("error!");
				goto FIN;
			}
		}

		memcpy((void*)&_pBuffer[nOffset], packet_read.Data, nDataLength);

		nOffset += nDataLength;
	}

	bRet = true;

FIN:

	return bRet;
}

bool ReadFrom(uint16_t _nSubAddress, uint8_t* _pBuffer, int _nReadLength)
{
	if (!WriteForRead(_nSubAddress, _pBuffer, 0, _nReadLength))
		return false;

	if (!Read(_pBuffer, _nReadLength))
		return false;

	return true;
}

int main(int argc, char* argv[])
{
	breakCapture = signal(SIGINT, signalingHandler);
	
	mConfig.ParseArgs(argc, argv);

	int nAction = mConfig.GetAction();

	if( nAction == ACTION_NULL || nAction == ACTION_HELP)
	{
		printf("===========================================================\n");
		printf("melfas_update_tool for chromeOS (HID_USB) - v1.0.6\n\n");
		printf("ex) melfas_update_tool -fw_update pid fw_name\n");		
		printf("ex) melfas_update_tool -fw_version pid\n");		
		printf("===========================================================\n");
		return 0;
	}

	mHandle = hid_open(0x1FD2, mConfig.GetPid(), NULL);
	if (NULL == mHandle)
	{
		printf("Device Open Fail!\n");
		return 0;
	}

	uint8_t readBuf[256];

	if (nAction == ACTION_DOWNLOAD)
	{
		pthread_t p_thread;
		int thr_id;
		int status;

		bIsWaitClose = 0;

		thr_id = pthread_create(&p_thread, NULL, t_function, (void *)"Download");

		// pthread_create() 으로 성공적으로 쓰레드가 생성되면 0 이 리턴됩니다
		if (thr_id < 0)
		{
			perror("thread create error : ");
			exit(0);
		}

		/* fw loading */
		char *file_path = mConfig.GetSpecFileName();

		/* fw parser */
		if (0 == fw_parser_tail(file_path))
		{
			printf("wrong fw file.\n");
			exit(0);
		}

		/* current vertion */
		ReadFrom(0x0120, readBuf, 8);

		printf("\n[Current Version]\n");
		printf("%02X%02X.%02X%02X.%02X%02X.%02X%02X\n", readBuf[1], readBuf[0], readBuf[3], readBuf[2], readBuf[5], readBuf[4], readBuf[7], readBuf[6]);

		system("tput civis");

		/* excute fw update */
		if (1 == excute_fw_update())
			printf("=== PASS ===\n\n");
		else
			printf("=== FAIL ===\n\n");

		pthread_join(p_thread, (void **)&status);

		system("tput cnorm");
	}
	if (nAction == ACTION_FW_VERSION)
	{
		/* current vertion */
		ReadFrom(0x0120, readBuf, 8);

		printf("\n[FW Version]\n");
		printf("%02X%02X %02X%02X %02X%02X %02X%02X\n", readBuf[1], readBuf[0], readBuf[3], readBuf[2], readBuf[5], readBuf[4], readBuf[7], readBuf[6]);
	}
	if (nAction == ACTION_BL_MODE)
	{
		change_mode_boot(MIP4_BOOT_MODE_BOOT);

		ReadFrom(0x0120, readBuf, 8);
		for(int i=0; i<8; i++)
		{
			printf("%02X ", readBuf[i]);
		}
		printf("\n");
	}
	if(nAction == ACTION_APP_MODE)
	{
		change_mode_boot(MIP4_BOOT_MODE_APP);
		ReadFrom(0x0120, readBuf, 8);
		for(int i=0; i<8; i++)
		{
			printf("%02X ", readBuf[i]);
		}
		printf("\n");
	}

	hid_close(mHandle);
	/* Free static HIDAPI objects. */
	hid_exit();

	mConfig.Clear();

	return 0;
}

/* Firmware update */

int fw_parser_tail(char *path)
{
#if DEBUG_LOG
	printf ("fw_parset_tail : [start]\n");
#endif

	FILE *fp;
	fp = fopen(path, "rb");
	if (NULL == fp)
	{
		printf ("file open error!\n");
		return 0;
	}

	fseek(fp, 0, SEEK_END);
	int bin_size = ftell(fp);

#if DEBUG_LOG
	printf ("bin_size : %d\n", bin_size);
#endif
	uint8_t *pBuf = (uint8_t*)malloc(bin_size);

	fseek(fp, 0, SEEK_SET);

	int nLengthRead = fread(pBuf, 1, bin_size, fp);
	binfile_length = nLengthRead;
#if DEBUG_LOG
	printf ("nLengthRead : %d\n", nLengthRead);
#endif
	if (bin_size != nLengthRead)
	{
		free(pBuf);
		pBuf = NULL;
		fclose(fp);

		return 0;
	}

	fseek(fp, 0, SEEK_SET);

#if DEBUG_LOG
	printf("%02X ", pBuf[0]);
	for (int i = 1; i < nLengthRead; i++)
	{
		printf("%02X ", pBuf[i]);
		if (((i+1)%16 == 0))
			printf("\n");
	}

	printf("\n");
#endif

	/* set tail */
	memcpy(&fw_tail, &pBuf[bin_size - MIP4_BIN_TAIL_SIZE], sizeof(struct mip4_bin_tail));

	if (bin_size == (int)(fw_tail.bin_length + fw_tail.bin_start_addr))
	{
		int nOffset = fw_tail.bin_start_addr;
		bin_size = bin_size - nOffset;

		pfwBuf = (uint8_t*)malloc(bin_size);
		memcpy(pfwBuf, &pBuf[nOffset], bin_size);

		memcpy(&fw_tail, &pfwBuf[bin_size - MIP4_BIN_TAIL_SIZE], sizeof(struct mip4_bin_tail));

		free(pBuf);
	}

#if DEBUG_LOG
	printf("=====================================================================\n");
	printf("%02X ", pfwBuf[0]);
	for (int i = 1; i < fw_tail.bin_length; i++)
	{
		printf("%02X ", pfwBuf[i]);
		if (((i + 1) % 16 == 0))
			printf("\n");
	}

	printf("\n");
#endif

#if DEBUG_LOG
	printf("TAIL START\n");
	printf("fw_tail->tail_mark      : %s\n", fw_tail.tail_mark);
	printf("fw_tail->chip_name      : %s\n", fw_tail.chip_name);
	printf("fw_tail->bin_start_addr : 0x%04X\n", fw_tail.bin_start_addr);
	printf("fw_tail->bin_length     : %d\n", fw_tail.bin_length);
	printf("fw_tail->boot_Ver       : 0x%04X\n", fw_tail.ver_boot);
	printf("fw_tail->core_Ver       : 0x%04X\n", fw_tail.ver_core);
	printf("fw_tail->app_Ver        : 0x%04X\n", fw_tail.ver_app);
	printf("fw_tail->param_Ver      : 0x%04X\n", fw_tail.ver_param);
	printf("fw_tail->boot_start     : 0x%02X\n", fw_tail.boot_start);
	printf("fw_tail->boot_end       : 0x%02X\n", fw_tail.boot_end);
	printf("fw_tail->core_start     : 0x%02X\n", fw_tail.core_start);
	printf("fw_tail->core_end       : 0x%02X\n", fw_tail.core_end);
	printf("fw_tail->app_start      : 0x%02X\n", fw_tail.app_start);
	printf("fw_tail->app_end	: 0x%02X\n", fw_tail.app_end);
	printf("fw_tail->param_start 	: 0x%02X\n", fw_tail.param_start);
	printf("fw_tail->param_end   	: 0x%02X\n", fw_tail.param_end);
	printf("fw_tail->checksum_type  : 0x%02X\n", fw_tail.checksum_type);
	printf("fw_tail->hw_category    : 0x%02X\n", fw_tail.hw_categoty);
	printf("fw_tail->param_id   	: 0x%04X\n", fw_tail.param_id);
	printf("fw_tail->param_length   : %d\n", fw_tail.param_length);
	printf("fw_tail->build_date   	: %X\n", fw_tail.build_date);
	printf("fw_tail->build_date   	: %X\n", fw_tail.build_time);
	printf("fw_tail->tail_size   	: %d\n", fw_tail.tail_size);
	printf("fw_tail->crc		: 0x%04X\n", fw_tail.crc);

	printf("\n");
#endif

	fclose(fp);

	return 1;
}

int bl_read_status()
{
	unsigned char read_buf[4];

	int result;
	int retry = 1000;
	int nAddr;

	nAddr = getAddress(MIP4_R0_BOOT, MIP4_R1_BOOT_STATUS);
	do
	{
		ReadFrom(nAddr, read_buf, 1);
		result = read_buf[0];

		#if DEBUG_LOG
			printf("bl_read_status : [%02X]\n", result);
		#endif

		switch (result)
		{
			case MIP4_BOOT_STATUS_DONE:
				return 1;
				break;
			case MIP4_BOOT_STATUS_ERROR:
				return 0;
				break;
			case MIP4_BOOT_STATUS_BUSY:
				break;		
			default:
				break;
		}

		usleep(1*1000);

	} while (--retry);

	return 0;
}

int change_mode_boot(unsigned char boot_mode)
{
	error_log = 0;
#if DEBUG_LOG
	printf("change_mode_boot [%d]\n", boot_mode);
#endif

	unsigned char read_buf[4];
	unsigned char write_buf[4];

	int nAddr;
	nAddr = getAddress(MIP4_R0_BOOT, MIP4_R1_BOOT_MODE);
	int retry = 1000;

	write_buf[0] = boot_mode;
	if (!WriteTo(nAddr, write_buf, 1))
	{
#if DEBUG_LOG
		printf("change_mode_boot write error\n");
#endif

		goto error;
	}

#if DEBUG_LOG
	printf("change_mode_boot write end\n");
#endif


	/* delay */
	usleep(1000*1000);

	do
	{
		/* verify */
		if (!ReadFrom(nAddr, read_buf, 1))
		{
#if DEBUG_LOG
			printf("change_mode_boot read error\n");
			printf("hid_close\n");
#endif
			hid_close(mHandle);
			/* Free static HIDAPI objects. */
			hid_exit();

#if DEBUG_LOG
			printf("hid - re-open\n");
#endif
			mHandle = hid_open(0x1FD2, mConfig.GetPid(), NULL);
		}
#if DEBUG_LOG
		printf("change_mode_boot read [%02X]\n", read_buf[0]);
		printf("change_mode_boot read end\n");
#endif

		if (boot_mode == read_buf[0])
		{
			return 0;
		}
		usleep(10);
	} while (--retry);

	error_log = 1;

	return 0;

error:
	error_log = 1;
	return 1;
}

int _WriteTargetAddrAndSize(int _nAddr, int _nSize)
{
	unsigned char write_buf[1024];
	int nAddr;

	nAddr = getAddress(MIP4_R0_BOOT, MIP4_R1_BOOT_TARGET_ADDR);

	write_buf[0] = (uint8_t)((_nAddr & 0x000000FF) >> 0);
	write_buf[1] = (uint8_t)((_nAddr & 0x0000FF00) >> 8);
	write_buf[2] = (uint8_t)((_nAddr & 0x00FF0000) >> 16);
	write_buf[3] = (uint8_t)((_nAddr & 0xFF000000) >> 24);

	write_buf[4] = (uint8_t)((_nSize & 0x000000FF) >> 0);
	write_buf[5] = (uint8_t)((_nSize & 0x0000FF00) >> 8);
	write_buf[6] = (uint8_t)((_nSize & 0x00FF0000) >> 16);
	write_buf[7] = (uint8_t)((_nSize & 0xFF000000) >> 24);

	#if DEBUG_LOG
		printf("<<< _WriteTargetAddrAndSize Data >>>\n");
		for (int i=0; i<10; i++)
		{
			printf("%02X ", write_buf[i]);
		}
		printf("\n");
	#endif

	if (!WriteTo(nAddr, write_buf, 8))
		goto error;

	return 1;
error:
	return 0;
}

int _WriteIntoBuf(uint8_t* _pData, int _nLength)
{
		if (!WriteTo(buf_addr, _pData, MIP4_BL_PAGE_SIZE))
	{
		goto error;
	}

	return 1;

error:
	return 0;
}

int _ReadFromBuf(uint8_t* _pBuf, int _nLength)
{
	if (!ReadFrom(buf_addr, _pBuf, _nLength))
		goto error;

#if DEBUG_LOG
	printf("_ReadFromBuf length : %d\n", _nLength);
	for (int i = 0; i < _nLength; i++)
	{
		printf("%02X ", _pBuf[i]);
	}
	printf("\n");
#endif
	return 1;

	uint8_t *pBuf;


	for (int i=0; i<_nLength; i+=MIP4_BL_PAGE_SIZE)
	{
		pBuf = &_pBuf[i];

		if (!ReadFrom((buf_addr+i), pBuf, _nLength))
			goto error;

#if DEBUG_LOG
		printf("<<< _ReadFromBuf Data >>>\n");
		for (int j = 0; j < _nLength; j++)
		{
			printf("%02X ", pBuf[j]);
		}
		printf("\n");
#endif
	}

	return 1;

error:
	return 0;
}

int PageWrite(int _nAddr, uint8_t* _pData, int _nLength)
{
#if DEBUG_LOG
	printf("<<< PageWrite Start >>>\n");
#endif

	unsigned char write_buf[1024];
	int nAddr;
	nAddr = getAddress(MIP4_R0_BOOT, MIP4_R1_BOOT_CMD);

	if (0 == _WriteTargetAddrAndSize(_nAddr, _nLength))
		goto error;
	
	if (0 == _WriteIntoBuf(_pData, _nLength))
		goto error;

	write_buf[0] = MIP4_BOOT_CMD_WRITE;

	// if (i2c_write(fd, write_buf, 3))
	if (!WriteTo(nAddr, write_buf, 1))
		goto error;

	if (0 == bl_read_status())
		goto error;
	
	return 1;
error:	
	return 0;
}

int PageRead(int _nAddr, uint8_t* _pBuffer, int _nLength )
{
#if DEBUG_LOG
	printf("<<< PageRead Start >>>\n");
#endif

	unsigned char write_buf[1024];
	int nAddr;
	nAddr = getAddress(MIP4_R0_BOOT, MIP4_R1_BOOT_CMD);

	if (0 == _WriteTargetAddrAndSize(_nAddr, _nLength))
		goto error;

	write_buf[0] = MIP4_BOOT_CMD_READ;

	if (!WriteTo(nAddr, write_buf, 1))
		goto error;

	if (0 == bl_read_status())
		goto error;

	if (0 == _ReadFromBuf(_pBuffer, _nLength))
		goto error;

#if DEBUG_LOG
	printf("<<< PageRead End >>>\n");
#endif
	return 1;
error:
#if DEBUG_LOG
	printf("<<< PageRead End (Error) >>>\n");
#endif
	return 0;
}

int PageWriteAndVerify(int _nAddr, uint8_t* _pData, int _nLength, int _bVerify )
{
#if DEBUG_LOG
	printf ("[ PageWriteAndVerify Start ]\n");
#endif
	int nAddr;
	int nLength;
	int nRwSize;
	uint8_t *pData;
	unsigned char read_buf[1024];

	nRwSize = MIP4_BL_PAGE_SIZE;
	nLength = nRwSize;

	for (int nOffset=0; nOffset<_nLength; nOffset+=nRwSize)
	{
		nAddr = _nAddr + nOffset;
		pData = &_pData[nOffset];

		if ( (_nLength - nOffset) < nLength )
		{
			nLength = _nLength - nOffset;
		}
		
#if DEBUG_LOG
		printf("[offset : 0x%04X ]\n", nOffset);
		printf("[addr : 0x%04X ]\n", nAddr);
		printf("[len  : %d ]\n", nLength);

		for (int i = 0; i < nLength; i++)
		{
			printf("%02X ", pData[i]);
		}
		printf("\n");
#endif
		if (0 == PageWrite(nAddr, pData, nLength))
			goto error;

		/* page read */
		if (0 == PageRead(nAddr, read_buf, nLength))
			goto error;

		for (int i = 0; i < nLength; i++)
		{
			if (pData[i] != read_buf[i])
			{
				printf ("[ verify fail ]\n");
				goto error;
			}
		}
	}
#if DEBUG_LOG
	printf("<<< PageWriteAndVerify End >>>\n");
#endif
	return 1;
error:
#if DEBUG_LOG
	printf("<<< PageWriteAndVerify End (Error) >>>\n");
#endif

	return 0;
}

int excute_fw_update()
{
	unsigned char read_buf[1024];
	unsigned char write_buf[1024];
	// int nAddr;
	int nAddr;
	int nOffset;
	int nStartAddr;
	int nBinarySize;
	int nLastAddr;
	int nLength;

	int nStepSize;
	uint8_t *pBin;
	int page = 2;

	/* enter boot mode */
	printf("\nmode change to boot\n");
	if (change_mode_boot(MIP4_BOOT_MODE_BOOT))
		goto error;

	/* read info */
	nAddr = getAddress(MIP4_R0_BOOT, MIP4_R1_BOOT_BUF_ADDR);
	if (!ReadFrom(nAddr, read_buf, 2))
	{
		printf("read info error\n");
		goto error;
	}

	buf_addr = (uint16_t)((read_buf[1] & 0x00FF) << 8 | (read_buf[0] & 0xFF00) >> 8);
#if DEBUG_LOG
	printf("boot address : 0x%04X[%02X, %02X]\n", buf_addr, read_buf[0], read_buf[1]);
#endif

	/* mass erase */
	printf("mass erase\n");

	nAddr = getAddress(MIP4_R0_BOOT, MIP4_R1_BOOT_CMD);
	write_buf[0] = MIP4_BOOT_CMD_MASS_ERASE;
	WriteTo(nAddr, write_buf, 1);

	if (!bl_read_status())
	{
		printf("ready status error! [%d]", __LINE__);
		goto error;
	}

	/* write & verify */
	pBin = pfwBuf;

	nStartAddr = fw_tail.bin_start_addr;
	nBinarySize = fw_tail.bin_length;
	nLastAddr = nStartAddr + nBinarySize;

	nStepSize = MIP4_BL_PAGE_SIZE;

	nLength = nStepSize;

#if DEBUG_LOG
	printf("%02X ", pBin[0]);
	for (int i = 1; i < binfile_length; i++)
	{
		printf("%02X ", pBin[i]);
		if (((i + 1) % 16 == 0))
			printf("\n");
	}
	printf("\n");
#endif

#if DEBUG_LOG
	printf(("=========================\n"));
	printf((">>> Download Flow sample\n"));
	printf(("=========================\n"));
	printf(("> Start address   : 0x%04X\n"), nStartAddr);
	printf(("> Last address    : 0x%04X\n"), nLastAddr);
	printf(("> Actual size     : 0x%04X\n"), nBinarySize);
	printf(("> Erase size      : 0x%04X\n"), nStepSize);
	// printf(("> Read Write size : 0x%04X\n"), mSpec.GetRwSize());
	// printf(("> Split size      : 0x%04X\n"), mSpec.GetRwSplitSize());
	printf(("=========================\n"));
	printf(("\n"));

	// goto error;
#endif

	//------------------------------------
	// 2nd page ~ last page
	//------------------------------------
	page = 0;
	for (nAddr=(nStartAddr+nStepSize); nAddr<nLastAddr; nAddr+=nLength)
	{
		nOffset = nAddr - nStartAddr;
		dl_step = nOffset/4;
		dl_total = nBinarySize/4;
		if ( (nLastAddr - nAddr) < nLength )
		{
			nLength = nLastAddr - nAddr;
		}

#if DEBUG_LOG
		printf("[addr : 0x%06X]\n", nAddr);
		printf("[page : %d / offset : 0x%04X ]\n", page, nOffset);
#endif
		if (0 == PageWriteAndVerify(nAddr, &pBin[nOffset], nLength, 1))
			goto error;

		if (0 == thread_flag)
			goto error;
		page++;
	}

	//------------------------------------
	// The 1st page ~ last page
	//------------------------------------

	nAddr   = nStartAddr;
	nLength = nStepSize;
	nOffset = 0;

	if( (nLastAddr - nAddr) < nLength )
		nLength = nLastAddr - nAddr;

	if (0 == PageWriteAndVerify(nAddr, &pBin[0], nLength, 1))
		goto error;

	dl_step = nBinarySize / 4;
	dl_total = nBinarySize / 4;

	usleep(10 * 1000);

	page++;
#if DEBUG_LOG
	printf("\npage : %d\n", page);
#endif

	/* exit boot mode */
	printf("\nmode change to app\n\n");
	change_mode_boot(MIP4_BOOT_MODE_APP);

	//
	memset(read_buf, 0x00, sizeof(read_buf));
	ReadFrom(0x0120, read_buf, 8);

	printf("[New Version]\n");
	printf("%02X%02X.%02X%02X.%02X%02X.%02X%02X\n\n", read_buf[1], read_buf[0],read_buf[3], read_buf[2],read_buf[5], read_buf[4], read_buf[7], read_buf[6]);

	return 1;

error:
	if (0 != thread_flag)
	{
		thread_flag = 0;
	}
	usleep(500 * 1000);

	/* exit boot mode */
	printf("mode change to app (error)\n\n");
	change_mode_boot(MIP4_BOOT_MODE_APP);

	return 0;
}