## Synthèse d’Image - Polytech Grenoble, INFO4, 2024-2025 

## TP3: VBO, Maillages et indexation 


## Introduction
L’objectif du TP est de s’initier à la génération et à l’importation de modèle 3D.
La base de code qui vous est fournie reprend les éléments de la dernière séance : création de fenêtre GLFW et initialisation de contexte OpenGL, affichage de géométrie, et utilisation des matrices de transformation pour manipuler la scène. Si ce n'est pas déjà fait, placez cette base de code dans votre dossier ```TP3D``` contenant les TPs.

- accédez au dossier des TPs: ```cd ~/TP3D/```
- vérifiez que le répertoire `models`se trouve bien dans `TP3D`, dans le cas contraire reportez vous au sujet du TP1 pour récupérer ce répertoire contenant des modèles 3D.
- placez vous dans le répertoire TP3: ```cd TP3```
- installer un lien vers le répertoire `models` situé dans le repertoire parent:

```
ln -sf ../models .
```

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
## Buffer de couleur

Pour rappel, un buffer ("tampon" en français) est une zone en mémoire sur la carte graphique qui contient des données utiles pour l’affichage de l’image. Pour l’instant, nous avons seulement utilisé un buffer pour stocker les positions des sommets de nos objets. Nous allons maintenant voir comment créer des buffers pour gérer d’autres types de données : couleurs et indices.

Comme pour le tableau de position, nous allons utiliser les ```std::vector``` pour représenter nos données sur le CPU. Cette structure a l’avantage de stocker l’information de manière contigue en mémoire. C'est essentiel pour transporter les données du CPU vers le GPU d'un bloc.  Pour  ajouter  un  élément à un ```std::vector```, utilisez la fonction ```push_back()```. Pour connaître le nombre d’éléments déjà stockés, utilisez ```size()```. La base de code vous fournit des exemples d'utilisation de ces fonctions pour créer le tableau de positions ```vector<vec3> vertices``` et le transférer sur le GPU.

Créez un tableau de type ```vector<vec3>``` stockant une couleur par sommet du cube,  que  vous  nommerez  ```colors```. Chaque couleur est représentée par ses coordonnées rouge, vert et bleu, entre 0 et 1. Il  doit  y  avoir autant de couleurs que de sommets. Choisissez les couleurs que vous voulez pour chacun des sommets de chacune des faces. 

Ensuite, il faut créer un buffer à partir de ```colors```. Pour ceci, il suffit de faire exactement pareil que pour les positions des sommets :

```
  // Creation d'un buffer des couleurs
  GLuint colorBufferID;
  glGenBuffers( 1, &colorBufferID);

  // Definition de vertexBufferID comme le buffer courant
  glBindBuffer( GL_ARRAY_BUFFER, colorBufferID);

  // Copie des couleurs sur la carte graphique
  glBufferData( GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), colors.data(), GL_STATIC_DRAW);

  // Obtention de l'ID l'attribut "in_color" dans programID
  GLuint vertexColorID = glGetAttribLocation( programID, "in_color");

  // on indique a OpenGL comment lire les donnees et on autorise leur lecture
  glVertexAttribPointer( vertexColorID, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
  glEnableVertexAttribArray( vertexColorID);
```

Notez que ce VBO est automatiquement associé au VAO  actif (vaoID).  Il faut modifier le vertex shader pour que celui-ci réceptionne ce nouvel attribut :

````
// Version d'OpenGL
#version 330

// Donnees d'entree
layout(location=0) in vec3 in_position;
layout(location=1) in vec3 in_color;

// Donnees de sortie
out vec4 my_color;

// Parametres
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

// Fonction appellee pour chaque sommet
void main()
{
  // Affectation de la position du sommet
  // gl_Position est definit par defaut dan GLSL
  gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(in_position, 1.0);

  my_color = vec4( in_color, 1.0);
}
````

###### Travail à réaliser:

Vérifiez que votre code est fonctionnel: vous devez maintenant voir apparaitre le cube avec les couleurs que vous avez attribuées dans le programme CPU.

## Buffer d'indices

Les sommets que nous avons utilisés pour créer le cube sont redondants. Un cube est en effet défini par 8 sommets, pourtant dans la base de code ces 8 sommets apparaissent 3 ou 4 fois dans le tableau ```vertices```. Pourquoi a-t-on besoin de cette redondance?

Pour éviter cela, nous allons :
- modifier les tableaux de sommets et de couleurs pour qu'ils représentent de manière unique les huits sommets du cube et leur couleur. Chaque sommet est ainsi désigné de manière unique par son *indice* dans le tableau des sommets, un entier de 0 jusqu'à 7.
- créer un tableau d’entier nommé ``indices`` qui contiendra la *topologie* du maillage. Chaque triplet d'indices consécutifs dans ce tableau représentera les 3 sommets d'un triangle. Comme un cube comporte 6 faces et que chacune de ces faces est représentée par 2 triangles, cela fait un total de 12 triangles pour le cube et donc ``12x3=36`` indices.

Voici un exemple de code qui crée un tableau de 6 entiers permettant de dessiner 2 triangles:

```
// Tableau des indices
uint desIndices[] = {
0, 3, 1, 3, 0, 2 // 6 SOMMETS POUR 2 TRIANGLES
};
vector<uint> indices;
for (int i=0; i<6; i++) {
indices.push_back( desIndices[i]);
}
```

Vous devrez déterminer vous-même les 36 indices. Attention: pour chaque triangle, les indices de ses sommets doivent être donnés dans l'ordre trigonométrique lorsque vous observez ce triangle depuis l'extérieur de l'objet. Si vous vous trompez d'orientation, la face ne sera pas rendue, elle apparaitra noire.  Pour ne pas vous tromper, faites sur papier le dessin d'un cube en indiquant les indices que vous aurez choisi pour les 8 sommets. Ensuite pour chaque face (haut, bas, gauche, droite, devant, derriere), faite un dessin de cette face vue de l'extérieur du cube, en reportant précisément les indices des sommets.

L’envoi du tableau des indices au GPU est très similaire à celui des sommets ou des couleurs, excepté le fait qu’il faut préciser qu’il s’agit d’un tableau d’indices (avec le mot clé ``GL_ELEMENT_ARRAY_BUFFER``) et qu’il n'est pas nécessaire d'établir de liens avec les shaders. En effet ces indices interviennent en dehors des shaders, après l'exécution du vertex shader, au moment de la construction des primitives graphiques juste avant la rasterisation de ces primitives.

```
// Creation du buffer des indices
GLuint indiceBufferID;
glGenBuffers( 1, &indiceBufferID);

// Definition de vertexBufferID comme le bufferCourant de type GL_ELEMENT_ARRAY_BUFFER
glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indiceBufferID);

// transmission des indices dans ce buffer
glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), indices.data(), GL_STATIC_DRAW);
```
N’oubliez pas non plus de détruire ce buffer à la fin.

Enfin, il faut aussi changer la manière de dessiner dans la boucle de rendu, avec:

```
 glDrawElements( GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); 
```

qui vient remplacer l'appel ```glDrawArrays(GL_TRIANGLES, 0, vertices.size())``` dans le code sans les indices.


###### Travail à réaliser:

Faites toutes ces modifications, et vérifiez que le cube apparait correctement.

### Maillage
Nous allons maintenant nous intéresser à un mode de représentation des objets en 3D : les maillages. Un maillage  est une surface discrétisée sous la forme de polygones, très souvent des triangles. Nous nous en servons en informatique graphique pour représenter les objets.
Un maillage est constitué :
- d’un ensemble de points, les sommets des polygones formant notre maillage,
- d’un ensemble d’indices, qui indique pour chaque polygone les indices des sommets qui le constitue.

Pour stocker un maillage dans un fichier, il existe différents formats.  Le plus simple d’entre eux est le format  ```OFF```. Voici un exemple de fichier ``OFF`` représentant un maillage avec 5 sommets et 2 polygones, un quadrilatère (4 indices) et un triangle (3 indices):

```
OFF
5 2 0

0. 0. 0.
1. 0. 0.
1. 1. 0.
0. 1. 0.
2. 1. 0.

4 0 1 2 3
3 1 4 3
```

Vous observez les 5 lignes contenant les coordonnées des sommets, suivies des 2 lignes contenant au début le nombre de sommets du polygone, puis les indices des sommets de ce polygone.

Les fichiers ```Mesh.h``` et ```Mesh.cpp``` implémentent une classe permettant de représenter un maillage :  cette  classe contient des attributs pour les tableaux de positions, de normales,  de couleurs,  et d’indices.  Par  ailleurs,  cette classe propose de charger automatiquement ces informations à partir d’un fichier ```OFF```. Attention: ces fichiers ne traitent que les maillages contenant exclusivement des triangles.


```
//-------------------------------------------------
// Lecture du maillage
Mesh m("../models/cube.off");
m.vertices.data(); // acces au tableau des positions
m.faces.data(); // acces au tableau des indices des triangles
```


###### Travail à réaliser:

Chargez un maillage du répertoire ```models``` et affichez le. Le travail consistera principalement à remplacer le tableau ``vertices`` dans le code précédent par ``m.vertices``, et à remplacer le tableau ``indices`` dans le code précédent par ``m.faces``.




