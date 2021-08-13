#include <GL/gl.h>
#include <EGL/egl.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    EGLDisplay eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    assert(eglDpy != EGL_NO_DISPLAY);

    EGLint major, minor;
    EGLBoolean result = eglInitialize(eglDpy, &major, &minor);
    assert(result);
    printf("egl %i.%i\n", major, minor);

    static const EGLint configAttribs[] = {EGL_SURFACE_TYPE,
                                           EGL_PBUFFER_BIT,
                                           EGL_BLUE_SIZE,
                                           8,
                                           EGL_GREEN_SIZE,
                                           8,
                                           EGL_RED_SIZE,
                                           8,
                                           EGL_DEPTH_SIZE,
                                           8,
                                           EGL_RENDERABLE_TYPE,
                                           EGL_OPENGL_BIT,
                                           EGL_NONE};

    EGLint numConfigs;
    EGLConfig eglCfg;
    result = eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);
    assert(result);
    assert(numConfigs >= 1);

    static const EGLint pbufferAttribs[] = {
        EGL_WIDTH, 128, EGL_HEIGHT, 128, EGL_NONE,
    };

    EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg, pbufferAttribs);
    assert(eglSurf != EGL_NO_SURFACE);

    result = eglBindAPI(EGL_OPENGL_API);
    assert(result);

    EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT, NULL);
    assert(eglCtx != EGL_NO_CONTEXT);

    result = eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);
    assert(result);

    const char* renderer = (const char*)glGetString(GL_RENDERER);
    assert(renderer);
    printf("renderer: %s\n", renderer);

    const char* version = (const char*)glGetString(GL_VERSION);
    assert(version);
    printf("version: %s\n", version);

    float c = 250.0f / 255.0f;
    glClearColor(c, c, c, 1);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glFlush();
    GLenum error = glGetError();
    assert(error == GL_NO_ERROR);

    eglSwapBuffers(eglDpy, eglSurf);
    eglWaitClient();
    eglWaitGL();

    result = eglMakeCurrent(eglDpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    assert(result);

    result = eglDestroyContext(eglDpy, eglCtx);
    assert(result);

    result = eglDestroySurface(eglDpy, eglSurf);
    assert(result);

    result = eglTerminate(eglDpy);
    assert(result);

    return 0;
}