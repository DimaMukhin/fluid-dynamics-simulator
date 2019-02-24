#version 150

in vec4 vPosition;
in vec4 vColor;

uniform mat4 model, view, projection;

out vec4 fcolor;

void main()
{
    gl_Position = projection * view * model * vPosition;
	fcolor = vColor;
}

