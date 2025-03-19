#version 410 core

//----------------------------------------------
// TESSELLATION EVALUATION SHADER
// C EST LE TROISIEME PROGRAMME EXECUTE PAR LA CARTE GRAPHIQUE
//----------------------------------------------

// LE PATCH ABSTRAIT EST UN QUAD. MAIS ON RECOIT QUAND MEME UN NOMBRE ARBITRAIRE DE POINTS DE CONTROLE
layout(quads, fractional_even_spacing) in;


// EN ENTREE ON DISPOSE D'UN TABLEAU CONTENANT LES XXX POINTS DE CONTROLE DU PATCH
// AINSI QUE LES DEUX PARAMETRES gl_TessCoord.x ET gl_TessCoord.y RETOURNES PAR LE TESSELLATION CONTROL SHADER
in gl_PerVertex {
        vec3 gl_Position;
    } gl_in[];

#define CP(i) gl_in[(i)].gl_Position

out float Ecart;

vec3 InterpolationBilineaire()
{
	vec3 P1 = mix( CP(0),  CP(1), gl_TessCoord.x);
	vec3 P2 = mix( CP(3),  CP(2), gl_TessCoord.x);
	vec3 P = mix( P1, P2, gl_TessCoord.y);

	return P;
}

// VARIABLES CONTENANT LES 3 PUISSANCES DU PARAMETRE gl_TessCoord.x
float T, T2, T3;

// PRECALCUL DES 4 PUISSANCES POUR LES 2 PARAMETRES DE TESSELLATION gl_TessCoord.x ET gl_TessCoord.y
void PuissanceDuParametre() 
{
        T = gl_TessCoord.x;
        T2 = T * T;
        T3 = T2 * T;
}       

// CALCUL UN POINT SUR UNE COURBE DE BEZIER CUBIQUE
vec3 BezierCubique( in float t, in float t2, in float t3, in vec3 CP0, in vec3 CP1, in vec3 CP2, in vec3 CP3)
{
        vec3 P;
        vec3 PT0, PT1, PT2, PT3;
        
        PT0 = CP0;
        PT1 = -3. * CP0 + 3. * CP1;
        PT2 = 3. * CP0 - 6. * CP1 + 3. * CP2;
        PT3 = -CP0 + 3. * CP1 - 3. * CP2 + CP3;
        
        P = PT0 + t * PT1 + t2 * PT2 + t3 * PT3;
        
        return P;
}

// CALCUL UN POINT SUR UNE COURBE DE BEZIER CUBIQUE AINSI QUE LA DERIVEE EN CE POINT
void BezierCubiqueEtDerivee( in float t, in float t2, in float t3, in vec3 CP0, in vec3 CP1, in vec3 CP2, in vec3 CP3, out vec3 P, out vec3 DP)
{
        vec3 PT0, PT1, PT2, PT3;
        
        PT0 = CP0;
        PT1 = -3. * CP0 + 3. * CP1;
        PT2 = 3. * CP0 - 6. * CP1 + 3. * CP2;
        PT3 = -CP0 + 3. * CP1 - 3. * CP2 + CP3;
        
        P = PT0 + t * PT1 + t2 * PT2 + t3 * PT3;
        DP = PT1 + 2 * t * PT2 + 3 * t2 * PT3;
        
}

#define EPAISSEUR_COURBE 1.0f

void main()
{
	vec3 P, DP;
//	gl_Position = vec4( gl_TessCoord.x, gl_TessCoord.y, 0., 1.); // POUR VISUALISER LES COORDONNEES DE TESSELLATION
//	gl_Position = vec4( InterpolationBilineaire(), 1.); // POUR VISUALISER L'INTERPOLATION BILINEAIRE DES 4 POINTS DE CONTROLE
	PuissanceDuParametre();
	BezierCubiqueEtDerivee( T, T2, T3, CP(0), CP(1), CP(2), CP(3), P, DP),
	DP = normalize(cross(DP, vec3(0.,0.,-1.)))/1.;
	gl_Position = vec4( P + EPAISSEUR_COURBE * (gl_TessCoord.y-0.5) * DP, 1.0); // POUR VOIR UN RUBAN AUTOUR DE LA COURBE
	Ecart = 1.5*(gl_TessCoord.y-0.5);

//	gl_Position = vec4( P, 1.0); // POUR NE VOIR QUE LA COURBE
//	Ecart = 0.;
}
