#version 150
uniform mat4 modelViewProjectionMatrix;
in vec3 position;
in vec3 color;
in vec3 boid_position;
uniform int cube_size;

smooth out vec4 theColor;
void main()
{
    vec4 p = vec4(position + boid_position, 1.0);
    gl_Position = modelViewProjectionMatrix * p;
    theColor = vec4(
        mix(0.0, 1.0, (p.x+cube_size)/(2*cube_size)),
        mix(0.0, 1.0, (p.y+cube_size)/(2*cube_size)),
        mix(0.0, 1.0, (p.z+cube_size)/(2*cube_size)), 1.0);
}
