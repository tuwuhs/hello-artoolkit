// HelloART.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <AR/ar.h>
#include <AR/video.h>
#include <AR/gsub_lite.h>
#include <GL/glut.h>
#include <stdio.h>

static ARHandle* g_pARHandle = NULL;
static ARGL_CONTEXT_SETTINGS_REF g_pArglSettings = NULL;

static ARUint8 *g_pARTImage = NULL;

static void Display()
{
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	arglPixelBufferDataUpload(g_pArglSettings, g_pARTImage);
	arglDispImage(g_pArglSettings);

	glutSwapBuffers();
}

static void MainLoop()
{
	ARUint8* pImage;

	pImage = arVideoGetImage();
	if (pImage) {
		g_pARTImage = pImage;
		glutPostRedisplay();
	}
}

int main(int argc, char* argv[])
{
	char* pVersion;
	arGetVersion(&pVersion);
	printf("Hello ARToolKit ver. %s\r\n", pVersion);
	free(pVersion);

	// Setup camera and AR handle
	arVideoOpen(NULL);
	//arVideoOpen("-device=Image");

	int xsize, ysize;
	arVideoGetSize(&xsize, &ysize);
	AR_PIXEL_FORMAT pixformat = arVideoGetPixelFormat();

	ARParam cparam;
	arParamLoad("camera_para.dat", 1, &cparam);
	printf("cparam xsize: %d ysize: %d\r\n", cparam.xsize, cparam.ysize);
	if (cparam.xsize != xsize || cparam.ysize != ysize) {
		arParamChangeSize(&cparam, xsize, ysize, &cparam);
	}

	ARParamLT* pCparamLT = arParamLTCreate(&cparam, AR_PARAM_LT_DEFAULT_OFFSET);

	g_pARHandle = arCreateHandle(pCparamLT);
	arSetPixelFormat(g_pARHandle, pixformat);
	arVideoCapStart();

	// Setup GLUT and gsub_lite
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(xsize, ysize);
	glutCreateWindow(argv[0]);

	g_pArglSettings = arglSetupForCurrentContext(&(pCparamLT->param), pixformat);
	arglSetupDebugMode(g_pArglSettings, g_pARHandle);
	arUtilTimerReset();

	glutDisplayFunc(Display);
	glutIdleFunc(MainLoop);

	glutMainLoop();

	arDeleteHandle(g_pARHandle);

	return 0;
}

