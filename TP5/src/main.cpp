#include <iostream>
#include <vector>
#include <string>
#include "shader.h" // Help to load shaders from files
// Include GLEW : Always include it before glfw.h et gl.h :)
#include <GL/glew.h>    // OpenGL Extension Wrangler Library : http://glew.sourceforge.net/ 
#include <GLFW/glfw3.h>    // Window, keyboard, mouse : http://www.glfw.org/
#include <glm/glm.hpp>  // OpenGL Mathematics : http://glm.g-truc.net/0.9.5/index.html
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include "stb_image.h"



#include "Mesh.h"
// Dimensions de la fenêtre :
#define WIDTH 1000.0f
#define HEIGHT 800.0f

using namespace glm;
using namespace std;

void view_control( GLFWwindow* myWindow, mat4& view_matrix, float dx);
void create_cube(Mesh* output);
void create_sphere(Mesh* output);

void load_texture( string filename, int &width, int &height, int &nrChannels, float* &data);

int main() {
  GLFWwindow* myWindow;
  
  cout << "Debut du programme..." << endl;

  //==================================================
  //============= Creation de la fenetre =============
  //==================================================
  

  // Initialisation de GLFW
  if(!glfwInit()) {
    cout << "Echec de l'initialisation de GLFW" << endl;
    exit(EXIT_FAILURE);
  }
  
  glfwWindowHint(GLFW_SAMPLES, 4); // Anti Aliasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Ouverture d'une fenêtre
  // et creation d'un contexte OpenGL
  if (!(myWindow = glfwCreateWindow( WIDTH, HEIGHT, "SI_INFO4", NULL, NULL))) {
    cout << "Echec de l'ouverture de fenetre OpenGL" << endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(myWindow);

  // Autorise GLFW a recevoir les appuis de touche
  glfwSetInputMode( myWindow, GLFW_STICKY_KEYS, GL_TRUE);

  // Initialisation de  GLEW
  glewExperimental = GL_TRUE;
  if(glewInit() != GLEW_OK) {
    cout << "Echec de l'initialisation de GLEW" << endl;
    exit(EXIT_FAILURE);
  }

  // Verification des donnees du contexte OpenGL
  const GLubyte* renderer = glGetString (GL_RENDERER);
  cout << "Carte Graphique : " << renderer << endl;
  
  const GLubyte* version = glGetString (GL_VERSION);
  cout << "Driver OpenGL : " << version << endl;
  

  //==================================================
  //================= Initialisation =================
  //==================================================
  
  cout << "Initialisations..." << endl;


  // Definition de la couleur du fond
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  //-------------------------------------------------
  // Initialisation du shader programm
    
  // Compilation du shader programm
  GLuint programID = LoadShaders( "../shader/vertex.glsl", "../shader/fragment.glsl" );
  cout << "programID = " << programID << endl;

  
  Mesh m;
  //create_sphere(&m);
  create_cube(&m);


  
  // Creation d'un VAO (c'est l'objet qui encapsule les VBOs et qu'on va manipuler)
  GLuint vaoID;
  glGenVertexArrays(1,&vaoID); 
  glBindVertexArray(vaoID); // rendre ce VAO actif
    
  //==================================================
  // Done : Creation d'un buffer (VBO) pour les positions des sommets
  // avec vertexBufferID pour identifiant
  //==================================================

  GLuint vertexBufferID;
  glGenBuffers(1, &vertexBufferID);
  cout << "vertexBufferID = " << vertexBufferID << endl;

  // Definition de vertexBufferID comme le buffer courant
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);

  // Copie des donnees sur la carte graphique (dans vertexBufferID)
  glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m.vertices.size(), m.vertices.data(), GL_STATIC_DRAW);

  // Obtention de l'ID de l'attribut "in_position" dans programID
  GLuint vertexPositionID = glGetAttribLocation(programID, "in_position");
  
  // On autorise et indique a OpenGL comment lire les donnees
  glVertexAttribPointer(vertexPositionID,3,GL_FLOAT,GL_FALSE,0,(void*)0);
  glEnableVertexAttribArray(vertexPositionID);


  //==================================================
  // Done : Creation d'un buffer (VBO) pour les normales des sommets
  // avec normalBufferID pour identifiant
  //==================================================

  GLuint normalBufferID;
  glGenBuffers(1, &normalBufferID);
  cout << "normalBufferID = " << normalBufferID << endl;

  // Definition de normalBufferID comme le buffer courant
  glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);

  // Copie des donnees sur la carte graphique (dans normalBufferID)
  glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m.normals.size(), m.normals.data(), GL_STATIC_DRAW);

  // Obtention de l'ID de l'attribut "in_position" dans programID
  GLuint vertexNormalID = glGetAttribLocation(programID, "in_normal");
  
  // On autorise et indique a OpenGL comment lire les donnees
  glVertexAttribPointer(vertexNormalID,3,GL_FLOAT,GL_TRUE,0,(void*)0);
  glEnableVertexAttribArray(vertexNormalID);

  //==================================================
  // Done : Creation d'un buffer (VBO) pour les coord de texture  des sommets
  // avec texcoordBufferID pour identifiant
  //==================================================

  GLuint texcoordBufferID;
  glGenBuffers(1, &texcoordBufferID);
  cout << "texcoordBufferID = " << texcoordBufferID << endl;

  // Definition de texcoordBufferID comme le buffer courant
  glBindBuffer(GL_ARRAY_BUFFER, texcoordBufferID);

  // Copie des donnees sur la carte graphique (dans texcoordBufferID)
  glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * m.texCoord.size(), m.texCoord.data(), GL_STATIC_DRAW);

  // Obtention de l'ID de l'attribut "in_position" dans programID
  GLuint vertexTexcoordID = glGetAttribLocation(programID, "in_texcoord");
  
  // On autorise et indique a OpenGL comment lire les donnees
  glVertexAttribPointer(vertexTexcoordID,2,GL_FLOAT,GL_FALSE,0,(void*)0);
  glEnableVertexAttribArray(vertexTexcoordID);


  //==================================================
  // Creation d'un nouveau buffer pour les indices des triangles (topologie)
  //==================================================
  GLuint indiceBufferID;
  glGenBuffers(1, &indiceBufferID);

  // Definition de vertexBufferID comme le buffer courant
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBufferID);

  // Copie des donnees sur la carte graphique (dans vertexBufferID)
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m.faces.size(), m.faces.data(), GL_STATIC_DRAW);

  glBindVertexArray(0); // Désactiver le VAO


  //==================================================
  // Matrices de transformation
  //==================================================
  mat4 projection_matrix = perspective(45.0f, WIDTH / HEIGHT, 0.1f, 100.0f);
  mat4 view_matrix = lookAt(vec3(1.0, 2.0, 1.0), vec3(0.0), vec3(0.0, 0.0, 1.0));
  mat4 model_matrix = scale(mat4(1.0f), vec3(0.5));
  
  GLuint PmatrixID = glGetUniformLocation(programID, "ProjectionMatrix");
  cout << "PmatrixID = " << PmatrixID << endl;
  
  GLuint VmatrixID = glGetUniformLocation(programID, "ViewMatrix");
  cout << "VmatrixID = " << VmatrixID << endl;
  
  GLuint MmatrixID = glGetUniformLocation(programID, "ModelMatrix");
  cout << "MmatrixID = " << MmatrixID << endl;



  //==================================================
  // TODO : créer la texture 
  // TODO : recuperer l'identifiant de "texSampler" dans le fragment shader 
  //==================================================
  int width, height, nrChannels;
  float* data;
  //load_texture("../textures/chessMulti.jpg", width, height, nrChannels, data);
  //load_texture("../textures/crate.jpg", width, height, nrChannels, data);
  load_texture("../textures/dice_texture_uv_map.jpg", width, height, nrChannels, data);


  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  cout << "textureID = " << textureID << endl;

  // Parametres de la texture
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Envoie de la texture sur la carte graphique
  glTexImage2D(GL_TEXTURE_2D, 0,
               GL_RGBA32F,
               width,
               height, 0,
               GL_RGBA, GL_FLOAT,
               (const GLvoid *)data);

  GLuint texSamplerID = glGetUniformLocation(programID, "texSampler");

  //==================================================
  //=========== Debut des choses serieuses ===========
  //==================================================

  cout << "Debut de la boucle principale..." << endl;

  double init_time = glfwGetTime();
  double prec_time = init_time;
  double cur_time = init_time;
  double speed = 1.0;
    
  char title[100];
  // Boucle de dessin
  do{
    // Nettoyage de la zone de dessin (couleurs+profondeurs)
    int w,h;
    glfwGetWindowSize( myWindow, &w, &h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calcul du temps ecoule et affichage en titre de fenetre
    prec_time = cur_time;
    cur_time = glfwGetTime() - init_time;
    double delta_time = cur_time - prec_time;
    snprintf(title,100,"SI_INFO4 - %2.0f FPS", 1.0/ delta_time);
    glfwSetWindowTitle( myWindow, title);

    //==================================================
    //============= Calcul du Point de Vue =============
    //==================================================
      
    view_control( myWindow, view_matrix, speed * delta_time);

    //==================================================
    //===================== Dessin =====================
    //==================================================

    // Definition de programID comme le shader courant
    glUseProgram(programID);

    // Transmission des matrices au vertex shader
    glUniformMatrix4fv(PmatrixID, 1, GL_FALSE, value_ptr(projection_matrix));
    glUniformMatrix4fv(VmatrixID, 1, GL_FALSE, value_ptr(view_matrix));
    glUniformMatrix4fv(MmatrixID, 1, GL_FALSE, value_ptr(model_matrix));

    //==================================================
    // TODO : envoie de la texture au shader 
    // Note: la texture est déjà sur GPU, il suffit de lier la texture
    // a une unité, puis de spécifier cette unité au shader 
    //==================================================
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(texSamplerID, 0);

    // set viewport, enable VAO and draw 
    glViewport(0,0,w,h);
    glBindVertexArray(vaoID);
    glDrawElements(GL_TRIANGLES,m.faces.size(),GL_UNSIGNED_INT,(void*)0);
    glBindVertexArray(0);

    // Echange des zones de dessin buffers
    glfwSwapBuffers( myWindow);

    // Traitement des évènements fenêtre, clavier, etc...
    glfwPollEvents();
      
    cout << "Temps ecoule (s) : " << cur_time << "\r";
    cout.flush();
    
    
  } // Execution de la boucle...
  while( glfwGetKey( myWindow, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&  // ... jusqu'a appui sur la touche ESC
	 (!glfwWindowShouldClose( myWindow))        );  // ... ou fermeture de la fenetre

  cout << endl;

  // Ferme GLFW et OpenGL
  glfwTerminate();
    
    
    
  //==================================================
  //============== Nettoyage la memoire ==============
  //==================================================
    
  // Liberation des buffers
  glDeleteBuffers(1, &vaoID);
  glDeleteBuffers(1, &vertexBufferID);
  glDeleteBuffers(1, &normalBufferID);
  glDeleteBuffers(1, &texcoordBufferID);
  glDeleteBuffers(1, &indiceBufferID);

  // Liberation des textures
  glDeleteTextures(1, &textureID);

  cout << "Fin du programme..." << endl;
    
    
  return EXIT_SUCCESS;
}


void create_cube(Mesh* output)
{

    output->vertices.push_back(vec3(-1, -1, -1));
    output->vertices.push_back(vec3( 1, -1, -1));
    output->vertices.push_back(vec3( 1,  1, -1));
    output->vertices.push_back(vec3(-1,  1, -1));
    
    output->vertices.push_back(vec3(-1, -1, -1));
    output->vertices.push_back(vec3(-1,  1, -1));
    output->vertices.push_back(vec3(-1,  1,  1));
    output->vertices.push_back(vec3(-1, -1,  1));
    
    output->vertices.push_back(vec3(-1, -1, -1));
    output->vertices.push_back(vec3(-1, -1,  1));
    output->vertices.push_back(vec3( 1, -1,  1));
    output->vertices.push_back(vec3( 1, -1, -1));
    
    output->vertices.push_back(vec3( 1,  1,  1));
    output->vertices.push_back(vec3(-1,  1,  1));
    output->vertices.push_back(vec3(-1, -1,  1));
    output->vertices.push_back(vec3( 1, -1,  1));
    
    output->vertices.push_back(vec3( 1,  1,  1));
    output->vertices.push_back(vec3( 1, -1,  1));
    output->vertices.push_back(vec3( 1, -1, -1));
    output->vertices.push_back(vec3( 1,  1, -1));
    
    output->vertices.push_back(vec3( 1,  1,  1));
    output->vertices.push_back(vec3( 1,  1, -1));
    output->vertices.push_back(vec3(-1,  1, -1));
    output->vertices.push_back(vec3(-1,  1,  1));
                        
    output->normals.push_back(vec3(0, 0, -1));
    output->normals.push_back(vec3(0, 0, -1));
    output->normals.push_back(vec3(0, 0, -1));
    output->normals.push_back(vec3(0, 0, -1));
    
    output->normals.push_back(vec3(-1, 0, 0));
    output->normals.push_back(vec3(-1, 0, 0));
    output->normals.push_back(vec3(-1, 0, 0));
    output->normals.push_back(vec3(-1, 0, 0));
    
    output->normals.push_back(vec3(0, -1, 0));
    output->normals.push_back(vec3(0, -1, 0));
    output->normals.push_back(vec3(0, -1, 0));
    output->normals.push_back(vec3(0, -1, 0));
    
    output->normals.push_back(vec3(0, 0,  1));
    output->normals.push_back(vec3(0, 0,  1));
    output->normals.push_back(vec3(0, 0,  1));
    output->normals.push_back(vec3(0, 0,  1));
    
    output->normals.push_back(vec3( 1, 0, 0));
    output->normals.push_back(vec3( 1, 0, 0));
    output->normals.push_back(vec3( 1, 0, 0));
    output->normals.push_back(vec3( 1, 0, 0));
    
    output->normals.push_back(vec3(0,  1, 0));
    output->normals.push_back(vec3(0,  1, 0));
    output->normals.push_back(vec3(0,  1, 0));
    output->normals.push_back(vec3(0,  1, 0));
                        
    // TODO : definir les coordonnées de texture des sommets du cube
    /* Version longue manuelle*/
    /*
    output->texCoord.push_back(vec2(0, 0));
    output->texCoord.push_back(vec2(1, 0));
    output->texCoord.push_back(vec2(1, 1));
    output->texCoord.push_back(vec2(0, 1));

    output->texCoord.push_back(vec2(0, 0));
    output->texCoord.push_back(vec2(1, 0));
    output->texCoord.push_back(vec2(1, 1));
    output->texCoord.push_back(vec2(0, 1));

    output->texCoord.push_back(vec2(0, 0));
    output->texCoord.push_back(vec2(1, 0));
    output->texCoord.push_back(vec2(1, 1));
    output->texCoord.push_back(vec2(0, 1));

    output->texCoord.push_back(vec2(0, 0));
    output->texCoord.push_back(vec2(1, 0));
    output->texCoord.push_back(vec2(1, 1));
    output->texCoord.push_back(vec2(0, 1));

    output->texCoord.push_back(vec2(0, 0));
    output->texCoord.push_back(vec2(1, 0));
    output->texCoord.push_back(vec2(1, 1));
    output->texCoord.push_back(vec2(0, 1));

    output->texCoord.push_back(vec2(0, 0));
    output->texCoord.push_back(vec2(1, 0));
    output->texCoord.push_back(vec2(1, 1));
    output->texCoord.push_back(vec2(0, 1));
    */

    /* Version courte boucle */
    /*
    for (int i = 0; i < 6; i++) {
        output->texCoord.push_back(vec2(0, 0));
        output->texCoord.push_back(vec2(1, 0));
        output->texCoord.push_back(vec2(1, 1));
        output->texCoord.push_back(vec2(0, 1));
    }
    */
    

    //Coordonnées pour le dé
    
    output->texCoord.push_back(vec2(2./4., 1./3.));
    output->texCoord.push_back(vec2(3./4., 1./3.));
    output->texCoord.push_back(vec2(3./4., 2./3.));
    output->texCoord.push_back(vec2(2./4., 2./3.));

    output->texCoord.push_back(vec2(1./4., 1./3.));
    output->texCoord.push_back(vec2(2./4., 1./3.));
    output->texCoord.push_back(vec2(2./4., 2./3.));
    output->texCoord.push_back(vec2(1./4., 2./3.));

    output->texCoord.push_back(vec2(0./4., 1./3.));
    output->texCoord.push_back(vec2(1./4., 1./3.));
    output->texCoord.push_back(vec2(1./4., 2./3.));
    output->texCoord.push_back(vec2(0./4., 2./3.));

    output->texCoord.push_back(vec2(3./4., 1./3.));
    output->texCoord.push_back(vec2(4./4., 1./3.));
    output->texCoord.push_back(vec2(4./4., 2./3.));
    output->texCoord.push_back(vec2(3./4., 2./3.));

    output->texCoord.push_back(vec2(1./4., 1./3.));
    output->texCoord.push_back(vec2(2./4., 1./3.));
    output->texCoord.push_back(vec2(2./4., 2./3.));
    output->texCoord.push_back(vec2(1./4., 2./3.));

    output->texCoord.push_back(vec2(2./4., 2./3.));
    output->texCoord.push_back(vec2(3./4., 2./3.));
    output->texCoord.push_back(vec2(3./4., 3./3.));
    output->texCoord.push_back(vec2(2./4., 3./3.));
    

    output->faces.push_back(0);
    output->faces.push_back(2);
    output->faces.push_back(1);

    output->faces.push_back(0);
    output->faces.push_back(3);
    output->faces.push_back(2);


    output->faces.push_back(4);
    output->faces.push_back(6);
    output->faces.push_back(5);

    output->faces.push_back(4);
    output->faces.push_back(7);
    output->faces.push_back(6);


    output->faces.push_back(8);
    output->faces.push_back(10);
    output->faces.push_back(9);

    output->faces.push_back(8);
    output->faces.push_back(11);
    output->faces.push_back(10);


    output->faces.push_back(12);
    output->faces.push_back(13);
    output->faces.push_back(14);

    output->faces.push_back(12);
    output->faces.push_back(14);
    output->faces.push_back(15);


    output->faces.push_back(16);
    output->faces.push_back(17);
    output->faces.push_back(18);

    output->faces.push_back(16);
    output->faces.push_back(18);
    output->faces.push_back(19);


    output->faces.push_back(20);
    output->faces.push_back(21);
    output->faces.push_back(22);

    output->faces.push_back(20);
    output->faces.push_back(22);
    output->faces.push_back(23);                            
}




void create_sphere(Mesh* output) {
  int N = 100;
  int Nu = 2 * N;
  int Nv = N;
  
  for(int i = 0; i < Nu; i++) {
    float u = float(i) / (Nu-1);
    float phi = u * M_PI * 2;
    
    for(int j = 0; j < Nv; j++) {
      float v = float(j) / (Nv-1);
      float psi = v * M_PI;
      
      vec3 p(cos(phi)*sin(psi), sin(phi)*sin(psi), cos(psi));
      output->vertices.push_back(p);
      
      output->normals.push_back(p);
      
      vec2 t(u, v);
      output->texCoord.push_back(t);
    }
  }
  
  
  vector<unsigned int> faces;
  for(int i = 0; i < Nu; i++) {
    for(int j = 0; j < Nv - 1; j++) {
      output->faces.push_back( i * Nv + j);
      output->faces.push_back( i * Nv + j+1);
      output->faces.push_back((i + 1) % Nu * Nv + j);

      output->faces.push_back( i * Nv + j+1);
      output->faces.push_back((i+1)%Nu * Nv + j + 1);
      output->faces.push_back((i + 1) % Nu * Nv + j);
    }
  }
}


void view_control(   GLFWwindow* myWindow, mat4& view_matrix, float dx) {
  if (glfwGetKey( myWindow, GLFW_KEY_UP ) == GLFW_PRESS) {
    vec4 axis = vec4(1.0, 0.0, 0.0, 0.0);
    axis = inverse(view_matrix) * axis;
    view_matrix = rotate(view_matrix, -dx, vec3(axis));
  }

  if (glfwGetKey( myWindow, GLFW_KEY_DOWN ) == GLFW_PRESS) {
    vec4 axis = vec4(1.0, 0.0, 0.0, 0.0);
    axis = inverse(view_matrix) * axis;
    view_matrix = rotate(view_matrix, +dx, vec3(axis));
  }

  if (glfwGetKey( myWindow, GLFW_KEY_RIGHT ) == GLFW_PRESS) {
    vec4 axis = vec4(0.0, 1.0, 0.0, 0.0);
    axis = inverse(view_matrix) * axis;
    view_matrix = rotate(view_matrix, dx, vec3(axis));
  }

  if (glfwGetKey( myWindow, GLFW_KEY_LEFT ) == GLFW_PRESS) {
    vec4 axis = vec4(0.0, 1.0, 0.0, 0.0);
    axis = inverse(view_matrix) * axis;
    view_matrix = rotate(view_matrix, -dx, vec3(axis));
  }

  if (glfwGetKey( myWindow, GLFW_KEY_PAGE_UP ) == GLFW_PRESS) {
    vec4 axis = vec4(0.0, 0.0, 1.0, 0.0);
    axis = inverse(view_matrix) * axis;
    view_matrix = rotate(view_matrix, -dx, vec3(axis));
  }

  if (glfwGetKey( myWindow, GLFW_KEY_PAGE_DOWN ) == GLFW_PRESS) {
    vec4 axis = vec4(0.0, 0.0, 1.0, 0.0);
    axis = inverse(view_matrix) * axis;
    view_matrix = rotate(view_matrix, +dx, vec3(axis));
  }
  
  if (glfwGetKey( myWindow, GLFW_KEY_Z ) == GLFW_PRESS) {
    vec4 axis = vec4(0.0, 0.0, 1.0, 0.0) * dx;
    axis = inverse(view_matrix) * axis;
    view_matrix = translate(view_matrix, vec3(axis));
  }

  if (glfwGetKey( myWindow, GLFW_KEY_S ) == GLFW_PRESS) {
    vec4 axis = vec4(0.0, 0.0, 1.0, 0.0) * (-dx);
    axis = inverse(view_matrix) * axis;
    view_matrix = translate(view_matrix, vec3(axis));
  }

  if (glfwGetKey( myWindow, GLFW_KEY_Q) == GLFW_PRESS) {
    vec4 axis = vec4(-1.0, 0.0, 0.0, 0.0) * dx;
    axis = inverse(view_matrix) * axis;
    view_matrix = translate(view_matrix, vec3(axis));
  }

  if (glfwGetKey( myWindow, GLFW_KEY_D ) == GLFW_PRESS) {
    vec4 axis = vec4(-1.0, 0.0, 0.0, 0.0) * (-dx);
    axis = inverse(view_matrix) * axis;
    view_matrix = translate(view_matrix, vec3(axis));
  }

  if (glfwGetKey( myWindow, GLFW_KEY_A ) == GLFW_PRESS) {
    vec4 axis = vec4(0.0, 1.0, 0.0, 0.0) * dx;
    axis = inverse(view_matrix) * axis;
    view_matrix = translate(view_matrix, vec3(axis));
  }

  if (glfwGetKey( myWindow, GLFW_KEY_E ) == GLFW_PRESS) {
    vec4 axis = vec4(0.0, 1.0, 0.0, 0.0) * (-dx);
    axis = inverse(view_matrix) * axis;
    view_matrix = translate(view_matrix, vec3(axis));
  }
}

void load_texture( string filename, int &width, int &height, int &nrChannels, float* &data) {

  // stbi_set_flip_vertically_on_load(true);
  data = stbi_loadf( filename.c_str(), &width, &height, &nrChannels, 4);
  if (data)
  {
    cout << "Image loaded successfully" << endl;
    cout << "Width: " << width << " Height: " << height << " Channels: " << nrChannels << endl;
  }
  else
  {
    cout << "Failed to load image" << endl;
  }
} 
