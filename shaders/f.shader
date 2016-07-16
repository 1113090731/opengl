#version 430 core

// ��ͼ������
uniform sampler2D tex;
// ������
uniform vec3 Ambient; 
// �����
uniform vec3 sunColor; // ��ɫ
uniform vec3 sunDirection; // ����
uniform float sunStrength; // ǿ��
uniform float sunShininess; // �����ָ��
uniform vec3 sunHalfVector; // ����ȱ��淽��

// ����
in vec3 normal;
in vec2 vs_tex_coord;
// ���
out vec4 color;

void main(){
	float diffuse = max(0.0,dot(normal,sunDirection));
	float specular = max(0.0,dot(normal,sunHalfVector));

	if(diffuse == 0.0)
		specular = 0.0;
	else
		specular = pow(specular,sunShininess);

	vec3 scatteredLight = Ambient + sunColor * diffuse;
	vec3 reflectedLight = sunColor * specular * sunStrength;

	vec4 texColor = texture(tex,vs_tex_coord);
	vec3 rgb = min(texColor.rgb * scatteredLight + reflectedLight,vec3(1.0));
	color = vec4(rgb,texColor.a);
}