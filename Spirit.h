#pragma once

#include "ModelMDX.h"

class Spirit
{
public:
	Spirit(ModelMDX *m, float x = 0, float y = 0, float s = 1);
	~Spirit();

	void draw();
	void playAnim(int n);
	void Spirit::playAnim(const char *name);

	float x, y, z, scale, angle; // λ����Ϣ
private:
	ModelMDX *model; // ģ����Ϣ
	Sequence currentSequence; // ��������
	long startTime, durationTime;
	vmath::mat4 *matrixs; // ��������
	GLuint matrixsTBO; // ������ͼ����

	
	int state;


	void Anim();
	void AnimEvent();
	void Spirit::AnimNode(Node *temp);
};

