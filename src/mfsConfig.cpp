
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mfsConfig.h"
//--------------------------------------------------
//
//--------------------------------------------------

mfsConfig::mfsConfig()
{
	Clear();
}

mfsConfig::~mfsConfig()
{
}

void mfsConfig::Clear(void)
{
	mnAction  = ACTION_NULL;
	mbLogFile = false;
}


//--------------------------------------------------
//
//--------------------------------------------------


void mfsConfig::ParseArgs(int argc, char* argv[])
{
	Clear();

    // printf("start ParseArgs\n");
	//-------------------------------------------
	// no parameter
	//-------------------------------------------
	if( argc==1 )
		return;

	//-------------------------------------------
	// parse
	//-------------------------------------------

	mnAction = ACTION_NULL;

	for(int i=0;i<argc;i++)
	{
		char *pParam = argv[i];
		// size_t len = strlen(pParam);

        // printf("arg [%d] : %s\n", i, pParam);

		//-------------------------
		// Option
		//-------------------------

		if( pParam[0] == '-' )
		{
            if (strcmp(pParam, "-fw_update") == 0)
            {
                mnAction = ACTION_DOWNLOAD;
                nPid = strtol((char *)argv[2], NULL, 16);
                printf("\nPID : 0x%04X\n", nPid);
                pSpecName = (char *)argv[3];
                printf("fw file name : %s\n", pSpecName);
                return;
            }
			if (strcmp(pParam, "-fw_version") == 0)
			{
				mnAction = ACTION_DOWNLOAD;
				nPid = strtol((char *)argv[2], NULL, 16);
				printf("\nPID : 0x%04X\n", nPid);
				return;
			}
			else if( strcmp(pParam, "-help")==0 )
			{
				mnAction = ACTION_HELP;
				return;
			}            
        }
    }

//	printf("SDFKJSLDKFJSDLFKSDF nAction : %d\n\n", mnAction);

	return;
}

//--------------------------------------------------
//
//--------------------------------------------------

int mfsConfig::GetAction(void)
{
	return mnAction;
}

bool mfsConfig::GetLogFileEnabled(void)
{
	return mbLogFile;
}

char* mfsConfig::GetSpecFileName(void)
{
    return pSpecName;
}

int mfsConfig::GetPid(void)
{
    return nPid;
}