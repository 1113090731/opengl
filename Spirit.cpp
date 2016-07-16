#include "Spirit.h"
#include "util.h"

Spirit::Spirit(ModelMDX *m, float x, float y , float s){
	model = m;
	matrixs = new vmath::mat4[m->numOfPivotPoint];
	this->x = x;
	this->y = y;
	this->z = 0;
	this->scale = s;
	this->angle = 0;
	state = 0;
}


Spirit::~Spirit()
{
}

void Spirit::playAnim(int n) {
	currentSequence = model->seqs[n];
	startTime = getTime();
	durationTime = startTime;
}

void Spirit::playAnim(const char *name) {
	currentSequence = model->seqs[0];
	for (int i = 0; i < model->numOfSeqs; i++) {
		const char *c = name;
		const char *temp = model->seqs[i].Name;
		bool isContain = true;
		while (*c != '\0'){
			if (*c != *temp){
				isContain = false;
				break;
			}
			c++;
			temp++;
		}
		if (isContain){
			currentSequence = model->seqs[i];
			break;
		}
	}
	startTime = getTime();
	durationTime = startTime;
}

extern GLuint program;

void Spirit::draw() {

	if (state == -1)
		return;

	//Anim();

	DWORD NumGeoset = model->geos.size();
	/*
	for (int i = 0; i < NumGeoset; i++) {
		for (DWORD j = 0; j < geosets[i]->NrOfVertexPositions; j++) {
			DWORD groupIndex = (geosets[i]->vertexGroup + j)->MatrixGroup;
			DWORD groupSize = (geosets[i]->matrixGroup + groupIndex)->MatrixGroupSize;
			//if (groupSize!=1)
			//	printf("groupSize %d\n", groupSize);
			int pos = 0;
			for (int l = 0; l < groupIndex; l++){
				pos += geosets[i]->matrixGroup[l].MatrixGroupSize;
			}

			VertexPosition temp0 = *(animVertex[i] + j);
			for (int k = 0; k < groupSize; k++){

				VertexPosition temp1 = geosets[i]->vertexPosition[j];
				DWORD matId = geosets[i]->matrixIndex[pos + k].matrixIndex;
				mulWithMat(&temp1, mats[matId]);
				if (k == 0){
					(animVertex[i] + j)->Position[0] = temp1.Position[0];
					(animVertex[i] + j)->Position[1] = temp1.Position[1];
					(animVertex[i] + j)->Position[2] = temp1.Position[2];
				}
				else{
					(animVertex[i] + j)->Position[0] += temp1.Position[0];
					(animVertex[i] + j)->Position[1] += temp1.Position[1];
					(animVertex[i] + j)->Position[2] += temp1.Position[2];
				}
			}
			(animVertex[i] + j)->Position[0] *= (1.0f / groupSize);
			(animVertex[i] + j)->Position[1] *= (1.0f / groupSize);
			(animVertex[i] + j)->Position[2] *= (1.0f / groupSize);
		}
	}*/


	vmath::mat4 translate = vmath::translate<float>(x, y, z);
	vmath::mat4 rotate = vmath::rotate<float>(angle, 0, 0, 1);
	vmath::mat4 scales = vmath::scale(scale, scale, scale);
	vmath::mat4 modelMat = translate * rotate * scales;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, modelMat);
	glUniformMatrix4fv(glGetUniformLocation(program, "normalMatrix"), 1, GL_FALSE, rotate);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_BUFFER, matrixsTBO);
	for (int i = 0; i < NumGeoset; i++) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, model->texId.at(model->matTexId.at(model->geos.at(i).MaterialId)));
		/*glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_BUFFER,model->geos.at(i).groupSize);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_BUFFER, model->geos.at(i).groupIndex);	*/
		//glActiveTexture(GL_TEXTURE3);			
		glBindVertexArray(model->geos.at(i).vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->geos.at(i).index);
		glDrawElements(GL_TRIANGLES, model->geos.at(i).TotalNrOfIndexes, GL_UNSIGNED_SHORT, 0);		
	}
	//glDeleteTextures(1, &matrixsTBO);
}


void Spirit::Anim() {

	if (state == -1)
		return;
	long currentTime = getTime();
	durationTime = currentTime - startTime;

	AnimEvent();

	// 遍历更新骨骼
	Node *temp = model->nodes->next;
	while (temp != NULL) {
		if (temp->ParentId == -1) {
			AnimNode(temp);
		}
		temp = temp->next;
	}
	for (int i = 0; i < model->numOfPivotPoint; i++){
		
		//for (int j = 0; j < 4;j++)
		//for (int k = 0; k < 4; k++){
		//	if (j==k)
		//		matrixs[i][j][k] = 1;
		//	else
		//		matrixs[i][j][k] = 0;
		//}
		matrixs[i].transpose();
	}

	matrixsTBO = writeToTBO(matrixs, 64 * model->numOfPivotPoint, GL_RGBA32F, GL_TEXTURE1);
}

// 根据当前状态设置动画。 目前仅需要动画结束和循环走动
void Spirit::AnimEvent() {

	long inter = currentSequence.IntervalEnd - currentSequence.IntervalStart;

	if (durationTime > inter) {
		if (currentSequence.Flags == 0) {

		}
		else {
			playAnim("Stand");
		}
	}
}

void Spirit::AnimNode(Node *temp) {

	DWORD start = currentSequence.IntervalStart;
	DWORD end = currentSequence.IntervalEnd;
	DWORD currentFrame = start + durationTime % (end - start);

	DWORD size;
	float r[4] = { 0, 0, 0, 1 };
	float t[3] = { 0, 0, 0 };
	float s[3] = { 1, 1, 1 };
	float p[3] = { 0, 0, 0 };
	if (temp->translation != NULL) {
		size = temp->translation->NrOfTracks;
		if (size == 1) {
			t[0] = temp->translation->translationTrack->Translation[0];
			t[1] = temp->translation->translationTrack->Translation[1];
			t[2] = temp->translation->translationTrack->Translation[2];
		}
		TranslationTrack *last = NULL, *first;
		for (int i = 0; i < size; i++) {
			if ((temp->translation->translationTrack + i)->Time
		> currentFrame) {
				last = temp->translation->translationTrack + i;
				if (i == 0)
					first = last;
				else
					first = last - 1;
				break;
			}
		}

		if (last != NULL) {
			if (last == first) {
				t[0] = last->Translation[0];
				t[1] = last->Translation[1];
				t[2] = last->Translation[2];
			}
			else {

				t[0] = interValue((last - 1)->Time, (last - 1)->Translation[0],
					last->Time, last->Translation[0], currentFrame);
				t[1] = interValue((last - 1)->Time, (last - 1)->Translation[1],
					last->Time, last->Translation[1], currentFrame);
				t[2] = interValue((last - 1)->Time, (last - 1)->Translation[2],
					last->Time, last->Translation[2], currentFrame);

			}
		}
	}

	if (temp->rotation != NULL) {
		size = temp->rotation->NrOfTracks;
		if (size == 1) {
			r[0] = temp->rotation->rotationTrack->Rotation[0];
			r[1] = temp->rotation->rotationTrack->Rotation[1];
			r[2] = temp->rotation->rotationTrack->Rotation[2];
			r[3] = temp->rotation->rotationTrack->Rotation[3];
		}
		RotationTrack *last = NULL, *first;
		for (int i = 0; i < size; i++) { // size = 2
			if ((temp->rotation->rotationTrack + i)->Time > currentFrame) {
				last = temp->rotation->rotationTrack + i;
				if (i == 0)
					first = last;
				else
					first = last - 1;
				break;
			}
		}


		if (last != NULL) {
			if (last == first) {
				r[0] = last->Rotation[0];
				r[1] = last->Rotation[1];
				r[2] = last->Rotation[2];
				r[3] = last->Rotation[3];
			}
			else {
				slerp(r, first->Rotation, last->Rotation,
					(currentFrame - first->Time)
					/ (float)(last->Time - first->Time));
			}
		}
	}

	if (temp->scaling != NULL) {
		size = temp->scaling->NrOfTracks;
		if (size == 1) {
			s[0] = temp->scaling->scalingTrack->Scaling[0];
			s[1] = temp->scaling->scalingTrack->Scaling[1];
			s[2] = temp->scaling->scalingTrack->Scaling[2];
		}
		ScalingTrack *last = NULL, *first;
		for (int i = 0; i < size; i++) {
			if ((temp->scaling->scalingTrack + i)->Time > currentFrame) {
				last = temp->scaling->scalingTrack + i;
				if (i == 0)
					first = last;
				else
					first = last - 1;
				break;
			}
		}

		if (last != NULL) {
			if (last == first) {
				s[0] = last->Scaling[0];
				s[1] = last->Scaling[1];
				s[2] = last->Scaling[2];
			}
			else {
				s[0] = interValue((last - 1)->Time, (last - 1)->Scaling[0],
					last->Time, last->Scaling[0], currentFrame);
				s[1] = interValue((last - 1)->Time, (last - 1)->Scaling[1],
					last->Time, last->Scaling[1], currentFrame);
				s[2] = interValue((last - 1)->Time, (last - 1)->Scaling[2],
					last->Time, last->Scaling[2], currentFrame);
			}
		}
	}

	p[0] = model->pivotPoints[temp->ObjectId][0];
	p[1] = model->pivotPoints[temp->ObjectId][1];
	p[2] = model->pivotPoints[temp->ObjectId][2];

	vmath::mat4 translate = vmath::translate<float>(t[0], t[1], t[2]);
	vmath::mat4 backPivot = vmath::translate<float>(p[0], p[1], p[2]);
	vmath::mat4 rotate = vmath::rotate<float>(2 * acos(r[3]) * 180 / 3.1415926, r[0], r[1], r[2]);
	vmath::mat4 scale = vmath::scale<float>(s[0], s[1], s[2]);
	vmath::mat4 toPivot = vmath::translate<float>(-p[0], -p[1], -p[2]);
	
	if (temp->ParentId == -1)
		matrixs[temp->ObjectId] = translate*backPivot*rotate*toPivot;
	else
		matrixs[temp->ObjectId] = matrixs[temp->parent->ObjectId] * translate*backPivot*rotate*toPivot;
		
	if (temp->child != NULL) {
		AnimNode(temp->child);
	}
	if (temp->brother != NULL) {
		AnimNode(temp->brother);
	}
}