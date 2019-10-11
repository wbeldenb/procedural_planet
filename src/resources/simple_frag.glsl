#version 330 core 
uniform float maxAltitude;
//to send the color to a frame buffer
layout(location = 0) out vec4 color;

in vec4 fragPos;
void main()
{
	color = vec4(0.0, 0.0, 0.0, 1.0);
	color.r = 0.5;
	color.b = 0.0;
	color.g = 0.0;
}
