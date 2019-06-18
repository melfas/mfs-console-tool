
#ifndef __CS_CONFIG_H__
#define __CS_CONFIG_H__

#define ACTION_NULL					0x00
#define ACTION_READ 				0x01
#define ACTION_WRITE				0x02
#define ACTION_TEST	    		0x03
#define ACTION_HELP         0x04
#define ACTION_DOWNLOAD     0x05
#define ACTION_BL_MODE      0x06
#define ACTION_APP_MODE     0x07
#define ACTION_FW_VERSION   0x08

class mfsConfig
{
public:

	mfsConfig();
	~mfsConfig();

protected:

	int	mnAction;
	int	mbLogFile;
  char* pSpecName;
  int nPid;

public:

  //--------------------------------
  //
  //--------------------------------

  void Clear(void);

  //--------------------------------
  //
  //--------------------------------

  void ParseArgs(int argc, char *argv[]);

  //--------------------------------
  //
  //--------------------------------

  int GetAction(void);

  bool GetLogFileEnabled(void);
  char *GetSpecFileName(void);

  void PrintIniInfo(void);

  int GetPid(void);
};
#endif