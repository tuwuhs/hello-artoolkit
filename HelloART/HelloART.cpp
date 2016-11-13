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

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, (GLdouble)g_pARHandle->xsize, 0, (GLdouble)g_pARHandle->ysize, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	GLfloat vertices[6][2];
	glLoadIdentity();
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glEnableClientState(GL_VERTEX_ARRAY);
	glLineWidth(2.0f);
	for (int j = 0; j < g_pARHandle->marker_num; j++) {
		ARMarkerInfo* pMarker = &(g_pARHandle->markerInfo[j]);
		if (pMarker->cutoffPhase != AR_MARKER_INFO_CUTOFF_PHASE_NONE) {
			continue;
		}
		int i;
		for (i = 0; i < 5; i++) {
			int dir = pMarker->dir;
			vertices[i][0] = (float)pMarker->vertex[(i + 4 - dir) % 4][0];
			vertices[i][1] = ((float)g_pARHandle->ysize - (float)pMarker->vertex[(i + 4 - dir) % 4][1]);
		}
		vertices[i][0] = (float)pMarker->pos[0];
		vertices[i][1] = ((float)g_pARHandle->ysize - (float)pMarker->pos[1]);
		glDrawArrays(GL_LINE_STRIP, 0, 6);
	}
	glDisableClientState(GL_VERTEX_ARRAY);

	glutSwapBuffers();
}

static void MainLoop()
{
	ARUint8* pImage;

	pImage = arVideoGetImage();
	if (pImage) {
		g_pARTImage = pImage;
		arDetectMarker(g_pARHandle, g_pARTImage);
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
	arSetDebugMode(g_pARHandle, AR_DEBUG_DISABLE);
	arVideoCapStart();

	// Setup AR
	//arSetPatternDetectionMode(g_pARHandle, AR_DEFAULT_PATTERN_DETECTION_MODE);
	arSetPatternDetectionMode(g_pARHandle, AR_MATRIX_CODE_DETECTION);
	arSetLabelingMode(g_pARHandle, AR_DEFAULT_LABELING_MODE);
	arSetPattRatio(g_pARHandle, AR_PATT_RATIO);
	arSetMatrixCodeType(g_pARHandle, AR_MATRIX_CODE_TYPE_DEFAULT);

	// Setup GLUT and gsub_lite
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(xsize, ysize);
	glutCreateWindow(argv[0]);

	g_pArglSettings = arglSetupForCurrentContext(&(pCparamLT->param), pixformat);
	arglSetupDebugMode(g_pArglSettings, g_pARHandle);

	// Register GLUT functions
	glutDisplayFunc(Display);
	glutIdleFunc(MainLoop);

	// The main loop
	glutMainLoop();

	// Clean up
	arDeleteHandle(g_pARHandle);

	return 0;
}

