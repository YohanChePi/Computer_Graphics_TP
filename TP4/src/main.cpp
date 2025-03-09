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

#include "Mesh.h"

// Dimensions de la fenêtre :
#define WIDTH 1000.0f
#define HEIGHT 800.0f

using namespace glm;
using namespace std;

void view_control( GLFWwindow* myWindow, mat4& view_matrix, float dx, float& brillance);

vec3 getMousePosition(GLFWwindow *myWindow);

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

  // Ouverture d'une fenêtre en 1024x768
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
  glClearColor(0.1, 0.1, 0.1, 0.0);

  // Activation du test de profondeur
  glEnable(GL_DEPTH_TEST);

  // Activation du test de culling, qui évite de rasteriser les faces orientées vers l'arrière
  glEnable(GL_CULL_FACE);

  //-------------------------------------------------
  // Initialisation du shader programm
    
  // Compilation du shader programm
  GLuint programID = LoadShaders( "../shader/vertex.glsl", "../shader/fragment.glsl" );
  cout << "programID = " << programID << endl;

  GLuint programID2 = LoadShaders( "../shader/vertex2.glsl", "../shader/fragment2.glsl" );
  cout << "programID2 = " << programID2 << endl;

  //-------------------------------------------------
  // Initialisation du maillage
  //string file_name("../doc/double_torus.off");
  
  string file_name("../models/blob2.off");
  // string file_name("../models/armadillo.off");
  //string file_name("../models/buddha.off");
  //string file_name("../models/dragon.off");
  // string file_name("../models/triceratops.off");
  // string file_name("../models/cube.off");
  // string file_name("../models/bunny.off");
  cout << "chargement de '" << file_name << "'..." << endl;
  Mesh m(file_name.c_str());
  m.normalize();
  m.colorize();
  cout << "...fini. " << m.nb_vertices << " sommets, " << m.nb_faces << " triangles." << endl;
  
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
  glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(vec3), m.vertices.data(), GL_STATIC_DRAW);

  // Obtention de l'ID de l'attribut "in_position" dans programID
  GLuint vertexPositionID = glGetAttribLocation(programID, "in_position");
  
  // On autorise et indique a OpenGL comment lire les donnees
  glVertexAttribPointer(vertexPositionID,3,GL_FLOAT,GL_FALSE,0,(void*)0);
  glEnableVertexAttribArray(vertexPositionID);


  //==================================================
  // Done : Creation d'un buffer (VBO) pour les couleurs des sommets
  // avec colorBufferID pour identifiant
  //==================================================
  GLuint colorBufferID;
  glGenBuffers(1, &colorBufferID);
  cout << "colorBufferID = " << vertexBufferID << endl;

  // Definition de vertexBufferID comme le buffer courant
  glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);

  // Copie des donnees sur la carte graphique (dans vertexBufferID)
  glBufferData(GL_ARRAY_BUFFER, m.colors.size() * sizeof(vec3), m.colors.data(), GL_STATIC_DRAW);

  // Obtention de l'ID de l'attribut "in_position" dans programID
  GLuint vertexColorID = glGetAttribLocation(programID, "in_color");
  
  // On autorise et indique a OpenGL comment lire les donnees
  glVertexAttribPointer(vertexColorID,3,GL_FLOAT,GL_FALSE,0,(void*)0);
  glEnableVertexAttribArray(vertexColorID);


  //==================================================
  // Done : Creation d'un buffer (VBO) pour les normales des sommets
  // avec normalBufferID pour identifiant
  //==================================================
  GLuint normalBufferID;
  glGenBuffers(1, &normalBufferID);
  cout << "normalBufferID = " << vertexBufferID << endl;

  // Definition de vertexBufferID comme le buffer courant
  glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);

  // Copie des donnees sur la carte graphique (dans vertexBufferID)
  glBufferData(GL_ARRAY_BUFFER, m.normals.size() * sizeof(vec3), m.normals.data(), GL_STATIC_DRAW);

  // Obtention de l'ID de l'attribut "in_position" dans programID
  GLuint vertexNormalID = glGetAttribLocation(programID, "in_normal");
  
  // On autorise et indique a OpenGL comment lire les donnees
  glVertexAttribPointer(vertexNormalID,3,GL_FLOAT,GL_FALSE,0,(void*)0);
  glEnableVertexAttribArray(vertexNormalID);



  //==================================================
  // Creation d'un nouveau buffer pour les indices des triangles (topologie)
  //==================================================
  GLuint indiceBufferID;
  glGenBuffers(1, &indiceBufferID);

  // Definition de vertexBufferID comme le buffer courant
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBufferID);

  // Copie des donnees sur la carte graphique (dans vertexBufferID)
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.faces.size() * sizeof(uint),
	       m.faces.data(), GL_STATIC_DRAW);



  glBindVertexArray(0); // Désactiver le VAO




  //-------------------------------------------------
  // Initialisation des matrices MVP

    
  // Definition des matrices de transformation
  mat4 projection_matrix = perspective(45.0f, WIDTH / HEIGHT, 0.1f, 100.0f);
  mat4 view_matrix = lookAt(vec3(0.0, 0.0, 2), vec3(0.0), vec3(0.0, 1.0, 0.0));
  mat4 model_matrix = scale(mat4(1.0f), vec3(0.5));

  GLuint PmatrixID = glGetUniformLocation(programID, "ProjectionMatrix");
  cout << "PmatrixID = " << PmatrixID << endl;

  GLuint VmatrixID = glGetUniformLocation(programID, "ViewMatrix");
  cout << "VmatrixID = " << VmatrixID << endl;

  GLuint MmatrixID = glGetUniformLocation(programID, "ModelMatrix");
  cout << "MmatrixID = " << MmatrixID << endl;

  GLuint PmatrixID2 = glGetUniformLocation(programID2, "ProjectionMatrix");
  cout << "PmatrixID2 = " << PmatrixID2 << endl;

  GLuint VmatrixID2 = glGetUniformLocation(programID2, "ViewMatrix");
  cout << "VmatrixID2 = " << VmatrixID2 << endl;

  GLuint MmatrixID2 = glGetUniformLocation(programID2, "ModelMatrix");
  cout << "MmatrixID2 = " << MmatrixID2 << endl;


  // Definition de la position de la lumiere
  vec3 lightPos = vec3(1.0, 1.0, 0.0);
  GLuint lightID = glGetUniformLocation(programID, "lightPos");
  cout << "lightID = " << lightID << endl;

  // Definition de la brillance
  float brillance = 1.;
  GLuint brillanceID = glGetUniformLocation(programID, "brillance");
  cout << "brillanceID = " << brillanceID << endl;




  //==================================================
  //=========== Debut des choses serieuses ===========
  //==================================================


  cout << "Debut de la boucle principale..." << endl;

  double init_time = glfwGetTime();
  double prec_time = init_time;
  double cur_time = init_time;
  double speed = 2.0;

  char title[100];
  // Boucle de dessin
  do{
    // Nettoyage de la zone de dessin (couleurs+profondeurs)
    int w,h;
    glfwGetFramebufferSize( myWindow, &w, &h);
    glViewport(0,0,w,h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //==================================================
    //============= Calcul du Point de Vue =============
    //==================================================
      
    prec_time = cur_time;
    cur_time = glfwGetTime() - init_time;
    double delta_time = cur_time - prec_time;
    snprintf(title,100,"SI_INFO4 - %2.0f FPS",1.0/ delta_time);
    glfwSetWindowTitle( myWindow, title);

    view_control( myWindow, view_matrix, speed * delta_time, brillance);


    //==================================================
    //===================== Dessin =====================
    //==================================================

    // Definition de programID comme le shader courant
    glUseProgram(programID);

    // Transmission des matrices au vertex shader
    glUniformMatrix4fv(PmatrixID, 1, GL_FALSE, value_ptr(projection_matrix));
    glUniformMatrix4fv(VmatrixID, 1, GL_FALSE, value_ptr(view_matrix));
    glUniformMatrix4fv(MmatrixID, 1, GL_FALSE, value_ptr(model_matrix));

    glUseProgram(programID2);

    // Transmission des matrices au vertex shader du second programme
    glUniformMatrix4fv(PmatrixID2, 1, GL_FALSE, value_ptr(projection_matrix));
    glUniformMatrix4fv(VmatrixID2, 1, GL_FALSE, value_ptr(view_matrix));
    glUniformMatrix4fv(MmatrixID2, 1, GL_FALSE, value_ptr(model_matrix));

    // Definition de programID comme le shader courant
    glUseProgram(programID);

    // Transmission de la position de la lumiere au vertex shader
    lightPos = getMousePosition(myWindow);
    glUniform3fv(lightID, 1, value_ptr(lightPos));

    // Transmission de la brillance au vertex shader
    glUniform1f(brillanceID, brillance);
    
    // set viewport, enable VAO and draw 
    glViewport(0,0,w,h);

    glBindVertexArray(vaoID);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  
    glDrawElements(GL_TRIANGLES,m.faces.size(),GL_UNSIGNED_INT,0);
    glBindVertexArray(0);
    
    /*
    // Definition de programID2 comme le shader courant
    glUseProgram(programID2);

    glBindVertexArray(vaoID);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES,m.faces.size(),GL_UNSIGNED_INT,0);
    glBindVertexArray(0);
    */

    // Echange des zones de dessin buffers
    glfwSwapBuffers( myWindow);

    // Traitement des évènements fenêtre, clavier, etc...
    glfwPollEvents();


      
    cout << "Temps ecoule (s) : " << cur_time << "\r";
    cout << "Position de la lumiere : " << lightPos.x << " " << lightPos.y << " " << lightPos.z << "\r";
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
  glDeleteBuffers(1, &colorBufferID);
  glDeleteBuffers(1, &normalBufferID);
  glDeleteBuffers(1, &indiceBufferID);



  cout << "Fin du programme..." << endl;
    
    
  return EXIT_SUCCESS;
}

void view_control(   GLFWwindow* myWindow, mat4& view_matrix, float dx, float& brillance) {
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

  if (glfwGetKey( myWindow, GLFW_KEY_R ) == GLFW_PRESS) {
    brillance += 0.1;
  }

  if (glfwGetKey( myWindow, GLFW_KEY_F ) == GLFW_PRESS) {
    brillance -= 0.1;
    brillance = std::max(0.0f, brillance);
  }
  

}

vec3 getMousePosition(GLFWwindow* window) {
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  float x = (2.0f * xpos) / width - 1.0f;
  float y = 1.0f - (2.0f * ypos) / height;
  return vec3(-x, -y, 0);
}
