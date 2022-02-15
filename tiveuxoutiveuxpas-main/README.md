#Objectif du projet

Ce projet s'inscrit dans notre 2ème année du cursus ingénieur de l'INSA de Rennes, dans le cours de Traitement images et vidéos 
commun aux options Data Science et Médias et Interactions.

Nous avons pour but de traiter un ensemble de formulaires d'icônes, rempli à la main. <br>
Nous avons pour cela conçu un programme qui permet d'exploiter et de traiter toutes les images (même celles qui sont mal cadrées),
afin d'extraire chaque pictogramme dessiné, et de récupérer les informations relatives. <br>
Nous générons ensuite pour chacune d’elles un fichier texte de description avec toutes les informations, le numéro du formulaire 
ainsi que l’emplacement du pictogramme dans le formulaire.

Enfin, nous sommes aussi en charge d'évaluer notre méthode sur le jeu d'entraînement et sur le jeu de test, afin de déterminer 
si nos choix et nos solutions ont été cohérentes.

---

#Fonctionnement de notre programme :

### Étape 1 : Recadrage et redimensionnement des images
Afin de pouvoir exploiter toutes les images, y compris celles qui sont décalées, nous avons décidé de pré-traiter tous les formulaires dans le but de les standardiser. <br>
Pour cela, nous détectons la croix du haut de l'image pour chaque formulaire, et nous comparons le décalage par rapport à une image parfaitement centrée.<br>
Nous utilisons ensuite la fonction *translate()* pour la recadrer.

Ensuite, nous détectons la croix du bas de l'image, et nous calculons la distance qui les sépare, afin de calculer l'angle de rotation et le redimensionnement à appliquer 
à l'image grâce aux fonctions *getAngleABC()* et *resize()*.

### Étape 2 : Récuperation des informations pour chaque ligne à partir des *templates*
Une fois notre image standardisée, nous récuperont les informations des pictogrammes (type, taille, position) grâce à la fonction *MatchTemplate* d'OpenCV.<br>
Avec les fonctions *iconDetection* et *sizeDetection*, nous les stockons dans une structure *DetectionStruct*.<br>
Cette structure contient un *boolean* qui nous informe s'il est détecté, sa position, sa taille et son type.

### Étape 3 : Création des imagettes png et fichier txt associé
Nous avons ainsi, pour chaque ligne, une structure *detectionStruct* contenant les informations relatives aux pictogrammes. <br>
Sachant que chaque imagette est situé à une certaine distance (x,y) du pictogramme de départ dans le formulaire, <br>
nous pouvons extraire les imagettes grâce a la fonction *extractImagette()* qui les place dans un dossier avec le nom associé demandé (type_dossier_formulaire_ligne).<br>
Le fichier Texte associé qui contient toutes les informations demandées y est aussi créé via la fonction *makeFichierTexte()*.
Il contient précisément : 
* Le type du pictogramme
* Le numéro du formulaire
* Le numéro du dossier
* Le numéro de page
* Son emplacement dans le formulaire (ligne et colonne)
* Sa taille si elle existe
  * *Boolean* dans la structure *detectionStruct*
  
---

#Méthode d'évaluation :
Afin de déterminer si nos choix ont été les bons, nous avons mis en place une méthode d'évaluation du taux de réussite d'extraction.<br>
Pour ce faire, nous créons un compteur d'imagettes non détectées, qui nous allons incrémenter de différentes manières :
* De 35 si la croix supérieure d'une page n'est pas détectée (impossibilité de la standardiser)
  * Une page de formulaire comprend 35 imagettes
* de 5 si un pictogramme n'est pas détecté
  * Une ligne comprend 5 imagettes
Cela nous permet donc, à la fin, de calculer le taux d'échec de notre extraction.

---

Ce projet a été réalisé en 2021 lors du module TIV par [Rodrigue Bourel](https://www.linkedin.com/in/rodriguebourel/), [Arthur Mouliner](https://www.linkedin.com/in/arthur-moulinier-322598156/),
[Mélissande Simonin](https://www.linkedin.com/in/m%C3%A9lissande-simonin-967347172/) et [Nour Zbitou](https://www.linkedin.com/in/nour-zbitou-23b5b620a/),