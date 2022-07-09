#ifndef __OPEN_GL_H__
#define __OPEN_GL_H__

#if defined(__APPLE__)
#  ifdef GL_ES_VERSION_2_0
#    include <OpenGLES/ES2/gl.h>
#  else
#    include <OpenGL/gl.h>
#  endif
#elif defined(_WIN32) || defined(_WIN64)
#  include <glad/glad.h>
#else
#  include <glad/glad.h>
#endif

#endif /* OPEN_GL_H */
