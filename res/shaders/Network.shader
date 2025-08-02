#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in float a_activation;
layout(location = 2) in vec2 a_offset;

uniform mat4 u_proj;
uniform vec2 u_translate;

out float v_activation;

void main()
{
   gl_Position = u_proj * vec4(u_translate + position + a_offset, 0.0, 1.0);

   v_activation = a_activation;
};

#shader fragment
#version 330 core

in float v_activation;

out vec4 FragColor;

void main()
{
   vec3 colorA = vec3(1.0, 1.0, 1.0);
   vec3 colorB = vec3(0.0, 1.0, 0.0);

   FragColor = vec4(mix(colorA, colorB, v_activation), 1.0);
};