// Version d'OpenGL
#version 330

in vec2 vert_texCoord;
in vec3 vert_normal;
in vec3 light_dir;
in vec3 view_dir;

out vec4 frag_color;

uniform sampler2D texSampler;

// Fonction appellee pour chaque fragment
void main() {
  vec3 normal = normalize(vert_normal);
  
  float t = dot(-light_dir, normal);
  t = max(t, 0.0);
  
  vec3 color = vec3(vert_texCoord.xy, 0.5);
  
  frag_color = vec4(color, 1.0);
  
  // TODO: récupérer la couleur correspondant à ce fragment dans la texture
}
