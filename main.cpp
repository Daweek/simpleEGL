#define EGL_EGLEXT_PROTOTYPES
#define GLEW_EGL
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <filesystem>




//#include <GL/glew.h>
//
#include "GL/glew.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>

//#include <GL/gl.h>
#include "shader.hpp"
//#include <glm/glm.hpp>
using namespace std;

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

//include <shader.hpp>
static const EGLint configAttribs[] = {
          EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
          EGL_BLUE_SIZE, 8,
          EGL_GREEN_SIZE, 8,
          EGL_RED_SIZE, 8,
          EGL_DEPTH_SIZE, 8,
          EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
          EGL_NONE
};    

  static const int pbufferWidth = 512;
  static const int pbufferHeight = 512;

  static const EGLint pbufferAttribs[] = {
        
        EGL_WIDTH, pbufferWidth,
        EGL_HEIGHT, pbufferHeight,
        EGL_NONE,
  };

int main(int argc, char *argv[])
{
  // 1. Initialize EGL
  EGLDisplay eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

  EGLint major, minor;

  eglInitialize(eglDpy, &major, &minor);

  cout<<"EGL Major: "<<major<<"  Minor: "<<minor<<endl;
  // 2. Select an appropriate configuration
  EGLint numConfigs;
  EGLConfig eglCfg;

  //eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);
  
  if( eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs)!= EGL_TRUE )
  {
      std:: cout << "ERROR: Configuration selection failed" << std::endl;
      exit(EXIT_FAILURE);
  }
  if( numConfigs == 0 )
  {
      std:: cout << "ERROR: No configurations" << std::endl;
      exit(EXIT_FAILURE);
  }

  // 3. Create a surface
  EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg,pbufferAttribs);

  // 4. Bind the API
  eglBindAPI(EGL_OPENGL_API);

  // 5. Create a context and make it current
  EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT,NULL);

  eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);

  //glewExperimental = true;
	if (glewInit() != GLEW_OK)
		assert(!"GLEW initialization\n");
	if(!glewIsSupported("GL_EXT_framebuffer_object"))
		assert(!"The GL_EXT_framebuffer_object extension is required.\n");


  static const int MAX_DEVICES = 4;
  EGLDeviceEXT eglDevs[MAX_DEVICES];
  EGLint numDevices;

  PFNEGLQUERYDEVICESEXTPROC eglQueryDevicesEXT =
    (PFNEGLQUERYDEVICESEXTPROC)
    eglGetProcAddress("eglQueryDevicesEXT");

  printf("Detected %d devices\n", numDevices);

  eglQueryDevicesEXT(MAX_DEVICES, eglDevs, &numDevices);

  

  // from now on use your OpenGL context
  int width = 512;
  int height = 512; 
  
  glClearColor(0.5f, 0.5f, 0.0f, 1.0f);

  cout<<"Here is the bug"<<endl;  
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
 

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );
  

	static const GLfloat g_vertex_buffer_data[] = { 
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	//do{

		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT );

		// Use our shader
		glUseProgram(programID);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);

    glDeleteBuffers(1, &vertexbuffer);
	  glDeleteVertexArrays(1, &VertexArrayID);
	  glDeleteProgram(programID);

    glFlush();
   
  GLubyte *m_glFrameBuffer = (GLubyte*) malloc ( 3 * sizeof(GLubyte) * width * height);

  std::string fileimg = "test.png";
	
	GLsizei nrChannels = 3;
	GLsizei stride = nrChannels * width;
	
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, m_glFrameBuffer);
	stbi_flip_vertically_on_write(true);
	stbi_write_png(fileimg.c_str(), width, height, nrChannels, m_glFrameBuffer, stride);

  ////////////////
  free(m_glFrameBuffer);

  std::cout << glGetString( GL_VERSION ) << std::endl;
  std::cout << glGetString( GL_VENDOR ) << std::endl;
  std::cout << glGetString( GL_RENDERER ) << std::endl;
  // 6. Terminate EGL when finished
  eglTerminate(eglDpy);

  return 0;
}