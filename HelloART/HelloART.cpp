// HelloART.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <AR/ar.h>
#include <stdio.h>

int _tmain(int argc, _TCHAR* argv[])
{
	char* pVersion;
	arGetVersion(&pVersion);
	printf("Hello ARToolKit ver. %s\r\n", pVersion);
	free(pVersion);

	return 0;
}

