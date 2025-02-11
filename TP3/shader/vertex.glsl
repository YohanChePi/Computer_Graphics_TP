// Version d'OpenGL
#version 330

// Donnees d'entree
layout(location=0) in vec3 in_position;
layout(location=1) in vec3 in_color;

// Donnees de sortie
out vec4 my_color;

// Parametres
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

// Fonction appellee pour chaque sommet
void main()
{
  // Affectation de la position du sommet
  // gl_Position est definit par defaut dan GLSL
  gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(in_position, 1.0);

  my_color = vec4( in_color, 1.0);
}
