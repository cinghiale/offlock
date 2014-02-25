#version 150
uniform mat4 modelViewProjectionMatrix;
in vec3 position;
in vec3 color;
in vec3 boid_position;

smooth out vec4 theColor;
void main()
{
    gl_Position = modelViewProjectionMatrix * vec4(position + boid_position, 1.0);
    theColor = vec4(color, 1.0);
}
