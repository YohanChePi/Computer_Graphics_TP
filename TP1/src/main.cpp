#include <iostream>
#include "shader.hpp" // Help to load shaders from files
// Include GLEW : Always include it before glfw.h et gl.h :)
#include <GL/glew.h> // OpenGL Extension Wrangler Library : http://glew.sourceforge.net/ 
#include <GLFW/glfw3.h> // Window, keyboard, mouse : http://www.glfw.org/
#include <glm/glm.hpp> // OpenGL Mathematics : http://glm.g-truc.net/0.9.5/index.html

using namespace glm;
using namespace std;

int main() {
  cout << "Debut du programme..." << endl;

  GLFWwindow* myWindow;

  // Initialisation de GLFW
  if(!glfwInit()) {
    cout << "Echec de l'initialisation de GLFW" << endl;
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_SAMPLES, 4); // Anti Aliasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2); // OpenGL 2.1
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Ouverture d'une fenêtre en 1024x768
  // et creation d'un contexte OpenGL
  if (!(myWindow = glfwCreateWindow( 1024, 768, "SI_INFO4", NULL, NULL))) {
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

  //===================== Initialisation =============================

  // Compilation du shader program et generation de l ’ID du Shader

  //GLuint programID = LoadShaders("../shader/vertex.glsl", "../shader/fragment.glsl");
  GLuint programID = LoadShaders("../shader/vertex_vao.glsl", "../shader/fragment.glsl");

  // Definition d’un vecteur
  vec3 v(-1.0f, -1.0f, 0.0f);

  // Definition d’un tableau de vecteurs ... à vous de le remplir !
  //vec3 vertex[3] = {vec3(-0.5f, -0.5f, 0.0f), vec3(0.5f, -0.5f, 0.0f), vec3(0.0f, 0.5f, 0.0f)}; //triangle equilateral
  //vec3 vertex[4] = {vec3(-0.5f, -0.5f, 0.0f), vec3(0.5f, -0.5f, 0.0f), vec3(-0.5f, 0.5f, 0.0f), vec3(0.5f, 0.5f, 0.0f)}; //carré
  vec3 vertex[12] = { //cube
      // Face avant
      vec3(0.5f, -0.5f, 0.0f),
      vec3(-0.5f, -0.5f, 0.0f),
      vec3(0.5f, 0.5f, 0.0f),
      vec3(-0.5f, 0.5f, 0.0f),
      // Partie haute
      vec3(-0.3f, 0.6f, 0.0f),
      vec3(-0.5f, 0.5f, 0.0f),
      vec3(0.7f, 0.6f, 0.0f),
      vec3(0.5f, 0.5f, 0.0f),
      // Partie droite
      vec3(0.5f, 0.5f, 0.0f),
      vec3(0.7f, 0.6f, 0.0f),
      vec3(0.5f, -0.5f, 0.0f),
      vec3(0.7f, -0.4f, 0.0f)};
  //vec3 vertex[5] = {vec3(-0.5f, -0.5f, 0.0f), vec3(0.5f, -0.5f, 0.0f), vec3(-0.5f, 0.5f, 0.0f), vec3(0.7f, 0.7f, 0.0f), vec3(0.0f, 0.9f, 0.0f)};

  // Obtention de l'ID de l'attribut "vertex_position" dans programID
  GLuint vertexPositionID = glGetAttribLocation(programID, "vertex_position");

  // Création d'un VAO et récupération de son ID
  GLuint vaoID;
  glGenVertexArrays(1, &vaoID);

  // Définition de notr VAO comme objet courant
  glBindVertexArray(vaoID);

  // Création d'un VBO et récupération de son ID
  GLuint vboID;
  glGenBuffers(1, &vboID);

  // Définition de notre VBO comme le buffer courant et liaison automatique de ce VBO au VAO actif (i.e. vaoID)
  glBindBuffer(GL_ARRAY_BUFFER, vboID);

  // Copie de données du CPU (dans vertex) vers le GPU (dans vboID)
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

  // On indique à OpenGL comment interpréter les données du vbo
  glEnableVertexAttribArray(vertexPositionID);
  glVertexAttribPointer(
      vertexPositionID, // ID de l'attribut à configurer
      3,                // nombre de composantes par position (x,y,z)
      GL_FLOAT,         // type des composantes
      GL_FALSE,         // normalisation des composantes
      0,                // décalage des composantes
      (void *)0         // offset des composantes
  );

  glBindVertexArray(0); // on désactive le VAO

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Pour dessiner uniquement les contours des triangles
  
  //==================================================================

  cout << "Initialisations..." << endl;

  // Definition de la couleur du fond
  glClearColor(0.0f,0.0f,0.3f,0.0f); // Fond bleu sombre (initialement)
  //glClearColor(0.0f,0.0f,0.0f,0.0f); // Fond noir

  cout << "Debut de la boucle principale..." << endl;
  unsigned int i = 0;
  
  // Boucle de dessin
  do {
    
    // Nettoyage de la zone de dessin
    glClear( GL_COLOR_BUFFER_BIT );

    //===================== Zone dessin =============================

    //**Dessin d'un triangle**//
    /*
    //Dessin en mode direct
      glUseProgram(programID); // On dit à OpenGL d’utiliser programID
      glBegin(GL_TRIANGLES);
      glColor3f(1.0f, 0.0f, 0.0f); // Rouge
      glVertex3f(-0.5f, -0.5f, 0.0f);
      glColor3f(0.0f, 1.0f, 0.0f); // Vert
      glVertex3f(0.5f, -0.5f, 0.0f);
      glColor3f(0.0f, 0.0f, 1.0f); // Bleu
      glVertex3f(0.0f, 0.5f, 0.0f);
      glEnd();
    */
    /*
    //Dessin en mode VAO
      glUseProgram(programID); // On dit à OpenGL d’utiliser programID
      glBindVertexArray(vaoID); // on active le VAO

      // on dessine le contenu de tous les VBOs associés à ce VAO
      glDrawArrays(GL_TRIANGLES, 0, 3);

      glBindVertexArray(0); // on désactive le VAO
    */
    //**Dessin d'un carré avec GL_TRIANGLES (2 triangles rectangles)*/
    /*
      glBindVertexArray(vaoID); // on active le VAO
      
      // on dessine le contenu de tous les VBOs associés à ce VAO
      glUseProgram(programID); // On dit à OpenGL d’utiliser programID
      glDrawArrays(GL_TRIANGLES, 0, 3);
      glUseProgram(0); // On dit à OpenGL d’utiliser programID
      glDrawArrays(GL_TRIANGLES, 1, 3);

      glBindVertexArray(0); // on désactive le VAO
    */

    //**Dessin d'un cube avec GL_TRIANGLES (6 triangles rectangles)*/
      glBindVertexArray(vaoID); // on active le VAO
      // Face avant
      glDrawArrays(GL_TRIANGLES, 0, 3);
      glDrawArrays(GL_TRIANGLES, 1, 3);
      // Partie haute
      glDrawArrays(GL_TRIANGLES, 4, 3);
      glDrawArrays(GL_TRIANGLES, 5, 3);
      // Partie droite
      glDrawArrays(GL_TRIANGLES, 8, 3);
      glDrawArrays(GL_TRIANGLES, 9, 3);
    
      glBindVertexArray(0); // on désactive le VAO

    //**Test des primitives OpenGL**//
      glBindVertexArray(vaoID); // on active le VAO

      // Points//
      glPointSize(10.0f);
      // glDrawArrays(GL_POINTS, 0, 4);

      // Lignes//
      glLineWidth(5.0f);
      // glDrawArrays(GL_LINES, 0, 4);

      // Bande//
      // glDrawArrays(GL_LINE_STRIP, 0, 4);

      // Boucle//
      // glDrawArrays(GL_LINE_LOOP, 0, 4);

      // Triangle en mode triangle strip//
      // glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);

      // Triangle en mode triangle fan//
      glDrawArrays(GL_TRIANGLE_FAN, 0, 5);
      
      glBindVertexArray(0); // on désactive le VAO

    //==================================================

    // Echange des zones de dessin buffers
    glfwSwapBuffers(myWindow);
	  
    cout << "Compteur : " << i++ << "\r";
    cout.flush();
    glfwWaitEvents();

  } // Execution de la boucle...
  while( glfwGetKey( myWindow, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&  // ... jusqu'a appui sur la touche ESC
	 (!glfwWindowShouldClose( myWindow))        );  // ... ou fermeture de la fenetre

  cout << endl;

  // Ferme GLFW et OpenGL
  glfwTerminate();

  //======================Nettoyer la memoire============================
  glDeleteBuffers(1, &vboID);
  glDeleteBuffers(1, &vaoID);
  //==================================================

  cout << "Fin du programme..." << endl;

  return EXIT_SUCCESS;
}

