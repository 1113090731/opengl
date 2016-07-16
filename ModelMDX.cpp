#include "ModelMDX.h"
#include "texture.h"
#include "util.h"

ModelMDX::ModelMDX()
{
	nodes = new Node();
}

ModelMDX::~ModelMDX()
{
}

void ModelMDX::draw(){
	for (int i = 0; i < geos.size(); i++){
		glBindTexture(GL_TEXTURE_2D, texId.at(matTexId.at(geos.at(i).MaterialId)));
		glBindVertexArray(geos.at(i).vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geos.at(i).index);
		glDrawElements(GL_TRIANGLES, geos.at(i).TotalNrOfIndexes, GL_UNSIGNED_SHORT, 0);
	}
}

void ModelMDX::loadModel(const char *file){

	FILE *handle = fopen(file, "rb");

	if (handle == NULL){
		printf("找不到文件 %s!\n", file);
		exit(1);
	}

	if (reverseDWord(readDWORD(handle)) != 'MDLX'){
		printf("%s 不是mdx文件!\n", file);
		exit(1);
	}

	while (!feof(handle)){
		DWORD tag = reverseDWord(readDWORD(handle));

		if (tag == 'SEQS'){
			loadSEQS(handle);
		}
		else if (tag == 'GEOS'){
			loadGeosets(handle);
		}
		else if (tag == 'BONE'){
			loadBONE(handle);
		}
		else if (tag == 'ATCH') {
			DWORD total = readDWORD(handle);
			fseek(handle, total, SEEK_CUR);
			continue;
			while (total > 0) {
				DWORD inSize = readDWORD(handle);
				total -= inSize;
				readNode(handle);
				CHAR path[260];
				fread(path, 260, 1, handle);
				readDWORD(handle);
			}
		}
		else if (tag == 'HELP') {
			DWORD total = readDWORD(handle);
			while (total > 0) {
				Node *node = readNode(handle);
				total -= node->InclusiveSize;
			}
		}
		else if (tag == 'PIVT') {
			DWORD chunkSize = readDWORD(handle);
			numOfPivotPoint = chunkSize / 12;
			pivotPoints = new FLOAT3[numOfPivotPoint];
			fread(pivotPoints, chunkSize, 1, handle);
		}
		else if (tag == 'TEXS'){
			loadTex(handle);
		}
		else if (tag == 'MTLS'){
			loadMTLS(handle);
		}
	}
	buildBone();
}

void ModelMDX::loadSEQS(FILE *handle){
	long chunkSize = readDWORD(handle);
	numOfSeqs = chunkSize / 132;
	seqs = (Sequence *)malloc(chunkSize);
	fread(seqs, chunkSize, 1, handle);
}

extern GLuint program;

void ModelMDX::loadGeosets(FILE *handle){

	int totalSize = readDWORD(handle);
	while (totalSize > 0){
		int currentSize = readDWORD(handle);
		totalSize -= currentSize;
		Geoset geo;
		glGenVertexArrays(1, &geo.vao);
		glBindVertexArray(geo.vao);
		if (reverseDWord(readDWORD(handle)) == 'VRTX'){
			int numOfVertex = readDWORD(handle);
			geo.NrOfVertexPositions = numOfVertex;
			GLuint buffer = readToBuffer(handle, numOfVertex*sizeof(FLOAT3),GL_ARRAY_BUFFER);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			int pos = glGetAttribLocation(program, "position");
			glEnableVertexAttribArray(pos);
			glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		if (reverseDWord(readDWORD(handle)) == 'NRMS'){
			int numOfVertex = readDWORD(handle);
			//fseek(handle, numOfVertex*sizeof(FLOAT3), SEEK_CUR);
			GLuint buffer = readToBuffer(handle, numOfVertex*sizeof(FLOAT3), GL_ARRAY_BUFFER);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			int pos = glGetAttribLocation(program, "v_normal");
			glEnableVertexAttribArray(pos);
			glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		if (reverseDWord(readDWORD(handle)) == 'PTYP'){
			int NrOfFaceTypeGroups = readDWORD(handle);
			fseek(handle, NrOfFaceTypeGroups*sizeof(DWORD), SEEK_CUR);
		}
		if (reverseDWord(readDWORD(handle)) == 'PCNT'){
			int NrOfFaceGroups = readDWORD(handle);
			fseek(handle, NrOfFaceGroups*sizeof(DWORD), SEEK_CUR);
		}
		if (reverseDWord(readDWORD(handle)) == 'PVTX'){
			int TotalNrOfIndexes = readDWORD(handle);
			geo.TotalNrOfIndexes = TotalNrOfIndexes;
			GLuint buffer = readToBuffer(handle, TotalNrOfIndexes*sizeof(WORD),GL_ELEMENT_ARRAY_BUFFER);
			geo.index = buffer;
		}
//		if (reverseDWord(readDWORD(handle)) == 'GNDX'){
			readDWORD(handle); // 'GNDX'
			int NrOfVertexGroups = readDWORD(handle);
		//	BYTE *groupOffsetTmp = new BYTE[NrOfVertexGroups];
		//	fread(groupOffsetTmp, NrOfVertexGroups, 1, handle);
		//	bindToVAO(geo.vao, 3, groupOffsetTmp, NrOfVertexGroups, GL_BYTE, 1);
			GLuint buffer = readToBuffer(handle, NrOfVertexGroups*sizeof(BYTE), GL_ARRAY_BUFFER);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			int groupId = glGetAttribLocation(program, "groupId");
			glEnableVertexAttribArray(groupId);
			glVertexAttribPointer(groupId, 1, GL_BYTE, GL_FALSE, 0, 0);
		
//		}
//		if (reverseDWord(readDWORD(handle)) == 'MTGC'){
			readDWORD(handle); // 'MTGC'
			int NrOfMatrixGroups = readDWORD(handle);
			DWORD *groupSizeTmp = new DWORD[NrOfMatrixGroups];
			fread(groupSizeTmp, NrOfMatrixGroups*sizeof(DWORD), 1, handle);
			
			//for (int i = 1; i < NrOfMatrixGroups; ){
			//	BYTE offset = 0;				
			//	for (int j = 0; j < i; j++){
			//		
			//		offset += groupSizeTmp[j];
			//	}
			//	i += groupSizeTmp[i];
			//	printf("%d\n", groupSizeTmp[i]);
			//	groupOffsetTmp[i] = offset;
			//}
//			for (int i = 0; i < NrOfVertexGroups;i)
//			bindToVAO(geo.vao, 2, groupOffsetTmp, NrOfVertexGroups, GL_BYTE, 1);
			geo.groupSize = writeToTBO(groupSizeTmp, NrOfMatrixGroups*sizeof(DWORD), GL_R32UI,GL_TEXTURE1);
//			free(groupOffsetTmp);
			free(groupSizeTmp);
//		}
		if (reverseDWord(readDWORD(handle)) == 'MATS'){
			int NrOfMatrixIndexes = readDWORD(handle);
			GLuint buffer = readToBuffer(handle, NrOfMatrixIndexes*sizeof(DWORD), GL_TEXTURE_BUFFER);
			GLuint tbo;
			glGenTextures(1, &tbo);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_BUFFER, tbo);
			glBindBuffer(GL_TEXTURE_BUFFER, buffer);
			glTexBuffer(GL_TEXTURE_BUFFER, GL_R32UI, tbo);
			geo.groupIndex = tbo;
		}
		geo.MaterialId = readDWORD(handle);
		fseek(handle,2*sizeof(DWORD)+sizeof(FLOAT)+sizeof(FLOAT3)*2 , SEEK_CUR);

		int NrOfExtents = readDWORD(handle);
		fseek(handle, NrOfExtents*(sizeof(FLOAT)+2 * sizeof(FLOAT3)), SEEK_CUR);
		if (reverseDWord(readDWORD(handle)) == 'UVAS'){
			int NrOfTextureVertexGroups = readDWORD(handle);
		}
		if (reverseDWord(readDWORD(handle)) == 'UVBS'){
			int NrOfVertexTexturePositions = readDWORD(handle);
			GLuint buffer = readToBuffer(handle, NrOfVertexTexturePositions*sizeof(FLOAT2), GL_ARRAY_BUFFER);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			int uv = glGetAttribLocation(program, "in_tex_coord");
			glEnableVertexAttribArray(uv);
			glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, 0, 0);
		}
		geos.push_back(geo);
	}
}

void ModelMDX::loadBONE(FILE *handle){
	int total = readDWORD(handle);
	while (total > 0) {
		Node *node = readNode(handle);
		readDWORD(handle); // GeosetId
		readDWORD(handle); // GeosetAnimationId
		total -= node->InclusiveSize;
		total -= 8;
	}
}

void ModelMDX::loadTex(FILE *handle){
	DWORD chunkSize = readDWORD(handle);
	short numTex = chunkSize / 268; // 贴图个数
	CHAR *texFile = new CHAR[260 * numTex]; // 贴图路径
	for (int i = 0; i < numTex; ++i){
		DWORD replaceableId = readDWORD(handle); // replaceableId

		fread(&texFile[i * 260], 260, 1, handle); // FileName
		readDWORD(handle); // Flags

		int length = strlen(&texFile[i * 260]);
		char *file = new char[length];
		strcpy(file, &texFile[i * 260]);
		file[length - 3] = 'p';
		file[length - 2] = 'n';
		file[length - 1] = 'g';
		if (replaceableId != 0)
			texId.push_back(0); // replaceableId 未处理
		else
			texId.push_back(loadTexture(file)); // png
		//texId[i] = loadBlp(&texFile[i*260]); //blp
	}
}

void ModelMDX::loadMTLS(FILE *handle){
	DWORD chunkSize = readDWORD(handle);
		while (chunkSize){
			DWORD inclusiveSize = readDWORD(handle);
			chunkSize -= inclusiveSize;
			readDWORD(handle); // PriorityPlane
			readDWORD(handle); // Flags

			readDWORD(handle); // 'LAYS'
			DWORD nrOfLayers = readDWORD(handle);
			for (int i = 0; i < nrOfLayers; ++i){			
				DWORD inSize = readDWORD(handle);  // inclusiveSize
				readDWORD(handle); // FilterMode
				readDWORD(handle); // ShadingFlags
				DWORD textureId = readDWORD(handle); // TextureId;
				matTexId.push_back(textureId);
				readDWORD(handle); // TextureAnimationId
				readDWORD(handle); // CoordId;
				readDWORD(handle); // Alpha; 32bit FLOAT
				fseek(handle, inSize - 28, SEEK_CUR);
			}
		}
}


Node* ModelMDX::readNode(FILE *Handle) {

	Node *node = new Node();
	fread(&node->InclusiveSize, 4, 1, Handle);
	fread(&node->Name, 80, 1, Handle);
	fread(&node->ObjectId, 4, 1, Handle);
	fread(&node->ParentId, 4, 1, Handle);
	fread(&node->Flags, 4, 1, Handle);
	if (reverseDWord(readDWORD(Handle)) == 'KGTR')
	{
		node->translation = new GeosetTranslation();
		fread(&node->translation->NrOfTracks, 4, 1, Handle);
		fread(&node->translation->InterpolationType, 4, 1, Handle);
		fread(&node->translation->GlobalSequenceId, 4, 1, Handle);
		node->translation->translationTrack =
			new TranslationTrack[node->translation->NrOfTracks];
		if (node->translation->InterpolationType > 1) {
			fread(node->translation->translationTrack,
				node->translation->NrOfTracks * sizeof(TranslationTrack), 1,
				Handle);
		}
		else {
			for (int i = 0; i < node->translation->NrOfTracks; i++) {
				fread(&node->translation->translationTrack[i].Time, 4, 1,
					Handle);
				fread(&node->translation->translationTrack[i].Translation,
					sizeof(FLOAT3), 1, Handle);
			}
		}
	}
	else {
		fseek(Handle, -4, 1);
	}

	if (reverseDWord(readDWORD(Handle)) == 'KGRT')
	{
		node->rotation = new GeosetRotation();
		fread(&node->rotation->NrOfTracks, 4, 1, Handle);
		fread(&node->rotation->InterpolationType, 4, 1, Handle);
		fread(&node->rotation->GlobalSequenceId, 4, 1, Handle);
		node->rotation->rotationTrack =
			new RotationTrack[node->rotation->NrOfTracks];
		if (node->rotation->InterpolationType > 1) {
			fread(node->rotation->rotationTrack,
				node->rotation->NrOfTracks * sizeof(RotationTrack), 1,
				Handle);
		}
		else {
			for (int i = 0; i < node->rotation->NrOfTracks; i++) {
				fread(&node->rotation->rotationTrack[i].Time, 4, 1, Handle);
				fread(&node->rotation->rotationTrack[i].Rotation,
					sizeof(FLOAT4), 1, Handle);
			}
		}
	}
	else {
		fseek(Handle, -4, 1);
	}

	if (reverseDWord(readDWORD(Handle)) == 'KGSC')
	{
		node->scaling = new GeosetScaling();
		fread(&node->scaling->NrOfTracks, 4, 1, Handle);
		fread(&node->scaling->InterpolationType, 4, 1, Handle);
		fread(&node->scaling->GlobalSequenceId, 4, 1, Handle);
		node->scaling->scalingTrack =
			new ScalingTrack[node->scaling->NrOfTracks];
		if (node->scaling->InterpolationType > 1) {
			fread(node->scaling->scalingTrack,
				node->scaling->NrOfTracks * sizeof(ScalingTrack), 1,
				Handle);
		}
		else {
			for (int i = 0; i < node->scaling->NrOfTracks; i++) {
				fread(&node->scaling->scalingTrack[i].Time, 4, 1, Handle);
				fread(&node->scaling->scalingTrack[i].Scaling, sizeof(FLOAT3),
					1, Handle);
			}
		}
	}
	else {
		fseek(Handle, -4, 1);
	}
	addNode(node);
	return node;
}

void ModelMDX::addNode(Node *n) {

	Node *temp = nodes;
	while (temp->next != NULL) {
		temp = temp->next;
	}
	temp->next = n;
}

void ModelMDX::buildNode(Node *n) {
	if (n->ParentId != -1) {
		Node *target = nodes;
		for (int i = 0; i <= n->ParentId; i++) {
			target = target->next;
		}
		n->parent = target;
		if (target->child == NULL) {
			target->child = n;
		}
		else {
			target = target->child;
			while (target->brother != NULL) {
				target = target->brother;
			}
			target->brother = n;
		}
	}
}

void ModelMDX::buildBone() {
	Node *temp = nodes->next;
	while (temp != NULL) {
		buildNode(temp);
		temp = temp->next;
	}
}