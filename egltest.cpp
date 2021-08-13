#include <GL/glew.h>
#include <EGL/egl.h>
#include <shader.hpp>

#include <iostream>

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>


EGLint const configAttribs[] = {
          EGL_CONFORMANT,EGL_OPENGL_ES3_BIT, 
          EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
          EGL_BLUE_SIZE, 8,
          EGL_GREEN_SIZE, 8,
          EGL_RED_SIZE, 8,
          EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
          EGL_NONE
};

EGLint const attrib_list[] = {
        EGL_RED_SIZE, 4,
        EGL_GREEN_SIZE, 4,
        EGL_BLUE_SIZE, 4,
        EGL_NONE
};

static const int pbufferWidth = 512;
static const int pbufferHeight = 512;

static const EGLint pbufferAttribs[] = {
      EGL_WIDTH, pbufferWidth,
      EGL_HEIGHT, pbufferHeight,
      EGL_NONE,
};


int main(int argc, char **argv)
{
  EGLDisplay display = eglGetDisplay( EGL_DEFAULT_DISPLAY ) ;
  if( display == EGL_NO_DISPLAY )
  {
      std:: cout << "ERROR: EGL could not be initialized"<< std::endl;
      exit(EXIT_FAILURE);
  }

    
  EGLint major, minor;
  if( eglInitialize( display, &major, &minor ) != EGL_TRUE )
  {
      std:: cout << "ERROR: Could not start EGL display connection"<< std::endl;
      exit(EXIT_FAILURE);
  }

  EGLint numConfigs;
  EGLConfig eglCfg;

  if( eglChooseConfig(display, configAttribs, &eglCfg, 1, &numConfigs) != EGL_TRUE )
  {
      std:: cout << "ERROR: Configuration selection failed" << std::endl;
      exit(EXIT_FAILURE);
  }
  if( numConfigs == 0 )
  {
      std:: cout << "ERROR: No configurations" << std::endl;
      exit(EXIT_FAILURE);
  }

  EGLSurface eglSurf = eglCreatePbufferSurface(display, eglCfg,pbufferAttribs);

  eglBindAPI( EGL_OPENGL_API );
  EGLContext context = eglCreateContext( display, eglCfg, EGL_NO_CONTEXT, NULL );
  
  if( eglMakeCurrent(display, eglSurf, eglSurf, context) != EGL_TRUE )
  {
      std:: cout << "ERROR: Display was not made current one"<< std::endl;
      exit(EXIT_FAILURE);
  }

  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
      std:: cout << "GLEW Error: " << glewGetErrorString(err) << std::endl;
      exit(EXIT_FAILURE);
  }

  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  static const GLfloat g_vertex_buffer_data[] = { 
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f,
	};


  int width = 512;
  int height = 512; 

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

  GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f(0.5f, 0.5f, 0.5f);
  glViewport(0,0,width,height);

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
\
  eglSwapBuffers(display,eglSurf);
  glFlush();

  GLubyte *m_glFrameBuffer = (GLubyte*) malloc ( 3 * sizeof(GLubyte) * width * height);

  std::string fileimg = "test.png";
	
	GLsizei nrChannels = 3;
	GLsizei stride = nrChannels * width;
	
	GLsizei bufferSize = stride * height;
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, m_glFrameBuffer);
	stbi_flip_vertically_on_write(true);
	stbi_write_png(fileimg.c_str(), width, height, nrChannels, m_glFrameBuffer, stride);

  ////////////////
  free(m_glFrameBuffer);

  std::cout << glGetString( GL_VERSION ) << std::endl;
  std::cout << glGetString( GL_VENDOR ) << std::endl;
  std::cout << glGetString( GL_RENDERER ) << std::endl;
  std::cout << ""

    return 0;
}