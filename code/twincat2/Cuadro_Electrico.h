///////////////////////////////////////////////////////////////////////////////
// 
// Beckhoff Automation GmbH
// 
// TwinCAT IO HeaderFile
// 
///////////////////////////////////////////////////////////////////////////////
// 
// D:\Cuadro_Electrico.h

#define	CUADRO_ELECTRICO_PORTNUMBER	302

#define	CUADRO_ELECTRICO_INPUTSIZE	9
#define	CUADRO_ELECTRICO_OUTPUTSIZE	7

#pragma pack(push, 1)

typedef struct
{
	unsigned char	OUTPUTS_SOFT[9];
} Cuadro_Electrico_Inputs, *PCuadro_Electrico_Inputs;

typedef struct
{
	unsigned char	INPUTS_SOFT[7];
} Cuadro_Electrico_Outputs, *PCuadro_Electrico_Outputs;

#pragma pack(pop)
