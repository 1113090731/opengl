#include "util.h"

GLchar* readShader(const char* filename){
	FILE* handle;
	fopen_s(&handle, filename, "rb");
	if (!handle){
		printf("Unable to open file '%s'\n", filename);
		return NULL;
	}

	fseek(handle, 0, SEEK_END);
	int len = ftell(handle);
	fseek(handle, 0, SEEK_SET);

	GLchar *source = new GLchar[len + 1];
	fread(source, 1, len, handle);
	fclose(handle);
	source[len] = 0;
	return source;
}

GLuint loadShaders(ShaderInfo* shaders){
	if (shaders == NULL)
		return 0;
	GLuint program = glCreateProgram();

	ShaderInfo* entry = shaders;
	while (entry->type != GL_NONE){
		GLuint shader = glCreateShader(entry->type);
		entry->shader = shader;
		const GLchar *source = readShader(entry->filename);
		if (source == NULL){
			for (entry = shaders; entry->type != GL_NONE; ++entry) {
				glDeleteShader(entry->shader);
				entry->shader = 0;
			}
			return 0;
		}

		glShaderSource(shader, 1, &source, NULL);
		delete[] source;

		glCompileShader(shader);

		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			GLsizei len;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

			GLchar* log = new GLchar[len + 1];
			glGetShaderInfoLog(shader, len, &len, log);
			printf("compiled failed:\n\t%s\n", log);
			delete[] log;
			return 0;
		}
		glAttachShader(program, shader);
		++entry;
	}

	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked){
		GLsizei len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = new GLchar[len + 1];
		glGetProgramInfoLog(program, len, &len, log);
		printf("linked failed:\n\t%s\n", log);
		delete[] log;
		for (entry = shaders; entry->type != GL_NONE; ++entry) {
			glDeleteShader(entry->shader);
			entry->shader = 0;
		}
		return 0;
	}

	return program;
}

long getTime() {
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	return sys.wMilliseconds + sys.wSecond * 1000 + sys.wMinute * 60000 + sys.wHour * 3600000 + sys.wDay * 86400000;
}

float interValue(DWORD firstP, float first, DWORD secondP, float second,DWORD interP) {
	DWORD distance = secondP - firstP;
	float percent = (interP - firstP) / (float)distance;
	float result = percent * (second - first) + first;
	return result;
}
// 线性插值
void slerp(float result[4], float starting[4], float ending[4], float t) {

	float cosa = starting[0] * ending[0] + starting[1] * ending[1]
		+ starting[2] * ending[2] + starting[3] * ending[3];
	if (cosa < 0.0f) {
		ending[0] = -ending[0];
		ending[1] = -ending[1];
		ending[2] = -ending[2];
		ending[3] = -ending[3];
		cosa = -cosa;
	}
	float k0, k1;
	if (cosa > 0.9999f) {
		k0 = 1.0f - t;
		k1 = t;
	}
	else {
		float sina = sqrt(1.0f - cosa * cosa);
		float a = atan2(sina, cosa);
		float invSina = 1.0f / sina;
		k0 = sin((1.0f - t) * a) * invSina;
		k1 = sin(t * a) * invSina;
	}
	result[0] = starting[0] * k0 + ending[0] * k1;
	result[1] = starting[1] * k0 + ending[1] * k1;
	result[2] = starting[2] * k0 + ending[2] * k1;
	result[3] = starting[3] * k0 + ending[3] * k1;
}

// 内存逆置
DWORD reverseDWord(DWORD DWord) {
	DWORD NewDWord;
	CHAR Source[4];
	CHAR Target[4];

	memcpy(Source, &DWord, 4);

	Target[0] = Source[3];
	Target[1] = Source[2];
	Target[2] = Source[1];
	Target[3] = Source[0];

	memcpy(&NewDWord, Target, 4);
	return NewDWord;
}

DWORD readDWORD(FILE *handle){
	DWORD temp;
	fread(&temp, sizeof(DWORD), 1, handle);
	return temp;
}

GLuint readToBuffer(FILE *handle, GLuint size, GLenum type){
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(type, buffer);
	glBufferData(type, size, NULL, GL_STATIC_DRAW);
	void *data = glMapBuffer(type, GL_WRITE_ONLY);
	fread(data, size, 1, handle);
	glUnmapBuffer(type);
	glBindBuffer(type, 0);
	return buffer;
}

void bindToVAO(GLuint vao, GLuint index, void *data, GLuint size, GLenum type, GLuint typeSize){
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, typeSize, type, GL_FALSE, 0, 0);
}

GLuint writeToTBO(void *data, GLuint size, GLenum type, GLenum texIndex){
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_TEXTURE_BUFFER, buffer);
	glBufferData(GL_TEXTURE_BUFFER, size, data, GL_STATIC_DRAW);
	GLuint tbo;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tbo);
	glBindTexture(GL_TEXTURE_BUFFER, tbo);
	glTexBuffer(GL_TEXTURE_BUFFER, type, buffer);
	return tbo;
}