// Version d'OpenGL
#version 330

// Donnee d'entree
in  vec4 my_color;

// Donnee de sortie
out vec4 frag_color;

// Fonction appellee pour chaque fragment
void main()
{
  // Affectation de la couleur du fragment
  frag_color = my_color;
}
