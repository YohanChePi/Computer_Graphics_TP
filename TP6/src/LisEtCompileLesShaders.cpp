#include <iostream>
#include <fstream>
#include <vector>

#include <GL/glew.h>

void LisEtCompileLeSourceDUnShader(const char *NomDuFichierSource, GLuint ShaderID)
{
	std::string CodeSource;
	GLint Resultat = GL_FALSE;
	int InfoLogLongueur;

	// LECTURE DU CODE SOURCE DU SHADER
	std::ifstream FichierSourceCode( NomDuFichierSource, std::ios::in);
	if(FichierSourceCode.is_open())
	{
		std::string Line = "";
		while(getline(FichierSourceCode, Line))
			CodeSource += "\n" + Line;
		FichierSourceCode.close();
	}
	else
	{
		std::cout << "Impossible d'ouvrir le fichier " << NomDuFichierSource << ". Etes-vous dans le bon repertoire ?" << std::endl;
	}	

	// COMPILATION DU SHADER
	std::cout << "Compilation du shader " << NomDuFichierSource << std::endl;

	char const * PointeurCodeSource = CodeSource.c_str();


	glShaderSource(ShaderID, 1, &PointeurCodeSource , NULL);
	glCompileShader(ShaderID);

	// VERIFICATION DE LA COMPILATION
	glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Resultat);
	glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLongueur);
	if ( InfoLogLongueur > 0 )
	{
		std::vector<char> MessageDErreur(InfoLogLongueur+1);
		glGetShaderInfoLog(ShaderID, InfoLogLongueur, NULL, &MessageDErreur[0]);
		std::cout << &MessageDErreur[0] << std::endl;
	}
}



GLuint LisEtCompileLesShaders(const char * NomFichierShaderSommet, const char *NomFichierShaderControl, const char *NomFichierShaderEvaluation, const char *NomFichierShaderGeometrie, const char * NomFichierShaderFragment){
	GLint Resultat = GL_FALSE;
	int InfoLogLongueur;

	// CREATION DES IDs DE SHADERS
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint TessControlShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
	GLuint TessEvalShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);
	GLuint GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// LECTURE ET COMPILATION DES SOURCES DES SHADERS
	if (NomFichierShaderSommet != NULL) { LisEtCompileLeSourceDUnShader( NomFichierShaderSommet, VertexShaderID);}
	if (NomFichierShaderControl != NULL) {LisEtCompileLeSourceDUnShader( NomFichierShaderControl, TessControlShaderID);}
	if (NomFichierShaderEvaluation != NULL) {LisEtCompileLeSourceDUnShader( NomFichierShaderEvaluation, TessEvalShaderID);}
	if (NomFichierShaderGeometrie != NULL) {LisEtCompileLeSourceDUnShader( NomFichierShaderGeometrie, GeometryShaderID);}
	if (NomFichierShaderFragment != NULL) {LisEtCompileLeSourceDUnShader( NomFichierShaderFragment, FragmentShaderID);}

	// EDITION DE LIEN
	std::cout << "Edition de lien du programme GLSL..." << std::endl;
	GLuint ProgrammeID = glCreateProgram();
	if (NomFichierShaderSommet != NULL) { glAttachShader(ProgrammeID, VertexShaderID);}
	if (NomFichierShaderControl != NULL) {glAttachShader(ProgrammeID, TessControlShaderID);}
	if (NomFichierShaderEvaluation != NULL) {glAttachShader(ProgrammeID, TessEvalShaderID);}
	if (NomFichierShaderGeometrie != NULL) {glAttachShader(ProgrammeID, GeometryShaderID);}
	if (NomFichierShaderFragment != NULL) {glAttachShader(ProgrammeID, FragmentShaderID);}
	glLinkProgram(ProgrammeID);

	// VERIFICATION DE L EDITION DE LIEN
	glGetProgramiv(ProgrammeID, GL_LINK_STATUS,  &Resultat);
	glGetProgramiv(ProgrammeID, GL_INFO_LOG_LENGTH, &InfoLogLongueur);
	if ( InfoLogLongueur > 0 )
	{
		std::vector<char> MessageDErreur(InfoLogLongueur+1);
		glGetProgramInfoLog(ProgrammeID, InfoLogLongueur, NULL, &MessageDErreur[0]);
		std::cout << &MessageDErreur[0] << std::endl;
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(TessControlShaderID);
	glDeleteShader(TessEvalShaderID);
	glDeleteShader(GeometryShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgrammeID;
}
