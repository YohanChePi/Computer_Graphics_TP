// Version d’OpenGL 
#version 120 

// Donnees d’entree 
attribute vec3 vertex_position; 

// Fonction appellee pour chaque sommet 
void main() { 

    // Affectation de la position du sommet 
    gl_Position = vec4( vertex_position, 1.); 
} 