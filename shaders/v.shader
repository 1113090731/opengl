#version 430 core

// input 
in vec4 position;
in vec3 v_normal;
in vec2 in_tex_coord;
in uint groupOffset;
in float groupId;

// out
out vec2 vs_tex_coord;
out vec3 normal;

// global
uniform mat4x4 modelMatrix;
uniform mat4x4 projectionViewModelMatrix;
uniform mat4x4 normalMatrix;

uniform usamplerBuffer groupMatrix;

void main(){
	
	mat4 boneMatrix = mat4(1.0);
	/*
	vec4 temp = position;
	uint size = texelFetch(groupSize,int(groupId)).r;
	
	for(int i=0;i<1;i++){
		uint matrixId = texelFetch(groupIndex,int(groupId+i)).r;
		vec4 c0 = texelFetch(groupMatrix,0);
		//vec4 c1 = texelFetch(groupMatrix,int(matrixId+1));
		//vec4 c2 = texelFetch(groupMatrix,int(matrixId+2));
		//vec4 c3 = texelFetch(groupMatrix,int(matrixId+3));
		//boneMatrix = mat4(c0,c1,c2,c3);
		//boneMatrix = mat4(1.0);
	}
	
	vec4 c0 = texelFetch(groupMatrix,int(groupId*4));
	vec4 c1 = texelFetch(groupMatrix,int(groupId*4+1));
	vec4 c2 = texelFetch(groupMatrix,int(groupId*4+2));
	vec4 c3 = texelFetch(groupMatrix,int(groupId*4+3));
	
	boneMatrix = mat4(c0,c1,c2,c3);
	*/

	gl_Position = projectionViewModelMatrix * modelMatrix * boneMatrix * position;

	vs_tex_coord = in_tex_coord;
	mat3x3 normal_matrix = mat3x3(normalMatrix[0].xyz,normalMatrix[1].xyz,normalMatrix[2].xyz);
	normal = normalize(normal_matrix * v_normal);
}