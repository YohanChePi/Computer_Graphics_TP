// Version d'OpenGL
#version 330

// Donnees d'entree
in vec3 in_position;
in vec3 in_color;
in vec3 in_normal;

// Donnees de sortie
out vec4 my_color;

// Parametres
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

// Parametre de Phong
vec3 La = vec3(0., 0., 1.); //lumière ambiante bleu
vec3 Ld = vec3(0., 0., 1.); //objet bleu
vec3 Ls = vec3(1.0, 1.0, 1.0); //éclairage blanc

float rho_a = 0.09; //lumière ambiante faible
float rho_d = 1.;
float rho_s = 0.2;

float s = 1.;

vec3 lightPosInWorldSpace = vec3(1.,1.,1.);
vec4 cameraPosInWorldSpace = inverse(ViewMatrix) * vec4(0.0, 0.0, 0.0, 1.0);

// Fonction appellee pour chaque sommet
void main() {
  // Affectation de la position du sommet
  // gl_Position est definit par defaut dan GLSL
  gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(in_position, 1.0);
  vec4 posInObjectSpace = vec4( in_position, 1.0);

  vec4 posInWorldSpace = ModelMatrix * posInObjectSpace;
  vec4 normalInWorldSpace = transpose(inverse( ModelMatrix))* vec4( in_normal, 0.);

  // my_color = vec4(in_normal*0.5+0.5,1.0);
  //my_color = posInObjectSpace;
  vec3 phong_ambiant = rho_a * La;
  vec3 lightDir = normalize(lightPosInWorldSpace - vec3(posInWorldSpace));
  vec3 viewDir = normalize(vec3(cameraPosInWorldSpace) - vec3(posInWorldSpace));
  vec3 reflectDir = reflect(lightDir, vec3(normalInWorldSpace));

  vec3 phong_diffus = rho_d * Ld * max(-1*dot(vec3(normalInWorldSpace), lightDir), 0.);
  vec3 phong_specular = rho_s * Ls * pow(max(dot(reflectDir,viewDir), 0.), s);

  my_color = vec4(phong_ambiant + phong_diffus + phong_specular, 1.0); // +
}
