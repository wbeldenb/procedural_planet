#version 330 core 
uniform float maxAltitude;
//to send the color to a frame buffer
layout(location = 0) out vec4 color;

in vec4 fragPos;
void main()
{
	color = vec4(0.0, 0.0, 0.0, 1.0);
	color.r = abs((fragPos.y + maxAltitude) / (maxAltitude));
	color.b = abs((fragPos.y + maxAltitude) / (maxAltitude));

	color.g = abs(0.05 / ((fragPos.y + maxAltitude) / (maxAltitude) + 0.01));
}
