#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

out vec4 fragPos;

void main()
{
	gl_Position = P * V * M * vertPos;
	fragPos = vertPos;
}
