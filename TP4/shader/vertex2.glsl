// Version d'OpenGL
#version 330

// Donnees d'entree
in vec3 in_position;

// Donnees de sortie
out vec4 my_color;

// Parametres
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

// Fonction appellee pour chaque sommet
void main() {
  // Affectation de la position du sommet
  gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(in_position, 1.0);

  // Affectation de la couleur du sommet
  my_color = vec4(1., 0., 0., 1.0);

}
