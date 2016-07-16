#version 430 core

// 贴图采样器
uniform sampler2D tex;
// 环境光
uniform vec3 Ambient; 
// 方向光
uniform vec3 sunColor; // 颜色
uniform vec3 sunDirection; // 方向
uniform float sunStrength; // 强度
uniform float sunShininess; // 光泽度指数
uniform vec3 sunHalfVector; // 光泽度表面方向

// 输入
in vec3 normal;
in vec2 vs_tex_coord;
// 输出
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