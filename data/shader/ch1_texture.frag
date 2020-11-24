#version 330 core  

/*in*/
in vec3 ourColor;
in vec2 texCoord;

/*out*/
out vec4 FragColor; 

/*uniform*/
uniform sampler2D ourTexture;

/*main*/
void main()  
{
    FragColor = texture(ourTexture,texCoord);
}