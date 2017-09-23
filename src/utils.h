#ifndef _utils_h
#define _utils_h

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>

#include "gl.h"

char* file_read(const char* filename, int* size);
void print_log(GLuint object);
GLuint create_shader(const char* filename, GLenum type);
GLuint create_gs_program(const char *vertexfile, const char *geometryfile, const char *fragmentfile, GLint input, GLint output, GLint vertices);
GLint get_attrib(GLuint program, const char *name);
GLint get_uniform(GLuint program, const char *name);
void print_opengl_info();

const char *info_get_sdl_event_type(int type);
void info_sdl_event_window(SDL_WindowEvent *pEvent);


#endif
