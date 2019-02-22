#version 150

in vec4 vPosition;
uniform mat4 model, view, projection;
uniform vec4 color;

void main()
{
    gl_Position = projection * view * model * vPosition;
}

