#version 450
#extension GL_ARB_separate_shader_objects : enable

/*---------------------------------------------------------
Uniforms - Material properties
---------------------------------------------------------*/
layout(set = 1, binding = 0) uniform MaterialUbo {
	vec4 baseColorFactor;
	vec3 emissiveFactor;
	float metallicFactor;
	float roughnessFactor;
} materialUbo;

layout(set = 1, binding = 1) uniform sampler2D baseColorTexture;
layout(set = 1, binding = 2) uniform sampler2D metallicRoughnessTexture;
layout(set = 1, binding = 3) uniform sampler2D normalTexture;
layout(set = 1, binding = 4) uniform sampler2D occlusionTexture;
layout(set = 1, binding = 5) uniform sampler2D emissiveTexture;

/*---------------------------------------------------------
Inputs
---------------------------------------------------------*/
layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 lightDirNorm;
layout(location = 3) in vec3 eyeDirNorm;

/*---------------------------------------------------------
Outputs
---------------------------------------------------------*/
layout(location = 0) out vec4 outColor;

/*---------------------------------------------------------
Functions
---------------------------------------------------------*/
vec4 blinnPhong();
vec4 lambertian();

void main() {
	//outColor = vec4(1.0, 1.0, 1.0, 1.0);
	//outColor = instUbo.baseColorFactor * texture(baseColorTexture, fragTexCoord);
	//outColor = lambertian();
	outColor = blinnPhong();
}

/**
Lambertian shading model
*/
vec4 lambertian()
{
	/*
	Formula:

	L = Kd * I * max(0, dot(n, l))
	*/

	/* Base diffuse color */
	vec4 Kd = instUbo.baseColorFactor * texture(baseColorTexture, fragTexCoord);

	/* Light intensity */
	vec4 I = vec4(1.0, 1.0, 1.0, 1.0);

	/* Normal - combination of surface normal and sampled normal texture */
	vec3 n = fragNormal * texture(normalTexture, fragTexCoord).xyz;
	n = normalize(n);

	/* Keep diffuse at 0 if light more than 90 degrees relative to surface */
	float maxnl = max(0, dot(n, lightDirNorm));
	
	return (Kd * I * maxnl);
}

/**
Blinn-phong shading model
*/
vec4 blinnPhong()
{
	/* 
	Formula:

	L = Kd * I * max(0, dot(n, l)) + Ks * I * max(0, dot(n, h))
	*/

	vec4 I = vec4(1.0, 1.0, 1.0, 1.0);		/* Light intensity */
	vec3 v = eyeDirNorm;					/* View/eye direction (normalized) */
	vec3 l = lightDirNorm;					/* Light direction (normalized) */
	vec3 h = normalize(v + l);				/* half vector (bisector) */
	
	/*
	Diffuse
	*/

	/* Base diffuse color */
	vec4 Kd = instUbo.baseColorFactor * texture(baseColorTexture, fragTexCoord);

	/* Normal - combination of surface normal and sampled normal texture */
	vec3 n = fragNormal * texture(normalTexture, fragTexCoord).xyz;
	n = normalize(n);

	/* Keep diffuse at 0 if light more than 90 degrees relative to surface */
	float maxnl = max(0, dot(n, l));
	
	/* Compute diffuse value */
	vec4 diffuse = Kd * I * maxnl;

	/*
	Specular
	*/

	/* Specular color */
	vec4 Ks = vec4(1.0, 1.0, 1.0, 1.0);

	/* Limit to 0 past 90 degrees. Also use "p" factor to control shininess */
	float p = 100;
	float maxnh = pow(max(0, dot(n, h)), p);

	/* Compute specular value */
	vec4 specular = Ks * I * maxnh;

	/*
	p value examples:

	10		- eggshell
	100		- really shiny
	1000	- really glossy
	10000	- nearly mirror like
	*/

	/*
	Diffuse + Specular
	*/
	return (diffuse + specular);
}