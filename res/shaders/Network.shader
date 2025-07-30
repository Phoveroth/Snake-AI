#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in float a_activation;
layout(location = 2) in vec2 a_offset;

uniform mat4 u_proj;
uniform vec2 u_translate;

void main()
{
   gl_Position = u_proj * vec4(u_translate + position + a_offset, 0.0, 1.0);

};

#shader fragment
#version 330 core

out vec4 FragColor;

void main()
{
   FragColor = vec4(1.0, 1.0, 1.0, 1.0);
   
};