#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 a_texcoords;
layout(location = 2) in vec2 a_offset;
layout(location = 3) in float a_texture;
layout(location = 4) in float a_texture_rotate;

out vec2 v_texcoords;

uniform mat4 u_proj;
uniform vec2 u_translate;

void main()
{
   gl_Position = u_proj * vec4(u_translate + position + a_offset, 0.0, 1.0);

   switch (int(a_texture_rotate)) {
      case 0: // No Rotation
         v_texcoords = a_texcoords;
         break;
      case 1: // 90
         v_texcoords = vec2(a_texcoords.y, 0.25 - a_texcoords.x);
         break;
      case 2: // 180
         v_texcoords = vec2(0.25 - a_texcoords.x, 0.25 - a_texcoords.y);
         break;
      case 3: // 270
         v_texcoords = vec2(0.25 - a_texcoords.y, a_texcoords.x);
         break;
      default: // Fallback case (No Rotation)
         v_texcoords = a_texcoords;
         break;
   }

   switch (int(a_texture)) {
      case 0: // Apple
         v_texcoords += vec2(0.0, 0.75);
         break;
      case 1: // Head
         v_texcoords += vec2(0.25, 0.75);
         gl_Position.z -= 0.01;
         break;
      case 2: // Tail
         v_texcoords += vec2(0.5, 0.75);
         gl_Position.z -= 0.01;
         break;
      case 3: // Half-Body
         v_texcoords += vec2(0.75, 0.75);
         break;
      case 4: // Cross
         v_texcoords += vec2(0.0,  0.5);
         break;
      case 5: // Body
         v_texcoords += vec2(0.25,  0.5);
         break;
      case 6: // Half-Cross
         v_texcoords += vec2(0.5,  0.5);
         break;
      case 13: // Empty
         break;
      default: // Fallback case (Apple)
         v_texcoords += vec2(0.0, 0.75);
         break;
   }

};

#shader fragment
#version 330 core

in vec2 v_texcoords;

out vec4 FragColor;

uniform sampler2D u_textureatlas;

void main()
{
   vec4 texColor = texture(u_textureatlas, v_texcoords);

   if (texColor.a < 0.5)
   {
        discard;
   }

   FragColor = texColor;
};