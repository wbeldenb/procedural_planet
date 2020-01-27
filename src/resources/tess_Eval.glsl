#version  410 core

layout(triangles, equal_spacing, ccw) in;
//layout(triangles, fractional_even_spacing, cw) in;

uniform mat4 V;
uniform mat4 P;
uniform vec3 planetCenter;
uniform float maxAltitude;
uniform int randSeed;

in vec3 position_ES_in[];
in vec3 normal_ES_in[];

out vec3 position_FS_in;
out vec3 normal_FS_in;

int octaves = 8;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2) {
	return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D (vec3 v0, vec3 v1, vec3 v2) {
	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

float hash(float n) {
    return fract(sin(n) * 753.5453123 * randSeed);
}

float snoise(vec3 x)
{
	vec3 p = floor(x);
	vec3 f = fract(x);
	f = f * f * (3.0f - (2.0f * f));

	float n = p.x + p.y * 157.0f + 113.0f * p.z;
	return mix(mix(mix(hash(n + 0.0f), hash(n + 1.0f), f.x),
		mix(hash(n + 157.0f), hash(n + 158.0f), f.x), f.y),
		mix(mix(hash(n + 113.0f), hash(n + 114.0f), f.x),
			mix(hash(n + 270.0f), hash(n + 271.0f), f.x), f.y), f.z);
}

float noise(vec3 position, int octaves, float frequency, float persistence) {
	float total = 0.0f;
	float maxAmplitude = 0.0f;
	float amplitude = 1.f;
	for (int i = 0; i < octaves; i++) {
		total += snoise(position * frequency) * amplitude;
		frequency *= 2.0f;
		maxAmplitude += amplitude;
		amplitude *= persistence;
	}
	return total / maxAmplitude;
}

float getHeight(vec3 pos) {

	float dist = length(pos);

	int biomeOctaves = 3;
	float biomeFreq = 0.01f;
	float biomePers = 0.3f;
	float biomePreIntensity = 2;
	float biomePower = 2;
	float biomePostIntensity = 3;
	float biomeTranslate = 0.0f;

	float biomeHeight = pow(noise(pos.xyz + vec3(100.0), 2, 0.01f, 0.1f), 2) * 1;
	biomeHeight = clamp(biomeHeight, 0, 1);

	float biomeTemperature = noise(pos.xyz + vec3(100.0), 2, 0.002f, 0.1f) * 1;
	float biomeHumidity = noise(pos.xyz - vec3(100.0), 2, 0.002f, 0.1f) * 1;
	biomeTemperature = pow(biomeTemperature, 3) * 3;
	biomeHumidity = pow(biomeHumidity, 3) * 3;

	int baseOctaves = 11;
	float baseFreq = 0.05;
	float basePers = mix(0.2, 0.2f, biomeHeight);
	float basePreIntensity = mix(0.7, 0.7, biomeHeight);
	float power = 2;
	float basePostIntensity = mix(3, 40, biomeHeight);
	//float translate = mix(0, 50, biomeHeight);
	//float translate = mix(-maxAltitude, maxAltitude, biomeHeight);
	float translate = mix(-50, 50, biomeHeight);

	int heightOctaves = 11;
	float heightFreq = 0.1;
	float heightPers = mix(0.3, 0.4, biomeHeight);
	float heightPreIntensity = mix(0.8, 1.5, biomeHeight);


	float baseheight = noise(pos.xzy, baseOctaves, baseFreq, basePers) * basePreIntensity;
	float height = noise(pos.xzy, heightOctaves, heightFreq, heightPers) * heightPreIntensity;
	baseheight = pow(baseheight, power) * basePostIntensity;
	height = baseheight * height + translate;


	return height;
}



void main() {
	//interpolate the attributes of the output vertex using the barycentric coords
	normal_FS_in = interpolate3D(normal_ES_in[0], normal_ES_in[1], normal_ES_in[2]);
	position_FS_in = interpolate3D(position_ES_in[0], position_ES_in[1], position_ES_in[2]);

	vec3 normal = normalize(position_FS_in - planetCenter);
	normal_FS_in = normalize(normal);

	//float height = noise(position_FS_in.xyz, 2, 0.01f, 0.1f) * maxAltitude;
	float height = getHeight(position_FS_in.xyz);
	
	//position_FS_in += generateHeight(position_FS_in.x, position_FS_in.y) * normal;
	position_FS_in += height * normal;

	gl_Position = P * V * vec4(position_FS_in, 1.0);
}
