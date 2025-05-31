//---------------------------------------------------------------------------

#ifndef RaydenBoardH
#define RaydenBoardH
//---------------------------------------------------------------------------

#include "TComunic.h"
#include "TDataMngControl.h"

enum  class RaydenBoardStatus: int {
   UNKOWN=-1,
   OFF=0,
   CFG=1,
   RUN=2
};
enum  class RaydenBoardNextCmd : int {
   NOTHING=-1,
   SETOFF=0,
   SETCFG=1,
   SETRUN=2
};
enum  class RaydenBoardLastCmdResult: int {
   NOTOK=0,
   OK=1
};

DWORD WINAPI RaydenBoardThread(LPVOID owner);

class RaydenBoard : public TDataManagerControl  {


    public:
    RaydenBoard( const wchar_t *name, TDataManagerControl *_parent = NULL);
    virtual ~RaydenBoard();
    virtual bool MyInit();
    virtual bool MyGo();
    virtual bool MyStop();
	virtual bool MyClose();
	void __fastcall DataModifiedPost( TData* da);

    bool StartThread();
    void StopThread();

//	bool SetNewCfg( char *cfg);

//	const char *GetCfg(){
//	  char *cfg = new char[1000];
//      unsigned int size = sizeof( cfg);
//	  if ( m_pNewCfgbyDatas->AsText( cfg, size)) {
//         return cfg;
//	  }
//	  return NULL;
//	}

    bool SetRunStatus() {
      return SetStatus( RaydenBoardStatus::RUN);
    }

//    bool SetCfgStatus() {
//      return SetStatus( CameraIlumDriverStatus::CFG);
//    }

    bool SetOffStatus() {
      return SetStatus( RaydenBoardStatus::OFF);
    }

          protected:
   TCom232           *m_com232;              ///< Serial Port Object.

   HANDLE            m_hthread,              ///< Handle to thread.
                     m_evStopThread,         ///< Handle for stop the thread
                     m_evSetStatus;          ///< Handle for launch a set Status.

   TData             *m_pStatus,
                     *m_pNextCmd,
					 *m_pNewCfgbyString,
					 *m_pNewCfgbyDatas,
					 *m_pLastCmdResult;


   TData           *m_typeNewCfg,
				   *m_cycleTime,
				   *m_numIlus,
				   *m_numSec,
				   *m_nameCam,
				   *m_offsetCam,
				   *m_TimeCam,

				   *m_name_Sec1Ilu,
				   *m_num_Sec1Ilu,
				   *m_offset_Sec1Ilu,
				   *m_time_Sec1Ilu,

				   *m_name_Sec2Ilu,
				   *m_num_Sec2Ilu,
				   *m_offset_Sec2Ilu,
				   *m_time_Sec2Ilu,

				   *m_name_Sec3Ilu,
				   *m_num_Sec3Ilu,
				   *m_offset_Sec3Ilu,
				   *m_time_Sec3Ilu,

				   *m_name_Sec4Ilu,
				   *m_num_Sec4Ilu,
				   *m_offset_Sec4Ilu,
				   *m_time_Sec4Ilu,

				   *m_name_Sec5Ilu,
				   *m_num_Sec5Ilu,
				   *m_offset_Sec5Ilu,
				   *m_time_Sec5Ilu,

				   *m_name_Sec6Ilu,
				   *m_num_Sec6Ilu,
				   *m_offset_Sec6Ilu,
				   *m_time_Sec6Ilu,

				   *m_name_Sec7Ilu,
				   *m_num_Sec7Ilu,
				   *m_offset_Sec7Ilu,
				   *m_time_Sec7Ilu,

				   *m_name_Sec8Ilu,
				   *m_num_Sec8Ilu,
				   *m_offset_Sec8Ilu,
				   *m_time_Sec8Ilu;


   protected:
    bool SendParam( char comando[], char resp[], int size, int nIntentos);
    bool SetStatus( RaydenBoardStatus newStatus);
    bool  RaydenBoard::SendCmd();
    RaydenBoardStatus GetStatus();
	bool SendCmdToRaydenBoard (const char *status, bool checkSum=false);
    friend DWORD WINAPI RaydenBoardThread(LPVOID owner);
    bool SendSpecifiedCmd(RaydenBoardNextCmd cmd);
	char *StringForCmd( RaydenBoardNextCmd cmd);
	unsigned char CheckSum( const char *characters);

	void FillCfgStringbyDatas();
};
#endif
