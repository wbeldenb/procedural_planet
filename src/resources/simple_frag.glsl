#version 410 core 

//to send the color to a frame buffer
layout(location = 0) out vec4 color;

uniform vec3 camPos;

//in vec4 fragPos;
in vec3 position_FS_in;
in vec3 normal_FS_in;
in float fragColor;

void main()
{
//	//vec2 texcoords=frag_tex;
//	//float t=1./1000.;
//	//texcoords -= vec2(camoff.x,camoff.z)*t;
//
//	//vec3 heightcolor = texture(tex, texcoords).rgb;
//	//heightcolor.r = 0.1 + heightcolor.r*0.9;
//	//color.rgb = texture(tex2, texcoords*50).rgb * heightcolor.r;
//	//color.a=1;
//
	vec3 bgcolor = vec3(49. / 255., 88. / 255., 114. / 255.);
	vec3 fgcolor = vec3(254. / 255., 225. / 255., 168. / 255.);

	float len = length(position_FS_in.xyz+camPos.xyz);
	len-=1000;
	len/=300.;
	len=clamp(len,0,1);
	vec3 lp=vec3(20000,0,20000);
	vec3 ld = normalize(lp - position_FS_in);
	//ld = vec3(1,-1,1);
	float light = dot(normalize(normal_FS_in),ld);
	light=clamp(light,0,1);

	vec3 cd = normalize(camPos - position_FS_in);
	vec3 h = normalize(ld+cd);
	float spec = dot(normalize(normal_FS_in),h);
	spec=clamp(spec,0,1);

	float ambientlight = 0.05+light*0.95;
	color.rgb = ambientlight*vec3(0.718,0.7,0.97);

	color.a=1;//1-len;
	//if(renderstate==2)
		//color=vec4(0,0,0,1);	
	color.rgb = (color.rgb*0.8+ vec3(1,1,1)*spec*0.1)*fgcolor;//*(1-len) + bgcolor*len;
	color.a=1;

//vec3 normal = normalize(normal_FS_in);
//	if (position_FS_in.x >=0) {
//		color = vec4(0.0, 0.0, 0.0, 1.0);
//		color.r = 0.5;
//		color.b = 0.0;
//		color.g = 0.0;
//	}
//	else {
//		color = vec4(0.0, 0.0, 0.0, 1.0);
//		color.r = 0.0;
//		color.b = 0.5;
//		color.g = 0.0;
//	}
//	if (position_FS_in.y >= 0)
//		color.g = 0.0;
//
//	else
//		color.g = 0.5;
}

