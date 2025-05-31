///////////////////////////////////////////////////////////////////////////////
// 
// Beckhoff Automation GmbH
// 
// TwinCAT IO HeaderFile
// 
///////////////////////////////////////////////////////////////////////////////
// 
// F:\Driver_EtherCAT.h

#define	DRIVER_ETHERCAT_PORTNUMBER	301

#define	DRIVER_ETHERCAT_INPUTSIZE	2105
#define	DRIVER_ETHERCAT_OUTPUTSIZE	3111

#pragma pack(push, 1)

typedef struct
{
	unsigned char	TRIG_DATA[1560];
	unsigned char	BASETIME_DATA[37];
	unsigned char	STROB_DATA[300];
	unsigned char	PUTEXP_DATA[201];
	unsigned short	numTerminales;
	unsigned short	falloTerminales;
	unsigned short	codeVersion;
	unsigned char	masterOk	: 1;
} Driver_EtherCAT_Inputs, *PDriver_EtherCAT_Inputs;

typedef struct
{
	unsigned char	TRIG_CFG[104];
	unsigned char	BASETIME_CFG;
	unsigned char	STROB_CFG[150];
	unsigned char	reserved1[951];
	unsigned char	mode;
	unsigned char	divisorEncTrigStrob;
	unsigned char	divisorExp;
	unsigned char	status;
	unsigned char	PUTEXP_CFG[1901];
} Driver_EtherCAT_Outputs, *PDriver_EtherCAT_Outputs;

#pragma pack(pop)
