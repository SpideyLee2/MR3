#version 450 core

layout (location = 0) in vec3 aPos;

layout (std140, binding = 0) uniform Matrices {
	mat4 view;
	mat4 projection;
};

out VS_OUT {
	vec3 localPos;
} vs_out;

void main() {
	vs_out.localPos = aPos;

	// Removes translation from the view matrix
	mat4 rotView = mat4(mat3(view));
	vec4 clipPos = projection * rotView * vec4(aPos, 1.0);

	// xyww swizzeling here ensures the depth value of the rendered cube fragments are always 1.0
	gl_Position = clipPos.xyww;
}