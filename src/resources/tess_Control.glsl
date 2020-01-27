#version  410 core

//define the number of control points in the output patch
layout (vertices = 3) out;

uniform vec3 camPos;
uniform int meshMaxSize;
uniform int resolution;

//attributes of the input control points
in vec3 position_CS_in[];
in vec3 normal_CS_in[];

//attibutes of the output control points
out vec3 position_ES_in[];
out vec3 normal_ES_in[];

//calculate the tessellation level
float calculateTessFactLinear() {
	float df = meshMaxSize * resolution;
	float dist = df - length(camPos.xyz - position_CS_in[gl_InvocationID].xyz);
	//float dist = df - distance(camPos.xyz, position_CS_in[gl_InvocationID].xyz);
	dist /= df;
	dist = pow(dist, 5);

	//increase constant dist multiplied by for higher max tess levels
	float tessfact = dist * 25;
	tessfact = max(1, tessfact);

	return tessfact;
}

void main() {
	//set the control points for each output patch
	normal_ES_in[gl_InvocationID] = normal_CS_in[gl_InvocationID];
	position_ES_in[gl_InvocationID] = position_CS_in[gl_InvocationID];

	float tessfact = 1;//calculateTessFactLinear();

	gl_TessLevelInner[0] = tessfact;
	gl_TessLevelInner[1] = tessfact;
	gl_TessLevelOuter[0] = tessfact;
	gl_TessLevelOuter[1] = tessfact;
	gl_TessLevelOuter[2] = tessfact;
}
