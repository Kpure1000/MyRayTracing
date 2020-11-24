/*
* uniform test
*/
#version 330 core  
out vec4 FragColor; 

uniform vec4 verColor;

void main()  
{
    //use uniform to set color
    FragColor = verColor;
}