// Version d'OpenGL
#version 330

in vec4 my_color;
in vec4 posInWorldSpace;
in vec4 normalInWorldSpace;
in vec4 cameraPosInWorldSpace;
in vec4 lightPosInWorldSpace;
in float brillance_shader;

/*
flat in vec4 my_color;
flat in vec4 posInWorldSpace;
flat in vec4 normalInWorldSpace;
flat in vec4 cameraPosInWorldSpace;
*/

out vec4 frag_color;

// Parametre de Phong
vec3 La = vec3(0., 0., 1.); // lumière ambiante bleu
vec3 Ld = my_color.xyz;
vec3 Ls = vec3(1.0, 1.0, 1.0); // éclairage blanc

float rho_a = 0.09; // lumière ambiante faible
float rho_d = 1.;
float rho_s = 0.2;

// Fonction appellee pour chaque fragment
void main() {
  // Calcul de l'equation de Phong
  vec3 lightDir = normalize(vec3(lightPosInWorldSpace) - vec3(posInWorldSpace));
  vec3 viewDir = normalize(vec3(cameraPosInWorldSpace) - vec3(posInWorldSpace));
  vec3 reflectDir = reflect(lightDir, vec3(normalInWorldSpace));

  vec3 phong_ambiant = rho_a * La;
  vec3 phong_diffus = rho_d * Ld * max(-dot(vec3(normalInWorldSpace), lightDir), 0.);
  vec3 phong_specular = rho_s * Ls * pow(max(dot(viewDir, reflectDir), 0.0), brillance_shader);


  // Affectation de la couleur du fragment
  //frag_color = vec4(phong_ambiant + phong_diffus + phong_specular, 1.0);

  //Phong shading
  vec3 color = phong_ambiant + phong_diffus + phong_specular;

  //Toon shading
  //float levels = 15.0; // Number of color levels
  //color = floor(color * levels) / levels; // Reduce the number of possible colors

  frag_color = vec4(color, 1.0);
}
