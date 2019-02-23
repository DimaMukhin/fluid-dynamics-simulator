#version 150

const int N = 128;
const int scale = 4;
uniform float dens[N * N];

uniform vec4 color;
out vec4 fcolor;

void main() 
{
	vec4 fragCoord = gl_FragCoord;
	fcolor = fragCoord / 512;
}
