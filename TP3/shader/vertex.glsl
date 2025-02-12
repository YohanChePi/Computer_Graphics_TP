// Version d'OpenGL
#version 330

// Donnees d'entree
layout(location=0) in vec3 in_position;
layout(location=1) in vec3 in_color;
layout(location=2) in vec3 in_normal;


// Donnees de sortie
out vec4 my_color;

// Parametres
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform int mode;
uniform float cur_time;

// Fonction appellee pour chaque sommet
void main()
{
  // Affectation de la position du sommet
  // gl_Position est definit par defaut dan GLSL
  //gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(in_position, 1.0);
  gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(in_position[0]+sin(cur_time), in_position[1]+sin(cur_time), in_position[2], 1.0);
  //gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(in_position[0]*sin(cur_time), in_position[1], in_position[2], 1.0);
  
  // Affectation de la couleur du sommet
  switch(mode)
  {
    case 0: // Color
      my_color = vec4( in_color, 1.0);
      break;
    case 1: // Normal
      my_color = vec4( in_normal, 1.0);
      break;
    case 2: // Position
      my_color = vec4( in_position, 1.0);
      break;
    case 3: //Profondeur depuis la camera
      my_color = vec4( gl_Position.z, gl_Position.z, gl_Position.z, 1.0);
      break;
  }
}
