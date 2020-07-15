#version 450
#extension GL_ARB_separate_shader_objects : enable

/*---------------------------------------------------------
Uniforms - per view
---------------------------------------------------------*/
layout(set = 0, binding = 0) uniform PerViewUBO {
    mat4 view;
    mat4 proj;
	vec3 cameraPos;
} viewUbo;

/*---------------------------------------------------------
Uniforms - per instance
---------------------------------------------------------*/
layout(set = 1, binding = 0) uniform PerMeshInstanceUBO {
	mat4 model;

	vec4 BaseColorFactor;
	vec3 EmissiveFactor;
	float MetallicFactor;
	float RoughnessFactor;
} meshUbo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 lightDirNorm;
layout(location = 3) out vec3 eyeDirNorm;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
	vec4 worldPos = meshUbo.model * vec4(inPosition, 1.0);

	vec4 lightPosWorld = vec4(-10.0, 0.0, 0.0, 1.0);
	vec4 lightDir =  lightPosWorld - worldPos;
	lightDirNorm = normalize(lightDir).xyz;

	vec4 eyeDir = vec4(viewUbo.cameraPos, 1.0) - worldPos;
	eyeDirNorm = normalize(eyeDir).xyz;

    gl_Position = viewUbo.proj * viewUbo.view * worldPos;
    fragNormal = inNormal;
	fragTexCoord = inTexCoord;
}