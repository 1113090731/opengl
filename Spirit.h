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

	float x, y, z, scale, angle; // 位置信息
private:
	ModelMDX *model; // 模型信息
	Sequence currentSequence; // 动画控制
	long startTime, durationTime;
	vmath::mat4 *matrixs; // 骨骼矩阵
	GLuint matrixsTBO; // 缓存贴图对象

	
	int state;


	void Anim();
	void AnimEvent();
	void Spirit::AnimNode(Node *temp);
};

