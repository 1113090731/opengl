#version 430 core

// input 
in vec4 position;
in vec2 in_tex_coord;
in float groupOffset;
in float groupId;

// out
out vec2 vs_tex_coord;

// global
uniform mat4x4 modelMatrix;
uniform mat4x4 viewMatrix;
uniform mat4x4 projectionViewModelMatrix;

uniform usamplerBuffer groupSize;
uniform usamplerBuffer groupIndex;
uniform samplerBuffer groupMatrix;

void main(){
	
	mat4 boneMatrix;
	vec4 temp = position;
	uint size = texelFetch(groupSize,int(groupId)).r;
	
	for(int i=0;i<1;i++){
		uint matrixId = texelFetch(groupIndex,int(groupOffset+i)).r;
		vec4 c0 = texelFetch(groupMatrix,0);
		vec4 c1 = texelFetch(groupMatrix,0+1);
		vec4 c2 = texelFetch(groupMatrix,0+2);
		vec4 c3 = texelFetch(groupMatrix,0+3);
		boneMatrix = mat4(c0,c1,c2,c3);
	}
	
	gl_Position = projectionViewModelMatrix * modelMatrix * position;
	vs_tex_coord = in_tex_coord;
}