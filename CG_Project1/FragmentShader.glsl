#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D texture1;
uniform vec3 color;
uniform bool isCrosshair;

void main()
{
    if (isCrosshair) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);  // Always red for crosshair
    } else {
        FragColor = texture(texture1, TexCoord);  // Use texture for other objects
    }
}