#version 410 core 

//to send the color to a frame buffer
layout(location = 0) out vec4 color;

//in vec4 fragPos;
in vec3 position_FS_in;
in vec3 normal_FS_in;

void main()
{
	vec3 normal = normalize(normal_FS_in);
	if (position_FS_in.x >=0) {
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
	if (position_FS_in.y >= 0)
		color.g = 0.0;

	else
		color.g = 0.5;
}
