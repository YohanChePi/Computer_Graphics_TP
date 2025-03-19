#version 450 core

//----------------------------------------------
// GEOMETRY SHADER
// C EST LE QUATRIEME PROGRAMME EXECUTE PAR LA CARTE GRAPHIQUE
//----------------------------------------------

// IL "AVALE" EN ENTREE LES TRIANGLES DONT LA CONNECTIVITE A ETE CALCULEE PAR LE TESSELLATION CONTROL SHADER 
// ET DONT LA GEOMETRIE A ETE CALCULE PAR LE TESSELLATION EVALUATION SHADER
layout(triangles) in;

in vec3 normaleTube[];

// EN SORTIE ON IMPLEMENTE DEUX POSSIBILITES: SOIT ON NE SORT QUE LES BORDS DES TRIANGLES
// SOIT ON REMPLI LES TRIANGLES

// POUR DESSINER JUSTE LES ARETES CHOISIR line_strip
layout(line_strip, max_vertices = 4) out;

// POUR REMPLIR LES FACES CHOISIR triangle_strip
//layout(triangle_strip, max_vertices = 3) out;

uniform mat4 MaMatriceComplete;

out vec3 normaleSortieGeomShader;

void main(void)
{
// ON DOIT AVOIR gl_in.length() = 3 CAR LE TESSELLATEUR CALCULE DES TRIANGLES 
	for(int i = 0; i < gl_in.length(); ++i)
	{
		gl_Position = MaMatriceComplete * gl_in[i].gl_Position; // APPLIQUE LA TRANSFORMATION UNIQUEMENT DANS LE GEOMETRY SHADER
		normaleSortieGeomShader = normaleTube[i];
		// gl_Position = MaMatriceComplete * myNameOfPerVertexInput[i].position; // APPLIQUE LA TRANSFORMATION UNIQUEMENT DANS LE GEOMETRY SHADER

// ON RAJOUTE UN i-eme SOMMET A LA PRIMITIVE
		EmitVertex();
	}

// AU CAS OU ON VEUT DESSINER LES CONTOURS DU TRIANGLE, IL FAUT RESSORTIR LE POINT D'ORIGINE POUR BOUCLER LE CONTOUR
// SI ON GENERE QUAND MEME UN 4IEME POINT POUR REMPLIR LE TRIANGLE, CELA NE POSE PAS DE PROBLEME AU RASTERIZER QUI
// IGNORERA LE 4IEME POINT.
	gl_Position = MaMatriceComplete *  gl_in[0].gl_Position; // APPLIQUE LA TRANSFORMATION UNIQUEMENT DANS LE GEOMETRY SHADER 
	normaleSortieGeomShader = normaleTube[0];
	// gl_Position = MaMatriceComplete *  myNameOfPerVertexInput[0].position; // APPLIQUE LA TRANSFORMATION UNIQUEMENT DANS LE GEOMETRY SHADER 

// ON RAJOUTE UN SOMMET A LA PRIMITIVE
	EmitVertex();
	
// ON TERMINE LA PRIMITIVE (SOIT line_strip SOIT triangle_strip SUIVANT LE layout CHOISI AU DEBUT DU PROGRAMME
	EndPrimitive();
}
