#version 150

in vec4 vPosition;
uniform mat4 ModelView, Projection;

void main()
{
    gl_Position = Projection * ModelView * vPosition;
}

