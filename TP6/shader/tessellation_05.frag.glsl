#version 450 core

//----------------------------------------------
// FRAGMENT SHADER
// C EST LE CINQUIEME ET DERNIER PROGRAMME EXECUTE PAR LA CARTE GRAPHIQUE
//----------------------------------------------

// EN SORTIE ON CALCULE UNE COULEUR A 4 COORDONNEES 
layout(location = 0, index = 0) out vec4 fragColor;

in vec3 normaleSortieGeomShader; // la normale en sortie du geometry shader, qui a entre temps été interpolée par le rasterizer

void main(void)
{
	// Shading trivial: on attribue une couleur egale au produit scalaire de la normale par une direction lumineuse constante,
	// et on rajoute 0.4f pour simuler une couleur ambiante
	float shading = max(dot(normaleSortieGeomShader, vec3(1.,0.,0.)),0.) + 0.4f;

	fragColor = vec4( vec3(shading), 1);
}
