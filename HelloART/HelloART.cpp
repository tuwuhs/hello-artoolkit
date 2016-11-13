// HelloART.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <AR/ar.h>
#include <AR/video.h>
#include <stdio.h>

static ARHandle* g_pARHandle;

int _tmain(int argc, _TCHAR* argv[])
{
	char* pVersion;
	arGetVersion(&pVersion);
	printf("Hello ARToolKit ver. %s\r\n", pVersion);
	free(pVersion);

	//arVideoOpen("-device=Image");
	
	ARParam cparam;
	arParamLoad("camera_para.dat", 1, &cparam);
	printf("cparam xsize: %d ysize: %d\r\n", cparam.xsize, cparam.ysize);
	// TODO: need to set size according to input file

	ARParamLT cparamLT;
	arParamLTCreate(&cparam, AR_PARAM_LT_DEFAULT_OFFSET);

	g_pARHandle = arCreateHandle(&cparamLT);

	arDeleteHandle(g_pARHandle);

	return 0;
}

