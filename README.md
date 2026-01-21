## Requirements
* Windows
* MSYS2 MinGW
* OpenGL
* GLAD

Generate [GLAD](https://gen.glad.sh/#generator=c&api=gl%3D3.3&profile=gl%3Dcore%2Cgles1%3Dcommon&options=LOADER)
and copy *include* and *src* into *external* folder.

## A faire :
* [x] Gerer une fenetre en utilisant l'API Windows
	* [x] Creer un contexte OpenGL
	* [x] Gerer les entrees clavier et souris
* [ ] Gerer une fenetre en utilisant l'API X11
	* [ ] Creer un contexte OpenGL
	* [ ] Gerer les entrees clavier et souris
* [x] Charger les fonctions OpenGL avec GLAD
* [x] Dessiner un triangle
* [ ] Empaqueter les ressources
	* [x] Empaqueter les shaders
	* [x] Empaqueter les images
	* [ ] Compresser les images
	* [x] Convertir les polices en spritesheets
	* [x] Empaqueter les polices
	* [ ] Ajouter la meta des polices
	* [ ] Empaqueter les fichiers audio
* [ ] Lire les donnees empaquetees
* [ ] Appliquer une texture