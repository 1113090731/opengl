
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

// ���Բ�ֵ
float interValue(DWORD firstP, float first, DWORD secondP, float second, DWORD interP);
void slerp(float result[4], float starting[4], float ending[4], float t);
// �ڴ�����
DWORD reverseDWord(DWORD DWord);

DWORD readDWORD(FILE *handle);

GLuint readToBuffer(FILE *handle, GLuint size, GLenum type);

void bindToVAO(GLuint vao, GLuint index, void *data, GLuint size, GLenum type, GLuint typeSize);

GLuint writeToTBO(void *data, GLuint size, GLenum type, GLenum texIndex);
