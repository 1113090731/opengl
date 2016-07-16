

#include "vgl.h"
#include "util.h"
#include "ModelMDX.h"
#include "spirit.h"
using namespace vmath;

enum LOCATION_IDs{ pos, modelMatrix, viewMatrix, projectionViewModelMatrix, NumLocation };
enum VAO_IDs{ Triangles, NumVAOs };
enum Buffer_IDs{ ArrayBuffer, NumBuffers };

GLuint location[NumLocation];
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

vmath::mat4 projection, camera;

GLfloat ro = 0;
ModelMDX mdx;
Spirit *hero;

GLuint program;

void init(){

	glEnable(GL_DEPTH_TEST);

	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "shaders/v.shader" },
		{ GL_FRAGMENT_SHADER, "shaders/f.shader" },
		{ GL_NONE, NULL }
	}; 
	program = loadShaders(shaders);
	glUseProgram(program);
	// 光照设置
	// 环境光
	glUniform4fv(glGetUniformLocation(program, "Ambient"), 1, vec4(1,1,1,1));
	// 方向光
	GLuint sunColor = glGetUniformLocation(program, "sunColor");
	GLuint sunDirection = glGetUniformLocation(program, "sunDirection");
	GLuint sunStrength = glGetUniformLocation(program, "sunStrength");
	GLuint sunShininess = glGetUniformLocation(program, "sunShininess");
	GLuint sunHalfVector = glGetUniformLocation(program, "sunHalfVector");
	glUniform3fv(sunColor, 1, vec3(1, 1, 1));
	glUniform3fv(sunDirection, 1, vec3(1, -1, 1));
	glUniform1f(sunStrength, 1);
	glUniform1f(sunShininess, 1);
	glUniform3fv(sunHalfVector, 1, vec3(1, 1, 1));

	glUniform1i(glGetUniformLocation(program, "groupMatrix"), 1);
	location[projectionViewModelMatrix] = glGetUniformLocation(program, "projectionViewModelMatrix");

	projection = perspective(45, 1, 1, 2000);
	camera = lookat(vec3(0, 150, 100), vec3(0, 0, 0), vec3(0, 1, 0));

	mdx.loadModel("hero.mdx");
	hero = new Spirit(&mdx);
}

void onDraw(){
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	
	mat4 rotatZ = rotate<float>(0, 0, 1,0);
	mat4 rotatX = rotate<float>(-90, 1, 0, 0);
	mat4 scale = vmath::scale<float>(0.7, 0.7, 0.7);
	mat4 world = projection * camera * rotatX * rotatZ * scale;
	glUniformMatrix4fv(location[projectionViewModelMatrix], 1, GL_FALSE, world);

	hero->angle += 1;
	hero->playAnim(0);
	hero->draw();

	glutSwapBuffers();
}

void onLogic(){
	glutPostRedisplay();
}

void onKeyDown(unsigned char key, int x, int y){

}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("test");
	glewExperimental = GL_TRUE; // 扩展
	glewInit();
	init();
	glutDisplayFunc(onDraw);
	glutIdleFunc(onLogic);
	glutKeyboardFunc(onKeyDown);
	glutMainLoop();
	return 0;
}