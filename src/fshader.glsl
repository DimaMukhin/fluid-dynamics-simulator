#version 150

uniform vec4 color;
in vec4 fcolor;
out vec4 thecolor;

void main() 
{
	thecolor = fcolor;
}
