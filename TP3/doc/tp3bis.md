## Synthèse d’Image - Polytech Grenoble, INFO4, 2024-2025 

## TP3 (suite) : VBO, Maillages et indexation


## Introduction
Ce TP poursuit le TP précédent. Si vous ne l’avez pas encore fait, terminez le TP3. Vous devez parvenir à charger un maillage indexé (avec la classe Mesh), puis à créer et envoyer les données vers des buffers sur le GPU avant de dessiner. Les maillages fournis ne sont pas tous de la même taille et ne sont pas centrés autour de la même position. Vous pouvez utiliser la fonction ```normalize``` dans la classe ```Mesh``` pour éviter d’avoir à modifier la caméra à chaque fois que vous ouvrez un nouvel objet. Cette fonction applique une translation et une transformation d'échelle à l'objet de telle manière à ce qu'il soit inclus dans la sphère unité. La fonction ```colorize``` permet de recalculer les couleurs par défaut si l'objet n'a pas de couleurs.

```
  Mesh m("../models/armadillo.off");
  m.normalize(); // met a l'echelle pour que le maillage remplisse la sphere unite
  m.colorize(); // calcul une couleur en chaque sommet
  m.vertices.data(); // acces au tableau des positions (tableau de vec3)
  m.vertices.size(); // taille du tableau de sommets, nombre de sommets
  m.normals.data(); // acces au tableau des normales (tableau de vec3)
  m.colors.data(); // acces au tableau des couleurs (tableau de vec3)
  m.faces.data(); // acces aux indices des sommets de chaque face (tableau d'entier)
```

##### Buffers
Cette section rappelle l’utilisation des tampons ou buffers.  Un tampon est une zone mémoire dans le GPU dans laquelle vous stockez les données nécessaires à l’affichage de votre objet 3D. Ces données sont d’abord générées par le CPU, et représentées dans la mémoire de celui-ci (la mémoire vive). Par la suite, elles seront copiées dans la mémoire du GPU (cache) dans des tampons afin d’être utilisées directement par les shaders.  Enfin, les tampons  seront détruits, libérant l’espace qu’ils occupaient en mémoire cache.

###### Création d'un buffer

Pour créer les données il faut :

- Déclarer un conteneur de données, i.e. le tableau C++ sur le CPU dans lequel sont initialisées les données:
    ```std::vector<T> conteneur;```
    Ici ```T``` représente le type de vos données. Ce sera par exemple ```glm::vec3``` pour des positions ou des couleurs RGB, ou ```uint``` (entiers non signés) pour des indices. Vous remplacerez ```conteneur``` par le nom que vous souhaitez donner à votre tableau, par exemples ```vertices``` ou ```normals```.

- Remplir le conteneur, par exemple:
   ```
   conteneur.push_back(glm::vec3(0.0f));
   conteneur.push_back(glm::vec3(1.0f, 0.0f, 0.5f));
   ```

Pour créer un tampon et y recopier les données précédentes il faut:

- Déclarer un identifiant pour le tampon:
  ```
  GLuint bufferID;
  ```
  Cet identifiant servira de référence dans le CPU vers le tampon situé dans le GPU.

- Générer le tampon dans le GPU:
  ```
  glGenBuffers( 1, &bufferID);
  ```

- Afficher l'identifiant ```bufferID```:
  ```
  std::cout << "bufferID = " << bufferID << std::endl;
  ```
  Cet affichage est facultatif. La valeur devrait être comprise entre 1 et le nombre de tampons que vous avez créés jusqu’à maintenant (typiquement moins d’une dizaine dans le cadre de ces TP).

- Définir ce tampon comme le tampon courant:
  ```
  glBindBuffer( GL_ARRAY_BUFFER, bufferID);
  ```
  Si votre tampon contient des indices (pour l’indexation des sommets), il faut remplacer ```GL_ARRAY_BUFFER``` par ```GL_ELEMENT_ARRAY_BUFFER```. 

- et enfin copiez les données du tableau ```conteneur``` dans le CPU vers le tampon dans le GPU:
  ```
  glBufferData( GL_ARRAY_BUFFER, conteneur.size() * sizeof(T), conteneur.data(), GL_STATIC_DRAW);
  ```
  Rappelez vous que ```T``` est le type des données contenues dans le tampon, typiquement ```glm::vec3``` ou ```uint```, et qu'il faut remplacer ```GL_ARRAY_BUFFER``` par ```GL_ELEMENT_ARRAY_BUFFER``` si votre tampon contient des indices.

###### Utilisation des buffers

Les données situées dans les tampons vont  être utilisées directement dans les shaders.  Pour ceci, il faut préciser   le lien entre chaque variable  d’entrée du shader et le tampon d’attributs correspondant.  Il faut le faire pour tous les buffers excepté le buffer d’indices. En effet le buffer d'indices ne représente pas des attributs, il n'est pas utilisé dans les shaders, il sert uniquement à former les primitives graphiques après le vertex shader et avant la rasterisation de ces primitives. La variable d’entrée du shader doit être d’un type compatible avec celle stockée dans le tampon : par exemple un ```glm::vec3``` en C++ est compatible avec le type ```vec3``` natif de ```GLSL```. 

Afin de spécifier le lien entre une variable d’entrée d’un shader et un tampon, il faut: 

- Récupérer l’identifiant de la variable :
  ```GLuint varID = glGetAttribLocation(programID, "in_variable");```
  Ici, on suppose que le shader a été compilé et que le programme résultant est identifié par ```programID```. Bien entendu ```in_variable``` est à remplacer par le nom de l'attribut dans le shader GLSL, par exemple ```in_color```. ```varID``` est un entier qui sert de référence dans le programme C++ vers la variable ```in_variable``` dans le programme GLSL.

- Préciser à OpenGL la manière dont le tampon doit être lu, et qu'il faut transmettre les valeurs du tampon dans la variable référencée par ```varID```. Attention: le tampon correspondant (```bufferID```) doit être bien actif. C’est le cas si vous enchainez les commandes. Sinon il faudra refaire un ```glBindBuffer``` avec le bon tampon :
  ```
  glVertexAttribPointer(varID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  ```
  Ici, ```varID``` indique la variable d’entrée avec laquelle lier le tampon courant, ```3``` est le nombre de composants  de la variable, ```GL_FLOAT``` est le type des composantes. Les autres paramètres ne nous intéressent pas pour l’instant.

- Activer la variable d’entrée du shader :
  ```glEnableVertexAttribArray(varID);```

Toutes ces instructions sont à mettre à l’initialisation du programme, avant la boucle de rendu.

###### Destruction des buffers

Une fois que vous avez  fini de vous en servir, c’est à dire une fois sorti de la boucle de rendu, il faut détruire   tous les buffers que vous avez créé. Pour ceci, il suffit de faire :
```glDeleteBuffers(1, &bufferID);```

###### Encapsulation des buffers dans le VAO et dessin

Avant même la création du premier buffer, nous avions créé et activé un Vertex Array Object (VAO). Lorsqu’on crée/active un nouveau buffer d’attributs, celui-ci sera automatiquement encapsulé/lié au VAO courant (un peu comme un conteneur qui contient tous les tableaux créés). Cela permet, lors du dessin dans la boucle de rendu, de n'avoir à se référer qu'au VAO pour activer tous les buffers. Par exemple :


  ```
  glBindVertexArray(vaoID);
  glDrawElements( GL_TRIANGLES, m.faces.size(), GL_UNSIGNED_INT, 0)
  ```
Ce code active tous les buffers (VBOs) associés au VAO identifié par ```vaoID```, puis les dessine avec les indices des  faces d’un maillage.

### Travail à faire

#### Ajout d'un attribut

Le maillage contient 3 buffers d’attributs (positions, normales et couleurs). Assurez vous que ces 3 attributs soient bien disponibles dans le vertex shader. Normalement, vous devriez déjà en avoir 2. Il vous faut donc créer/initialiser un nouveau buffer (dans le programme C++) et un attribut dans le vertex shader, en suivant pas à pas les instructions que nous venons de détailler.

#### Découpage de la fenêtre en plusieurs zones

La fonction suivante permet de restreindre la zone de dessin dans la fenêtre courant : ```glViewport(x,y,w,h);```.  ```x,y``` sont les coordonnées entières du pixel en bas à gauche de la zone de dessin. ```w,h``` sont respectivement la largeur et la hauteur, en pixels, de la zone de dessin. Pour dessiner dans une certaine zone, il suffit d’appeler cette fonction avant d’utiliser la commande ```glDrawElements```. Il faut dessiner plusieurs fois avec des appels différents à ```glViewport``` pour dessiner dans plusieurs zones:

  ```
  glBindVertexArray(vaoID); // On active le VAO

  glViewport( ... );
  glDrawElements( ... );

  ...

  glViewport( ... );
  glDrawElements( ... );

  glBindVertexArray(0);

  ```

  Sachant que ```glfwGetFramebufferSize( myWindow, &windowWidth, &windowHeight);``` permet de récupérer la taille actuelle de la fenêtre active et de la stocker dans les variables ```windowWidth``` et ```windowHeight```, arrangez-vous pour découper votre écran en 4 et pour dessiner votre objet dans chacune     de ces sous-fenêtres.

#### Envoi de variables uniformes au shader

On aimerait avoir une visualisation différente de l’objet dans chacune des sous fenêtres, à l'aide d'un seul shader. Pour cela, nous allons informer le shader du dessin qu'il doit réaliser, en lui envoyant un entier entre 1 et 4. Ce processus d'envoi est exactement le même que pour l’envoi des matrices model/view/projection :
- Ajouter une variable ```uniform``` dans le vertex shader (de type ```int```).
- Au même moment que l’initialisation des matrices, récupérer la localisation de cette nouvelle variable dans le shader GLSL avec```glGetUniformLocation```.

- A chaque dessin, modifier dans le programme C++ la valeur de cette variable avec la fonction ```glUniform1i(variableID,valeurEntiere)``` (fonctionne de la même manière que ```glUniformMatrix```)

Arrangez vous pour dessiner les normales, les positions et les couleurs dans des zones différentes.  Essayez de trouver la formule calculant la profondeur depuis la caméra,  et visualiser cette profondeur dans la 4ième zone.

#### Animation
Envoyez une nouvelle variable uniform de type ```float``` au vertex shader qui contiendra le temps courant (variable ```cur_time```). Essayez d’animer votre objet à l’aide de cette variable.  Par  exemple, vous pouvez la faire onduler au  cours du temps (en utilisant un sinus qui multiplie l’une des coordonnées).  Pour modifier la valeur  de la variable,     on utilisera ```glUniform1(variableID,valeurFlottante)``` dans ce cas. Vous pouvez aussi modifier cette variable pour qu’elle soit différente dans les différentes zones pour obtenir des animations diverses.
Note: l’utilisation de (trop de) conditions dans  les  shaders  n’est  en  général  pas  recommandée.  Il  vaut  mieux créer plusieurs shaders et switcher entre eux lorsque c’est nécessaire.
