## Synthèse d’Image - Polytech Grenoble, INFO4, 2024-2025 

## TP2: Matrices de transformation 

## Organisation 

Vous déposerez sur Chamilo une archive nom_prenom.zip contenant: 

- Les codes commentés prêt à être compilé/executé et générant les sorties demandées. 
- Un rapport contenant vos résultats (réponses, images, commentaires,...) 

## Introduction
L’objectif du TP est de construire un outil de visualisation de modèle 3D simple permettant la navigation dans        une scène 3D. La base de code qui vous est fournie reprend les éléments de la dernière séance : création de fenêtre GLFW et initialisation de contexte OpenGL, affichage de géométrie.  Si ce n'est pas déjà fait, placez cette base de code dans votre dossier ```TP3D``` contenant les TPs.

- accédez au dossier des TPs: ```cd ~/TP3D/```
- placez vous dans le répertoire TP2: ```cd TP2```

- créez un dossier pour la compilation, accédez à ce dossier, lancez CMake, lancez la compilation
```
mkdir build
cd build
cmake ..
make
```

Vous pouvez alors exécuter le programme:

```
./si_info4
```

Note : La version d’OpenGL est maintenant la 3.3, et GLSL 150.

### Pipeline de Transformation
Comment déplacer, tourner, changer l’échelle d’un modèle 3D ? Comment créer une modifier le point de vue dans     la scene ? Ces éléments sont à la base de la création d’une scène virtuelle et même si leur utilisation est cachée par une interface utilisateur, il reste essentiel d’en connaître le fonctionnement.

#### Les coordonnées homogènes


C’est quoi ? Un point 3D est souvent codé par ses coordonnées cartésiennes (x, y, z). En informatique graphique, quand il s’agit d’utiliser OpenGL pour manipuler des points on utilise une quatrième coordonnées w. Les coordonnées (x, y, z, w) sont dites homogènes.
Pourquoi ?  Les coordonnées homogènes permettent d’implémenter rotations, translations, changement d’échelle   et projection sous forme de matrices. Elles ont deux avantages :
- Rapidité d’évaluation sur le GPU (les processeurs sont optimisés pour cela).
- formalisme unique pour représenter des transformations affines de point, des transformations linéaires de vecteurs, ou encore des projections centrales (perspective) de points.

Attention : Le produit matriciel n’est pas commutatif : $RT$ n'est pas égal à $TR$. Pour appliquer une translation $T$ puis une rotation $R$ on utilise le produit $RT$.

Utile :
- Si $w = 1$ alors $(x, y, z, 1)$ représente une position dans l’espace.
- Si $w = 0$ alors $(x, y, z, 0)$ représente un vecteur.
- Si $w \neq 0$ on peut projeter un point $p_h = (x, y, z, w)$ vers l’espace usuel en normalisant par w : $p_c = ( x/w ,  y/w ,  z/w , 1)$

#### D’un point 3D à l’écran

Le pipeline de transformation  s’applique sur chacun des sommets d’un maillage (en général dans le vertex shader)   et intervient avant la rasterisation. Il est composé de trois étapes: transformation de l'objet dans le monde (contenant tous les objets), transformation du monde dans le repère caméra, projection de la caméra vers l'écran.

*transformation des objets vers le monde*
La transformation d'un objet représente la position de cet objet dans  le  monde  contenant tous les objets:  elle  transforme  un  point  de l’espace objet vers l’espace monde. Elle est représentée par la matrice $M$ (comme modèle). Cette matrice objet est différente pour chaque objet de la scène.  Un modèle 3D est défini dans un système de coordonnées locales (l’espace objet)  dont l’origine (0, 0, 0) correspond souvent au centre de l’objet. Lorsqu’il est déformé, les données le décrivant ne changent pas : c’est la transformation de l’objet qui rend compte de cette déformation (qui a lieu dans l’espace monde). Par exemple, dans un jeu le déplacement d’un personnage ou d’un objet est défini par cette transformation.

*positionnement de la caméra*
Positionner la caméra consiste à transformer l'espace monde dans l'espace objet de la caméra.  Chaque point de l’espace monde  est transformé vers l’espace objet de  l’objet  caméra.   Cette opération est  représentée par la matrice $V$ (comme vue).   Il n’y a qu’une seule matrice caméra dans la scène.   C’est cette transformation qui      rend compte du changement de point de vue dans la scène 3D. Ainsi, lorsqu’on se déplace dans une scène 3D, les coordonnées des objets dans l’espace monde ne changent pas.

*projection*
La projection représente la prise de vue de la caméra : elle transforme un point de l’espace caméra vers l’espace fenêtre.  Elle est représentée par la matrice $P$ (comme projection).  Il n’y a qu’une seule matrice de projection dans   la scène. Cette transformation rend compte de la perspective induite par la caméra. Elle transforme la pyramide de vue tronquée représentant le champ de vision de la caméra (view frustum en jargon OpenGL)  en un cube unité.

En conclusion,   quand on multiplie le point $x$ par le produit matriciel $PVM$ , on fait passer celui-ci depuis   le repère objet (dans lequel il est décrit) au repère de la fenêtre (vers lequel il va être rendu). Cette décomposition en trois matrices permet de changer indépendemment et facilement tous les aspects de l’image à rendre : la position des objets, celle de la caméra, et les paramètres de prise de vue. À noter que conceptuellement, c’est toujours le monde qui est déformé pour rentrer dans le champ de vision de la caméra, qui elle reste fixe. Cette transformation est effectuée dans le vertex shader, c’est ce que nous allons voir maintenant.

### Travail à réaliser

Dans ce TP vous allez mettre en place et manipuler le pipeline de transformation complet: créer les matrices de modèle, vue et projection pour placer l’objet dans la scène virtuelle, et manipuler la matrice de vue pour se déplacer autour de l’objet.

*Coté GLSL*
Nous allons commencer par écrire le vertex shader nous permettant de procéder à la transformation $MVP$.
Ouvrez le vertex shader nommé ```vertex.glsl``` dans le dossier ```shader/```. Vous remarquerez que celui-ci crée une couleur pour le sommet, qui est ensuite transmise au fragment shader.
Pour récupérer les matrices dans le shader, ajoutez-y le code suivant au début :
```
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
```

Pour appliquer la transformation, il faut faire :
```
// Transformation PVM
gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(in_position, 1.0);
```
Il n’est pas nécessaire de modifier le fragment shader à ce stade là.

Note :    Contrairement au TP précédent, ce shader est en GLSL150.  Le mot clef attribute (pour les variables)      a été remplacé par les mots clefs in (pour les données d’entrée) et out (pour les données de sortie). Le mot clef uniform (pour les paramètres) reste le même. Les données d’entrée sont différentes pour chaque instance du shader (ie.  pour chaque sommet :  position, couleur, ...), tandis que les paramètres sont uniques.  Les données de sortie    sont transmises au shader suivant (depuis le vertex shader vers le fragment shader).

*Coté C++*
Nous devons maintenant créer les matrices dans ```main.cpp``` et les transmettre à notre shader.
- Matrice modèle : On ne va pas déplacer notre modèle dans ce TP. L’espace du modèle et l’espace du monde  sont confondus, la matrice modèle est donc la matrice identité.
- Matrice de vue : On va placer notre camera en (0, 0, 2) (position) et la pointer sur (0, 0, 0) (orientation).
- Matrice de projection : On va utiliser une projection en perspective avec un angle de 45°.

Création des matrices. Ce code permet de créer les matrices, il est à mettre dans l’initialisation du programme :

```	 
mat4 model_matrix = mat4(1.0);
mat4 view_matrix = lookAt(
    vec3( 0.f, 0.f, 2.f), // PLACE DE LA CAMERA
    vec3(0.f), // POINT QUE LA CAMERA REGARDE
    vec3(0.f, 1.f, 0.f)); // VERTICALE DE LA CAMERA
mat4 projection_matrix = perspective(
    45.0f, // ANGLE DE VUE
    WIDTH / HEIGHT, // RAPPORT D'ASPECT DE LA FENETRE
    .1f, // LIMITE PROCHE DE LA PYRAMIDE DE VUE
    100.f); //  LIMITE LOINTAINE DE LA PYRAMIDE DE VUE
// Recuperation des ID des matrices dans le shader program
GLuint MmatrixID = glGetUniformLocation( programID, "ModelMatrix");
GLuint VmatrixID = glGetUniformLocation( programID, "ViewMatrix");
GLuint PmatrixID = glGetUniformLocation( programID, "ProjectionMatrix");
```

Transmission des matrices. Ce code permet  de  transmettre  les  matrices  au  shader,  il  est  à  mettre  dans  la  boucle de rendu, après la mise en place du shader :
 	 
````
glUniformMatrix4fv( MmatrixID, 1, GL_FALSE, value_ptr(model_matrix));
glUniformMatrix4fv( VmatrixID, 1, GL_FALSE, value_ptr(view_matrix));
glUniformMatrix4fv( PmatrixID, 1, GL_FALSE, value_ptr(projection_matrix));
````
Vous devriez maintenant voir le carré initial légèrement rétreci sous l’effet de la perspective.
Pourquoi est-il nécessaire de spécifier une direction verticale ?

Changement de point de vue.    Pour déplacer la camera dans la scène, il suffit de modifier view_matrix dans       la boucle de rendu. Pour ce faire, glm propose des fonctions de manipulation matricielles. Étant donné un espace caméra défini par une matrice $m$, on a :

- rotate(mat4 m, float angle, vec3 axis) : tourne $m$ de angle (en degrés) autour de axis.
- scale(mat4 m, vec3 value) : change l’échelle de $m$ selon x, y, z en fonction des composantes de value.
- translate(mat4 m, vec3 displacement) : déplace m de displacement.

Ces fonctions retournent le résultat de la composition de la transformation définie par $m$ avec une transformation spécifique. Autrement dit, elles implémentent une multiplication matricielle à gauche.
### Travail à réaliser
Utilisez ces fonctions pour modifier l’initialisation de la matrice de vue.

*Animation de la caméra*
En effectuant un changement de point de vue infime à chaque passage dans la boucle       de rendu, on peut créer une animation.
### Travail à réaliser
Animez votre caméra en la faisant tourner autour de l’axe y.

*Navigation dans la  scene*
Nous allons maintenant coder une fonction nous permettant de controler le point de    vue sur a scene grâce au clavier. GLFW nous permet de connaitre l’état des touches du clavier (appuyées ou non). Ainsi, à chaque passage dans la boucle de rendu, nous pouvons interroger l’état des touches et modifier la matrice   de vue en conséquence.

``` 	  
if(glfwGetKey( aWindow, GLFW_KEY_UP ) == GLFW_PRESS) {
    // faire une action lorsque la touche haut est pressee
  }
```
Vous disposez d’un identifiant pour chaque touche du clavier. En voici quelques uns :
- ```GLFW_KEY_UP``` : touche "haut"
- ```GLFW_KEY_DOWN``` : touche "bas"
- ```GLFW_KEY_RIGHT``` : touche "droite"
- ```GLFW_KEY_LEFT``` : touche "gauche"
- ```GLFW_KEY_Z``` : touche "Z"
- ```GLFW_KEY_LEFT_SHIFT``` : touche "shift" de gauche.

####Travail à réaliser:
Dans la fonction ```view_control()```, implémentez de quoi tourner la caméra autour de l’origine.

*Note* :  Lorqu’on  compose  des  rotations,  on  effectue  successivement  des  multiplications  à  gauche  de  matrices. Il faut donc prendre en compte la rotation des axes induites par les rotations précédentes. La fonction ```inverse(mat4 m)``` permet d’obtenir l’inverse de $m$. Cette fonction intervient dans la définition de l’axe de rotation. Typiquement, pour tourner autour de l’axe x, il ne suffira pas d’utiliser ```vec3(1.0,0.0,0.0)``` pour la rotation, mais plutôt ```vec3(inverse(view_matrix)*vec4(1.0,0.0,0.0,0.0))```.