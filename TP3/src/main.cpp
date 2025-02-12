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

void view_control( GLFWwindow* myWindow, mat4& view_matrix, float dx);

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
  
  glfwWindowHint(GLFW_SAMPLES, 16); // Anti Aliasing
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

  // Activation du culling
  glEnable(GL_CULL_FACE);


  //-------------------------------------------------
  // Initialisation du shader programm
    
  // Compilation du shader programm
  GLuint programID = LoadShaders( "../shader/vertex.glsl", "../shader/fragment.glsl" );
  cout << "programID = " << programID << endl;



  //-------------------------------------------------
  // Initialisation des arrays de données
    
  // Definition d'un tableau de vecteurs
  vector<vec3> vertices;
  vertices.push_back(vec3(-1, -1, -1));//0
  vertices.push_back(vec3( 1, -1, -1));//1
  vertices.push_back(vec3( 1, 1, -1));//2
  vertices.push_back(vec3( -1, 1, -1));//3
  vertices.push_back(vec3( -1, -1, 1));//4
  vertices.push_back(vec3( 1, -1, 1));//5
  vertices.push_back(vec3( 1, 1, 1));//6
  vertices.push_back(vec3( -1, 1, 1));//7
  /*
  vertices.push_back(vec3(-1, -1, -1));
  vertices.push_back(vec3( 1,  1, -1));
  vertices.push_back(vec3( 1, -1, -1));
  vertices.push_back(vec3(-1, -1, -1));
  vertices.push_back(vec3(-1,  1, -1));
  vertices.push_back(vec3( 1,  1, -1));

  vertices.push_back(vec3(-1, -1, -1));
  vertices.push_back(vec3(-1,  1,  1));
  vertices.push_back(vec3(-1,  1, -1));
  vertices.push_back(vec3(-1, -1, -1));
  vertices.push_back(vec3(-1, -1,  1));
  vertices.push_back(vec3(-1,  1,  1));

  vertices.push_back(vec3(-1, -1, -1));
  vertices.push_back(vec3( 1, -1,  1));
  vertices.push_back(vec3(-1, -1,  1));
  vertices.push_back(vec3(-1, -1, -1));
  vertices.push_back(vec3( 1, -1, -1));
  vertices.push_back(vec3( 1, -1,  1));

  vertices.push_back(vec3( 1,  1,  1));
  vertices.push_back(vec3(-1,  1,  1));
  vertices.push_back(vec3(-1, -1,  1));
  vertices.push_back(vec3( 1,  1,  1));
  vertices.push_back(vec3(-1, -1,  1));
  vertices.push_back(vec3( 1, -1,  1));

  vertices.push_back(vec3( 1,  1,  1));
  vertices.push_back(vec3( 1, -1,  1));
  vertices.push_back(vec3( 1, -1, -1));
  vertices.push_back(vec3( 1,  1,  1));
  vertices.push_back(vec3( 1, -1, -1));
  vertices.push_back(vec3( 1,  1, -1));

  vertices.push_back(vec3( 1,  1,  1));
  vertices.push_back(vec3( 1,  1, -1));
  vertices.push_back(vec3(-1,  1, -1));
  vertices.push_back(vec3( 1,  1,  1));
  vertices.push_back(vec3(-1,  1, -1));
  vertices.push_back(vec3(-1,  1,  1));
  */
  
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
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), vertices.data(), GL_STATIC_DRAW);

  // Obtention de l'ID de l'attribut "in_position" dans programID
  GLuint vertexPositionID = glGetAttribLocation(programID, "in_position");
  
  // On autorise et indique a OpenGL comment lire les donnees
  glVertexAttribPointer(vertexPositionID,3,GL_FLOAT,GL_FALSE,0,(void*)0);
  glEnableVertexAttribArray(vertexPositionID);

    
  //==================================================
  // Todo 1 : Creation d'un nouveau buffer pour la couleur des sommets
  //==================================================
  vector<vec3> colors;
  /*
  colors.push_back(vec3(1, 0, 0));
  colors.push_back(vec3(0, 1, 0));
  colors.push_back(vec3(0, 0, 1));
  colors.push_back(vec3(1, 1, 1));
  colors.push_back(vec3(0, 0, 0));
  colors.push_back(vec3(1, 1, 0));
  colors.push_back(vec3(0, 1, 1));
  colors.push_back(vec3(1, 0, 1));
  */
  for (int i = 0; i < 10000; i++){
    colors.push_back(vec3( rand() % 100 / 100.0, rand() % 100 / 100.0, rand() % 100 / 100.0));
  }

    // Creation d'un buffer des couleurs
    GLuint colorUnitBufferID;
  glGenBuffers(1, &colorUnitBufferID);

  // Definition de colorUnitBufferID comme le buffer courant
  glBindBuffer(GL_ARRAY_BUFFER, colorUnitBufferID);

  // Copie des couleurs sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), colors.data(), GL_STATIC_DRAW);

  // Obtention de l'ID l'attribut "in_color" dans programID
  GLuint vertexColorID = glGetAttribLocation(programID, "in_color");
  printf("vertexColorID = %d\n", vertexColorID);

  // on indique a OpenGL comment lire les donnees et on autorise leur lecture
  glVertexAttribPointer(vertexColorID, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glEnableVertexAttribArray(vertexColorID);

  //==================================================
  // Todo 2 : Creation d'un nouveau buffer pour les indices des triangles
  //==================================================
  vector<uint> indices;
  // Tableau des indices
  uint desIndices[] = {
      0,3,1, 1,3,2, // Devant
      1,2,5, 5,2,6, // Droite
      5,6,4, 4,6,7, // Derriere
      4,7,3, 4,3,0, // Gauche
      2,7,6, 2,3,7, // Dessus
      0,1,4, 1,5,4  // Dessous
  };
  for (int i = 0; i < 36; i++)
  {
    indices.push_back(desIndices[i]);
  }


  // Creation du buffer des indices
  GLuint indiceBufferID;
  glGenBuffers(1, &indiceBufferID);

  // Definition de vertexBufferID comme le bufferCourant de type GL_ELEMENT_ARRAY_BUFFER
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBufferID);

  // transmission des indices dans ce buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), indices.data(), GL_STATIC_DRAW);

  //==================================================
  // Todo 3 : Creation des buffers avec le chargement d'un maillage
  //==================================================

  Mesh m = Mesh("../models/triceratops.off");
  m.normalize();
  m.colorize();


  //== Creation du buffer des positions des sommets ==
  GLuint vertexMeshBufferID;
  glGenBuffers(1, &vertexMeshBufferID);
  cout << "vertexMeshBufferID = " << vertexMeshBufferID << endl;

  // Definition de vertexMeshBufferID comme le buffer courant
  glBindBuffer(GL_ARRAY_BUFFER, vertexMeshBufferID);

  // Copie des donnees sur la carte graphique (dans vertexMeshBufferID)
  glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(vec3), m.vertices.data(), GL_STATIC_DRAW);

  // Obtention de l'ID de l'attribut "in_position" dans programID
  GLuint vertexMeshPositionID = glGetAttribLocation(programID, "in_position");

  // On autorise et indique a OpenGL comment lire les donnees
  glVertexAttribPointer(vertexMeshPositionID, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glEnableVertexAttribArray(vertexMeshPositionID);


  //== Creation du buffer des indices Mesh ==
  GLuint indiceMeshBufferID;
  glGenBuffers(1, &indiceMeshBufferID);

  // Definition de vertexMeshBufferID comme le bufferCourant de type GL_ELEMENT_ARRAY_BUFFER
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceMeshBufferID);

  // transmission des indices dans ce buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.faces.size() * sizeof(uint), m.faces.data(), GL_STATIC_DRAW);


  //== Creation d'un nouveau buffer pour les couleurs des sommets==
  GLuint colorMeshBufferID;
  glGenBuffers(1, &colorMeshBufferID);

  // Definition de colorMeshBufferID comme le buffer courant
  glBindBuffer(GL_ARRAY_BUFFER, colorMeshBufferID);

  // Copie des couleurs sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER, m.colors.size() * sizeof(vec3), m.colors.data(), GL_STATIC_DRAW);

  // Obtention de l'ID l'attribut "in_color" dans programID
  GLuint vertexMeshColorID = glGetAttribLocation(programID, "in_color");
  printf("vertexMeshColorID = %d\n", vertexMeshColorID);

  // on indique a OpenGL comment lire les donnees et on autorise leur lecture
  glVertexAttribPointer(vertexMeshColorID, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glEnableVertexAttribArray(vertexMeshColorID);


  //== Creation du buffer des normales des sommets ==
  GLuint normalMeshBufferID;
  glGenBuffers(1, &normalMeshBufferID);
  cout << "normalMeshBufferID = " << normalMeshBufferID << endl;

  // Definition de normalMeshBufferID comme le buffer courant
  glBindBuffer(GL_ARRAY_BUFFER, normalMeshBufferID);

  // Copie des donnees sur la carte graphique (dans normalMeshBufferID)
  glBufferData(GL_ARRAY_BUFFER, m.normals.size() * sizeof(vec3), m.normals.data(), GL_STATIC_DRAW);

  // Obtention de l'ID de l'attribut "in_normal" dans programID
  GLuint vertexMeshNormalID = glGetAttribLocation(programID, "in_normal");

  // On autorise et indique a OpenGL comment lire les donnees
  glVertexAttribPointer(vertexMeshNormalID, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glEnableVertexAttribArray(vertexMeshNormalID);

 
  
  //================================================================================================

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

  GLuint modeID = glGetUniformLocation(programID, "mode");
  cout << "modeID = " << modeID << endl;

  GLuint timeID = glGetUniformLocation(programID, "cur_time");
  cout << "timeID = " << timeID << endl;

  


  //==================================================
  //=========== Debut des choses serieuses ===========
  //==================================================


  cout << "Debut de la boucle principale..." << endl;

  double init_time = glfwGetTime();
  double prec_time = init_time;
  double cur_time = init_time;
  double speed = 0.5;

  char title[100];
  // Boucle de dessin
  do{
    // Nettoyage de la zone de dessin (couleurs+profondeurs)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //==================================================
    //============= Calcul du Point de Vue =============
    //==================================================
      
    prec_time = cur_time;
    cur_time = glfwGetTime() - init_time;
    double delta_time = cur_time - prec_time;
    snprintf(title,100,"INFO4 - SI - TP3 - %2.0f FPS", 1.0/ delta_time);
    glfwSetWindowTitle( myWindow, title);

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
    glUniform1f(timeID, cur_time);

    glBindVertexArray(vaoID); // On active le VAO


    //==================================================
    // Todo : Appeller la fonction de dessin par indexation (a la place de celle ci-dessous)
    //==================================================

    // on dessine le contenu de tous les VBOs (buffers) associés à ce VAO
    // i.e: positions et couleurs
    //glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_TRIANGLES, m.faces.size(), GL_UNSIGNED_INT, 0);

    //== Decoupage de la fenetre en 4 ==
    int width, height;
    glfwGetWindowSize(myWindow, &width, &height);

    glViewport(0, 0, width / 2, height / 2);
    glDrawElements(GL_TRIANGLES, m.faces.size(), GL_UNSIGNED_INT, 0);
    glUniform1i(modeID, 0); //color

    glViewport(width / 2, 0, width / 2, height / 2);
    glDrawElements(GL_TRIANGLES, m.faces.size(), GL_UNSIGNED_INT, 0);
    glUniform1i(modeID, 1); //normale

    glViewport(0, height / 2, width / 2, height / 2);
    glDrawElements(GL_TRIANGLES, m.faces.size(), GL_UNSIGNED_INT, 0);
    glUniform1i(modeID, 2);//position

    glViewport(width / 2, height / 2, width / 2, height / 2);
    glDrawElements(GL_TRIANGLES, m.faces.size(), GL_UNSIGNED_INT, 0);
    glUniform1i(modeID, 3);//profondeur depuis la camera
    
    glBindVertexArray(0); // On désactive le VAO  

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



  //==================================================
  // Todo : Libérer TOUS les buffers que vous avez cree
  //==================================================

  glDeleteBuffers(1, &colorUnitBufferID);
  glDeleteBuffers(1, &indiceBufferID);
  glDeleteBuffers(1, &vertexMeshBufferID);
  glDeleteBuffers(1, &indiceMeshBufferID);
  glDeleteBuffers(1, &colorMeshBufferID);
  glDeleteBuffers(1, &normalMeshBufferID);


  cout << "Fin du programme..." << endl;
    
    
  return EXIT_SUCCESS;
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

