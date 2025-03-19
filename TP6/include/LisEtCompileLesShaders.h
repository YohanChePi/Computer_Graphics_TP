#ifndef _LISETCOMPILELESSHADERS_
#define _LISETCOMPILELESSHADERS_

#include <GL/glew.h>

GLuint LisEtCompileLesShaders(const char * vertex_file_path, const char *tess_control_file_path, const char *tess_eval_file_path, const char *geom_file_path, const char * fragment_file_path);

#endif
