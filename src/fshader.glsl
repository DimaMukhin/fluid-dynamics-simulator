#version 150

#define IX(i, j) ((i) + (N) * (j))

const int N = 128;
const int scale = 4;

uniform vec4 color;
out vec4 fcolor;

void main() 
{
	fcolor = color;
}
