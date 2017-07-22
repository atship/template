#define _CRT_SECURE_NO_WARNINGS
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "nativewin.h"

#include <iostream>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "../../templateApp.h"

class esContext
{
public:
    esContext() :
        nativeDisplay(0), nativeWin(0),
        eglDisplay(0), eglSurface(0), eglContext(0), 
        nWindowWidth(0), nWindowHeight(0), nMouseX(0), nMouseY(0)
    {}

    ~esContext() {}

    EGLNativeDisplayType nativeDisplay;
    EGLNativeWindowType nativeWin;
    EGLDisplay eglDisplay;
    EGLSurface eglSurface;
    EGLContext eglContext;

    int         nWindowWidth;
    int         nWindowHeight;
    int         nMouseX;
    int         nMouseY;
};

#pragma pack(8)

esContext ctx;

using namespace std;

void OnNativeWinResize(int width, int height)
{
    if (ctx.nativeWin != NULL)
    {
        ctx.nWindowWidth = width;
        ctx.nWindowHeight = height;
        //glViewport(0, 0, width, height);
    }
}

void OnNativeWinMouseMove(int mousex, int mousey, bool lbutton)
{
	if (lbutton) {
		templateApp.ToucheMoved(mousex, mousey, 1);
	}
}

void OnNativeWinMouseDown(int mousex, int mousey, bool lbutton)
{
	if (lbutton) {
		templateApp.ToucheBegan(mousex, mousey, 1);
	}
}

void OnNativeWinMouseUp(int mousex, int mousey, bool lbutton)
{
	if (lbutton) {
		templateApp.ToucheEnded(mousex, mousey, 1);
	}
}

EGLBoolean Setup(esContext &ctx)
{
    EGLBoolean bsuccess;

    // create native window
    EGLNativeDisplayType nativeDisplay;
    if(!OpenNativeDisplay(&nativeDisplay))
    {
        printf("Could not get open native display\n");
        return GL_FALSE;
    }

	EGLNativeWindowType nativeWin;
	if (!CreateNativeWin(640, 480, &nativeWin))
	{
		printf("Could not create window\n");
		CloseNativeDisplay(nativeDisplay);
		return GL_FALSE;
	}

	nativeDisplay = GetDC(nativeWin);

    // get egl display handle
    EGLDisplay eglDisplay;
    eglDisplay = eglGetDisplay(nativeDisplay);
    if(eglDisplay == EGL_NO_DISPLAY)
    {
        printf("Could not get EGL display\n");
        CloseNativeDisplay(nativeDisplay);
        return GL_FALSE;
    }
    ctx.eglDisplay = eglDisplay;

    // Initialize the display
    EGLint major = 0;
    EGLint minor = 0;
    bsuccess = eglInitialize(eglDisplay, &major, &minor);
    if (!bsuccess)
    {
        printf("Could not initialize EGL display\n");
        CloseNativeDisplay(nativeDisplay);
        return GL_FALSE;
    }
    if (major < 1 || minor < 4)
    {
        // Does not support EGL 1.4
        printf("System does not support at least EGL 1.4\n");
        CloseNativeDisplay(nativeDisplay);
        return GL_FALSE;
    }

    // Obtain the first configuration with a depth buffer
    EGLint attrs[] = { EGL_DEPTH_SIZE, 16, EGL_NONE };
    EGLint numConfig =0;
    EGLConfig eglConfig = 0;
    bsuccess = eglChooseConfig(eglDisplay, attrs, &eglConfig, 1, &numConfig);
    if (!bsuccess)
    {
        printf("Could not find valid EGL config\n");
        CloseNativeDisplay(nativeDisplay);
        return GL_FALSE;
    }

    // Get the native visual id
    int nativeVid;
    if (!eglGetConfigAttrib(eglDisplay, eglConfig, EGL_NATIVE_VISUAL_ID, &nativeVid))
    {
        printf("Could not get native visual id\n");
        CloseNativeDisplay(nativeDisplay);
        return GL_FALSE;
    }

    // Create a surface for the main window
    EGLSurface eglSurface;
    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, nativeWin, NULL);
    if (eglSurface == EGL_NO_SURFACE)
    {
        printf("Could not create EGL surface\n");
        DestroyNativeWin(nativeDisplay, nativeWin);
        CloseNativeDisplay(nativeDisplay);
        return GL_FALSE;
    }
    ctx.eglSurface = eglSurface;

    // Create an OpenGL ES context
    EGLContext eglContext;
    EGLint ctxattrs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, ctxattrs);
    if (eglContext == EGL_NO_CONTEXT)
    {
        printf("Could not create EGL context\n");
        DestroyNativeWin(nativeDisplay, nativeWin);
        CloseNativeDisplay(nativeDisplay);
        return GL_FALSE;
    }

    // Make the context and surface current
    bsuccess = eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    if(!bsuccess)
    {
        printf("Could not activate EGL context\n");
        DestroyNativeWin(nativeDisplay, nativeWin);
        CloseNativeDisplay(nativeDisplay);
        return GL_FALSE;
    }

    ctx.eglContext = eglContext;
    ctx.nativeDisplay = nativeDisplay;
    ctx.nativeWin = nativeWin;

    // make sure an initial resize event is provided to the application
    //OnNativeWinResize(640, 480);
    
    return GL_TRUE;
}

int main(int argc, char** argv)
{
    ctx.nWindowWidth  = 640;
    ctx.nWindowHeight = 480;
    int lRet = 0;

    // create window and setup egl
    if(Setup(ctx) == GL_FALSE)
    {
        return lRet;
    }

	templateApp.Init(640, 480);

    // main loop
    while (UpdateNativeWin(ctx.nativeDisplay, ctx.nativeWin))
    {
		templateApp.Draw();
		eglSwapBuffers(ctx.eglDisplay, ctx.eglSurface);
    }

    eglMakeCurrent(EGL_NO_DISPLAY, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(ctx.eglDisplay, ctx.eglContext);
    eglDestroySurface(ctx.eglDisplay, ctx.eglSurface);
    eglTerminate(ctx.eglDisplay);
    DestroyNativeWin(ctx.nativeDisplay, ctx.nativeWin);
    CloseNativeDisplay(ctx.nativeDisplay);

    return lRet;
}
