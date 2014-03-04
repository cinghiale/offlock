#version 150
uniform vec4 globalColor;
smooth in vec4 theColor;
out vec4 outputColor;

void main()
{
    outputColor = theColor;
}
