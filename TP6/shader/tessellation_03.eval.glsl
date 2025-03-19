#version 450 core

//----------------------------------------------
// TESSELLATION EVALUATION SHADER
// C EST LE TROISIEME PROGRAMME EXECUTE PAR LA CARTE GRAPHIQUE
//----------------------------------------------

// DANS CETTE APPLICATION LE PATCH ABSTRAIT EST UN QUAD
// fractional_even_spacing PERMET DE SUBDIVISER PROGRESSIVEMENT
// [https://www.khronos.org/opengl/wiki/Tessellation]
layout(quads, fractional_even_spacing) in;

// EN ENTREE ON DISPOSE D'UN TABLEAU CONTENANT LES XXX POINTS DE CONTROLE DU PATCH 
// LE NOMBRE EST DEFINI DANS LE PROGRAMME C++ PAR LA LIGNE glPatchParameteri(GL_PATCH_VERTICES, 4);
// AINSI QUE LES DEUX PARAMETRES gl_TessCoord.x ET gl_TessCoord.y RETOURNES PAR LE TESSELLATION CONTROL SHADER

out vec3 normaleTube;

// x ET y SONT LES COORDONNEES DE TESSELLATION ENTRE 0 ET 1
// b[0], b[1], b[2], b[3] SONT LES POINTS DE CONTROLE D'UNE COURBE DE BEZIER
void pointSurLeTube( float x, float y, vec3 b[4])
{
	// CALCUL DE i ENTRE 0 ET 2
	// ET s ENTRE 0. ET 1.
	// TELS QUE 3 x = i + t
	// i ET s SONT UTILISES POUR CALCULER pointSurLaCourbe
	// QUI VA PARCOURIR LE POLYGONE b[0], b[1], b[2], b[3]
	// QUAND x VARIE ENTRE 0 ET 1
	int i = int( min( floor( x * 3.), 2.));
	float s = x * 3. - i;
	//vec3 pointSurLaCourbe = mix( b[i], b[i+1], s);
	//pointSurLaCourbe (Algo de Casteljau)
	vec3 b10 = mix( b[0], b[1], x);
	vec3 b11 = mix( b[1], b[2], x);
	vec3 b12 = mix( b[2], b[3], x);
	vec3 b20 = mix( b10, b11, x);
	vec3 b21 = mix( b11, b12, x);
	vec3 pointSurLaCourbe = mix( b20, b21, x);


	// LA TANGENTE AU POLYGONE EST SIMPLEMENT LE VECTEUR PORTANT L'ARETE
	// SUR LEQUEL LE POINT pointSurLaCourbe SE TROUVE
	//vec3 tangenteCourbe = normalize(b[i+1] - b[i]);
	vec3 tangenteCourbe = normalize(b21 - b20);
	vec3 normaleCourbe = cross(tangenteCourbe, vec3(0., 0., 1.));
	vec3 binormale = cross(tangenteCourbe, normaleCourbe);

	float rayonTube = 0.05f;
	float angle = 2. * 3.14159265359 * y;

// LES 2 LIGNES SUIVANTES CALCULENT LES VARIABLES DE SORTIE DE CE SHADER
	normaleTube = normaleCourbe * cos(angle) + binormale * sin(angle);
	gl_Position = vec4( pointSurLaCourbe + rayonTube * normaleTube, 1.0f);
}

void main()
{
	vec3 b[4];
	for (int i=0; i<4; i++)
	{
		b[i] = gl_in[i].gl_Position.xyz;
	}
	pointSurLeTube( gl_TessCoord.x, gl_TessCoord.y, b);
}
