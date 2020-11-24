/*
* uniform test
*/
#version 330 core  
out vec4 FragColor; 

in vec4 verPos;

void main()  
{
    //use uniform to set color
    float t = verPos.y * 0.5 + 0.5;
    FragColor = vec4((1.0 - t) * vec3( 1.0,1.0,1.0 ) 
    + t * vec3( 0.5,0.7,1.0 ), 1.0);
}