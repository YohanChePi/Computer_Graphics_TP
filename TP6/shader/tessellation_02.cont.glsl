#version 450 core

//------------------------------------------------
// TESSELLATION CONTROL SHADER (TCS)
// C EST LE DEUXIEME PROGRAMME EXECUTE SUR LA CARTE GRAPHIQUE
// SON ROLE EST DE TRIANGULER LES PATCHS DE TESSELLATION
// ET DE CALCULER LES COORDONNEES DE TESSELLATION
// DE CHAQUE SOMMET DE CES TRIANGLES
//------------------------------------------------

// CE TCS RESSORT LES 4 SOMMETS D UN PATCH QUAD DE TESSELLATION
// AINSI QU'IMPLICITEMENT LES DEUX COORDONNEES DE TESSELLATION
layout(vertices = 4) out;

// ON TRANSMETS LES POSITIONS DES SOMMETS DU PATCH
// AU GEOMETRY SHADER

	
void main(void)
{
	gl_TessLevelOuter[0] = 35.0f;
	gl_TessLevelOuter[1] = 35.0f;
	gl_TessLevelOuter[2] = 35.0f;
	gl_TessLevelOuter[3] = 35.0f;

	gl_TessLevelInner[0] = 35.0f;
	gl_TessLevelInner[1] = 35.0f;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
