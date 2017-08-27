#version 450
#extension GL_ARB_viewport_array : enable

layout (triangles, invocations = 2) in;
layout (triangle_strip, max_vertices = 3) out;


layout( binding = 0) uniform QeDataMVP {
	mat4 model;
    mat4 view;
    mat4 proj;
	mat4 normal;
} mvp;

layout( binding = 1) uniform QeDataLight {
	vec4 pos;
    vec4 dir;
	vec4 color;
	int type;
	float intensity;
	float coneAngle;
} light;

layout(location = 0) in vec3 inColor[];
layout(location = 1) in vec2 inTexCoord[];
layout(location = 2) in vec3 inNormal[];

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec3 outLighttoVertex;
layout(location = 4) out vec3 outEye;


void main(void)
{	
	for(int i = 0; i < gl_in.length(); i++)
	{
		outColor = inColor[i];
		outTexCoord = inTexCoord[i];
		
		//outNormal = normalize( vec3(mvp.view * mvp.model * vec4(inNormal[i],0)));
		outNormal = normalize( vec3(mvp.normal * vec4(inNormal[i],0)));
		//mat4 normalMatrix = transpose(inverse(mvp.view * mvp.model));
		//outNormal = normalize( vec3(normalMatrix * vec4(inNormal[i],0)));
		
		vec4 posC = mvp.view * mvp.model * gl_in[i].gl_Position;
		outEye = vec3(-posC);
		outLighttoVertex = vec3( mvp.view * light.pos - posC);
		
		gl_Position = mvp.proj * posC;
		//gl_Position = mvp.proj *mvp.view * mvp.model * gl_in[i].gl_Position;
	
		// Set the viewport index that the vertex will be emitted to
		gl_ViewportIndex = gl_InvocationID;
		gl_PrimitiveID = gl_PrimitiveIDIn;
		EmitVertex();
	}
	EndPrimitive();
}