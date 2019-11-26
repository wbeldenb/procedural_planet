#version  410 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform float theta;
uniform vec3 axis;
uniform vec4 planetCenter;

out vec4 fragPos;

void main()
{
	gl_Position = P * V * M * vertPos;
	
	if (theta > 0) {
		vec4 temp = P * V * M * vertPos - planetCenter;
		gl_Position = vec4(temp.xyz * cos(theta) + axis*dot(axis, temp.xyz)*(1 - cos(theta)) + cross(temp.xyz, axis)*sin(theta), 1.f);
		gl_Position += planetCenter;
	}
	fragPos = vertPos;
}
