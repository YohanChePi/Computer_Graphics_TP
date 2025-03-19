#include <iostream> // pour utiliser std::cout std::endl << >>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include "main.h"
#include "LisEtCompileLesShaders.h"


#define TAILLE_HORIZONTALE 800
#define TAILLE_VERTICALE 450

const int nPoint = 30;
const int degre = 3;

//----------------------------------------
// CLES SUR LA CARTE GRAPHIQUE LIEES AU PROGRAMME GPU DE TESSELLATION

// CLE DU PROGRAMME GPU
static GLuint MonProgrammeGLSL;

// CLE DE LA VARIABLE SOMMET DANS LE PROGRAMME GPU
static GLint MonSommetID;

// CLE DE LA FINESSE DE DECOUPAGE DANS LE PROGRAMME GPU
static GLint FinesseDecoupageID;

// CLE DE LA MATRICE DE TRANSFORMATION DANS LE PROGRAMME GPU
static GLint MatriceCompleteID;

// CLE DE LA MATRICE DE VUE DANS LE PROGRAMME GPU
static GLint MatriceVueID;

//----------------------------------------
// CLES DE LA GEOMETRIE ET DES INDICES DES SOMMETS DES PATCHS SUR LA CARTE GRAPHIQUE
static GLuint MonVAO;
static GLuint MesSommetsID;
static GLuint MesIndicesID;

void view_control( GLFWwindow* myWindow, glm::mat4& view_matrix, float dx);

void InitialisationDuProgrammeGPUEtDesLiensVersCeProgramme()
{
	MonProgrammeGLSL = LisEtCompileLesShaders( 
		"../shader/tessellation_01.vert.glsl",
		"../shader/tessellation_02.cont.glsl",
		"../shader/tessellation_03.eval.glsl",
		"../shader/tessellation_04.geom.glsl",
		"../shader/tessellation_05.frag.glsl");

// JE RECHERCHE LES CLES DESIGNANT LES VARIABLES UNIFORMES QUE J UTILISE DANS LES CODES GLSL
// CES VARIABLES SONT DITES UNIFORMES CAR ELLES ONT LA MEME VALEUR POUR TOUS LES SOMMETS/PRIMITIVES/FRAGMENTS.
// LA CHAINE DE CARACTERE A DROITE CONTIENT LE NOM DES VARIABLES DANS LES CODES GLSL
// LE RESULTAT DE LA FONCTION EST LA CLE VERS CETTE VARIABLE GLSL
// CETTE CLE EST UTILISEE PLUS TARD DANS LE CODE C++ POUR TRANSMETTRE UNE VALEUR DU CPU VERS LA VARIABLE GPU
	FinesseDecoupageID = glGetUniformLocation(MonProgrammeGLSL, "u_FinesseDecoupage");
	MonSommetID = glGetAttribLocation(MonProgrammeGLSL, "MonSommetEnEntree");
	MatriceCompleteID = glGetUniformLocation( MonProgrammeGLSL, "MaMatriceComplete");
	MatriceVueID = glGetUniformLocation( MonProgrammeGLSL, "MaMatriceDeVue");
	fprintf( stderr, "MonSommetID = %d\n", MonSommetID);
	fprintf( stderr, "FinesseDecoupageID = %d\n", FinesseDecoupageID);
	fprintf( stderr, "MatriceCompleteID = %d\n", MatriceCompleteID);
	fprintf( stderr, "MatriceVueID = %d\n", MatriceVueID);
}

void InitialisationDesDonnees()
{
	float radius, angle, height;
	const float nTurn = 5.0f;

	glm::vec3 MesSommets[ nPoint];
	GLuint MesIndices[ nPoint];

	// CALCUL DE SOMMETS SUR UNE SPIRALE	
	for (int i=0; i<nPoint; i++)
	{
		float t = (1.0f * i) / (nPoint-1);
		radius = t;
		angle = nTurn * 2. * M_PI * t;
		height = t;
		MesSommets[i] = glm::vec3( radius * cos( angle), radius * sin( angle), height);
	}
	//afficher MesSommets
	// TODO. MODIFIER LES SOMMETS MesSommets[i] POUR LES INDICES i CORRESPONDANT AUX RACCORDS ENTRE LES COURBES DE BEZIER
	// AFIN QUE LES COURBES DE BEZIER SE RACCORDENT AVEC CONTINUITE C1

	// Les raccords des courbes de Bezier sont entre les points i multiples de 3 et i multiples de 4
	// etant donne que les courbes de Bezier sont de degre 3
	// Il faut donc modifier ces points pour que les courbes se raccordent avec continuite C1
	/*
	for (int i = 1; i < nPoint - 1; i++) {
		if (i % 3 == 0) {
			MesSommets[i] = 0.5f * (MesSommets[i - 1] + MesSommets[i + 1]);
		}
	}
	*/
	

	
	MesIndices[0] = 0;
	for(int i=1; i<nPoint; i++)
	{
		MesIndices[i] = MesIndices[i-1] + (i % 4 != 0);
		// MesIndices[i] = i % 4;
		std::cout << MesIndices[i] << " ";
	}

	glGenBuffers( 1, &MesSommetsID); // BUFFER DES SOMMETS
	glBindBuffer(GL_ARRAY_BUFFER, MesSommetsID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MesSommets), (void*)MesSommets, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers( 1, &MesIndicesID); // BUFFER DES INDICES
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MesIndicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MesIndices), (void*)MesIndices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EncapsuleLesDonneesDansUnVao()
{
	glGenVertexArrays( 1, &MonVAO); // ON GENERE UN VAO
	glBindVertexArray(MonVAO);

	glEnableVertexAttribArray(0); // ON ACTIVE LE TABLEAU D'ATTRIBUT D'INDICE 0

        glBindBuffer(GL_ARRAY_BUFFER, MesSommetsID); // ON ACTIVE LE VBO DES SOMMETS
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MesIndicesID); // ON ACTIVE LE VBO DES INDICES

	// ON INDIQUE COMMENT LES DONNEES SONT TRANSMISES AU SHADER
	glVertexAttribPointer(
            0,                  // indice de l'attribut à configurer, correspond à la ligne "layout(location=0) in vec3 MonSommetEnEntree;" dans le vertex shader
            3,                  // nombre de composante par position. ici 3 composantes: (x, y, z).
            GL_FLOAT,           // type des composantes
            GL_FALSE,           // normalisation des composantes
            0,                  // decalage des composantes
            (void*)0            // offset des composantes
        );

	glBindVertexArray(0); // ON DESACTIVE LE VAO
}

int main( int argc, char *argv[])
{
	// fprintf( stderr, "GL_MAX_PATCH_VERTICES = %d\n", GL_MAX_PATCH_VERTICES);
	// fprintf( stderr, "GL_MAX_TESS_GEN_LEVEL = %d\n", GL_MAX_TESS_GEN_LEVEL);

	GLFWwindow *MaFenetre = InitialiseUnContexteEtUneFenetreOpenGL();

	InitialisationDuProgrammeGPUEtDesLiensVersCeProgramme();
	glUseProgram(MonProgrammeGLSL);

	InitialisationDesDonnees();
	EncapsuleLesDonneesDansUnVao();

	std::cout << "MonProgrammeGLSL = " << MonProgrammeGLSL << std::endl;

	std::cout<<"MonVAO = "<<MonVAO<< std::endl;

	glClearColor( 0.0, 0.0, 0.0, 1.0f);

	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);

// INDIQUE LE NOMBRE DE POINTS A TRAITER PAR PATCH TESSELLE
// ICI ON A BESOIN DE 4 POINTS DE CONTROLE POUR TESSELER UN TUBE
// AUTOUR D'UNE COURBE DE BEZIER DE DEGRE 3

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	double init_time = glfwGetTime();
	double prec_time = init_time;
	double cur_time = init_time;
	double speed = 5.0;
	char TitreDeLaFenetre[100];

	// MATRICE DE LA VUE CAMERA
	glm::vec4 positionDeLaCamera = glm::vec4(0.0f, 0.f, 40.f, 1.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(positionDeLaCamera),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

	while (!glfwWindowShouldClose( MaFenetre)) {

		TraiteLesEvenementsClavier( MaFenetre);

		//==================================================
		//============= Calcul du Point de Vue =============
		//==================================================
		
		prec_time = cur_time;
		cur_time = glfwGetTime() - init_time;
		double delta_time = cur_time - prec_time;
		snprintf(TitreDeLaFenetre,100,"SI_INFO4 - %2.0f FPS",1.0/ delta_time);
		glfwSetWindowTitle( MaFenetre, TitreDeLaFenetre);

		view_control( MaFenetre, View, speed * delta_time);
		float MonAngle = glfwGetTime()*50.;

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// MATRICE DE PROJECTION CENTRALE
		glm::mat4 Projection = glm::perspective(glm::radians(45.0f), TAILLE_HORIZONTALE*1.0f / TAILLE_VERTICALE, 0.1f, 1000.f);

		// MATRICE POUR PLACER NOTRE OBJET DANS LE MONDE
		glm::mat4 Model = glm::scale( glm::mat4(1.0f), glm::vec3(10.0f))
				 * glm::translate( glm::mat4(1.0f), glm::vec3( 0.0f, 0.0f, 0.0f));
	
		// COMBINAISON DES TROIS MATRICES DANS LE BON SENS:
		// ON COMMENCE PAR PLACER L OBJET DANS LE MONDE
		// PUIS LE MONDE DANS LE REPERE CAMERA
		// ET ENFIN ON APPLIQUE LA PROJECTION CENTRALE
		glm::mat4 MaMatriceComplete = Projection * View * Model; 
		glUniformMatrix4fv( MatriceCompleteID, 1, GL_FALSE, glm::value_ptr(MaMatriceComplete));
		glm::mat4 MaMatriceVue = View * Model;
		glUniformMatrix4fv( MatriceVueID, 1, GL_FALSE, glm::value_ptr(MaMatriceVue));

		glBindVertexArray( MonVAO);
		glDrawElements( GL_PATCHES, nPoint, GL_UNSIGNED_INT, NULL); // ICI APPEL DES SHADERS GLSL
		glBindVertexArray( 0);

		glfwSwapBuffers(MaFenetre);

		glfwPollEvents();
	}

	glDeleteBuffers(1, &MesIndicesID);
	glDeleteBuffers(1, &MesSommetsID);
	glDeleteVertexArrays(1, &MonVAO);

	glfwTerminate();
}


GLFWwindow* InitialiseUnContexteEtUneFenetreOpenGL()
{
	GLFWwindow* MaFenetre;

	// INITIALISATION DE GLFW
	if( !glfwInit() )
	{
		std::cout << "Echec de l'initialisation de GLFW" << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // Anti Aliasing
	// glfwWindowHint(GLFW_SAMPLES, 0); // pas d'Anti Aliasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// Ouverture d'une fenêtre en 1024x768
	// et creation d'un contexte OpenGL
	if( !(MaFenetre = glfwCreateWindow(TAILLE_HORIZONTALE, TAILLE_VERTICALE, "Polytech INFO4", NULL,NULL)) )
	{
		std::cout << "Echec de l'ouverture de fenetre OpenGL" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(MaFenetre);

	// Autorise GLFW a recevoir les appuis de touche
	glfwSetInputMode( MaFenetre, GLFW_STICKY_KEYS, GL_TRUE);

	// Initialisation de  GLEW
	glewExperimental = GL_TRUE; 
	if (glewInit() != GLEW_OK) {
		std::cout << "Echec de l'initialisation de GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Affichage des donnees du contexte OpenGL
	const GLubyte* renderer = glGetString (GL_RENDERER);
	std::cout << "TYPE DE CARTE GRAPHIQUE : " << renderer << std::endl;

	const GLubyte* version = glGetString (GL_VERSION);
	std::cout << "VERSION DE PILOTE OpenGL : " << version << std::endl;

	const GLubyte* shading_version = glGetString (GL_SHADING_LANGUAGE_VERSION);
	std::cout << "VERSION GLSL: " << shading_version << std::endl;

	return MaFenetre;
}

void TraiteLesEvenementsClavier(GLFWwindow *MaFenetre)
{
	if (glfwGetKey( MaFenetre, GLFW_KEY_ESCAPE ) == GLFW_PRESS) {
		std::cout << "Touche ESCAPE\n";
		glDeleteBuffers(1, &MesIndicesID);
		glDeleteBuffers(1, &MesSommetsID);
		glDeleteVertexArrays(1, &MonVAO);
		glfwTerminate();
		exit(1);
	}
}


void view_control(   GLFWwindow* myWindow, glm::mat4& view_matrix, float dx) {
	float relativeSpeedZoom = 3.0f;
	if (glfwGetKey( myWindow, GLFW_KEY_UP ) == GLFW_PRESS) {
	  glm::vec4 axis = glm::vec4(1.0, 0.0, 0.0, 0.0);
	  axis = glm::inverse(view_matrix) * axis;
	  view_matrix = glm::rotate(view_matrix, -dx, glm::vec3(axis));
	}
  
	if (glfwGetKey( myWindow, GLFW_KEY_DOWN ) == GLFW_PRESS) {
	  glm::vec4 axis = glm::vec4(1.0, 0.0, 0.0, 0.0);
	  axis = inverse(view_matrix) * axis;
	  view_matrix = rotate(view_matrix, +dx, glm::vec3(axis));
	}
  
	if (glfwGetKey( myWindow, GLFW_KEY_RIGHT ) == GLFW_PRESS) {
	  glm::vec4 axis = glm::vec4(0.0, 1.0, 0.0, 0.0);
	  axis = inverse(view_matrix) * axis;
	  view_matrix = glm::rotate(view_matrix, dx, glm::vec3(axis));
	}
  
	if (glfwGetKey( myWindow, GLFW_KEY_LEFT ) == GLFW_PRESS) {
	  glm::vec4 axis = glm::vec4(0.0, 1.0, 0.0, 0.0);
	  axis = inverse(view_matrix) * axis;
	  view_matrix = glm::rotate(view_matrix, -dx, glm::vec3(axis));
	}
  
	if (glfwGetKey( myWindow, GLFW_KEY_PAGE_UP ) == GLFW_PRESS) {
	  glm::vec4 axis = glm::vec4(0.0, 0.0, 1.0, 0.0);
	  axis = inverse(view_matrix) * axis;
	  view_matrix = glm::rotate(view_matrix, -dx, glm::vec3(axis));
	}
  
	if (glfwGetKey( myWindow, GLFW_KEY_PAGE_DOWN ) == GLFW_PRESS) {
	  glm::vec4 axis = glm::vec4(0.0, 0.0, 1.0, 0.0);
	  axis = inverse(view_matrix) * axis;
	  view_matrix = glm::rotate(view_matrix, +dx, glm::vec3(axis));
	}
	
	if (glfwGetKey( myWindow, GLFW_KEY_Z ) == GLFW_PRESS) {
	  glm::vec4 axis = glm::vec4(0.0, 0.0, 1.0, 0.0) * relativeSpeedZoom * dx;
	  axis = inverse(view_matrix) * axis;
	  view_matrix = glm::translate(view_matrix, glm::vec3(axis));
	}
  
	if (glfwGetKey( myWindow, GLFW_KEY_S ) == GLFW_PRESS) {
	  glm::vec4 axis = glm::vec4(0.0, 0.0, 1.0, 0.0) * relativeSpeedZoom * (-dx);
	  axis = inverse(view_matrix) * axis;
	  view_matrix = glm::translate(view_matrix, glm::vec3(axis));
	}
  
	if (glfwGetKey( myWindow, GLFW_KEY_Q) == GLFW_PRESS) {
	  glm::vec4 axis = glm::vec4(-1.0, 0.0, 0.0, 0.0) * dx;
	  axis = inverse(view_matrix) * axis;
	  view_matrix = glm::translate(view_matrix, glm::vec3(axis));
	}
  
	if (glfwGetKey( myWindow, GLFW_KEY_D ) == GLFW_PRESS) {
	  glm::vec4 axis = glm::vec4(-1.0, 0.0, 0.0, 0.0) * (-dx);
	  axis = inverse(view_matrix) * axis;
	  view_matrix = glm::translate(view_matrix, glm::vec3(axis));
	}
  
	if (glfwGetKey( myWindow, GLFW_KEY_A ) == GLFW_PRESS) {
	  glm::vec4 axis = glm::vec4(0.0, 1.0, 0.0, 0.0) * dx;
	  axis = inverse(view_matrix) * axis;
	  view_matrix = glm::translate(view_matrix, glm::vec3(axis));
	}
  
	if (glfwGetKey( myWindow, GLFW_KEY_E ) == GLFW_PRESS) {
	  glm::vec4 axis = glm::vec4(0.0, 1.0, 0.0, 0.0) * (-dx);
	  axis = glm::inverse(view_matrix) * axis;
	  view_matrix = glm::translate(view_matrix, glm::vec3(axis));
	}
  }


