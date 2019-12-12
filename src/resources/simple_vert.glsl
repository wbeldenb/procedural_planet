#version  410 core
layout(location = 0) in vec4 position_VS_in;
layout(location = 1) in vec3 normal_VS_in;

uniform mat4 M;
uniform float theta;
uniform vec3 axis;
uniform vec3 planetCenter;

out vec3 position_CS_in;
out vec3 normal_CS_in;

void main()
{
//	gl_Position = P * V * M * vertPos;
	//vec4 temp = M * position_VS_in;
	//temp.xyz -= planetCenter.xyz;
	//temp = M * temp;
	//temp.xyz += planetCenter.xyz;
//	if (theta > 0.00001f) {
//		temp.xyz -= planetCenter;
//		temp = vec4(temp.xyz * cos(theta) + axis*dot(axis, temp.xyz)*(1 - cos(theta)) + cross(temp.xyz, axis)*sin(theta), 1.f);
//		temp.xyz += planetCenter;
//	}
//	fragPos = vertPos;

	position_CS_in = (M * position_VS_in).xyz;
	normal_CS_in = (M * vec4(normal_VS_in, 0.0)).xyz;
}
