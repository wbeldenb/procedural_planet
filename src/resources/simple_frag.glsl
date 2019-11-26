#version 410 core 
uniform float maxAltitude;
//to send the color to a frame buffer
layout(location = 0) out vec4 color;

in vec4 fragPos;
void main()
{
	if (fragPos.x >=0) {
		color = vec4(0.0, 0.0, 0.0, 1.0);
		color.r = 0.5;
		color.b = 0.0;
		color.g = 0.0;
	}
	else {
		color = vec4(0.0, 0.0, 0.0, 1.0);
		color.r = 0.0;
		color.b = 0.5;
		color.g = 0.0;
	}
	if (fragPos.y >= 0)
		color.g = 0.0;

	else
		color.g = 0.5;
}
