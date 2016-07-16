
#pragma once

#include <vector>
#include "vgl.h"

typedef struct {
	GLenum       type;
	const char*  filename;
	GLuint       shader;
} ShaderInfo;

GLuint loadShaders(ShaderInfo*);

long getTime();

// 线性插值
float interValue(DWORD firstP, float first, DWORD secondP, float second, DWORD interP);
void slerp(float result[4], float starting[4], float ending[4], float t);
// 内存逆置
DWORD reverseDWord(DWORD DWord);

DWORD readDWORD(FILE *handle);

GLuint readToBuffer(FILE *handle, GLuint size, GLenum type);

void bindToVAO(GLuint vao, GLuint index, void *data, GLuint size, GLenum type, GLuint typeSize);

GLuint writeToTBO(void *data, GLuint size, GLenum type, GLenum texIndex);
