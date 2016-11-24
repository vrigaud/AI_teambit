___________________________________________________________________________________________

										IA_VOID
___________________________________________________________________________________________
Brunel Constant																 Rigaud Vincent


-------------
 Modélisation
-------------

L'une des premières tâches que nous avons du réaliser fut celle de trouver une façon de 
modéliser notre environnement, et ce grâce aux informations qui nous sont envoyées par 
le moteur IABootCamp.


L'environnement
---------------

	Le graphe
Le monde dans lequel évoluent nos npcs est un monde en "fausse" 2D, il est composé de tuiles 
communiquant les unes avec les autres. Nous avons donc décider, sans grande originalité, de 
représenter notre monde sous la forme d'un graphe. Il est alors représenter par un singleton 
nommé Map dans notre code. Ce singleton est composé d'une liste de nodes et propose différentes 
fonctions relatives à ces nodes comme par exemple obtenir un chemin d'un node vers un autre.

	Les Nodes
Les nodes sont des objets que nous avons designé pour représenter toutes les tuiles de notre 
environnement. Pour cela un node est essentiellement une structure de données et propose peu 
de service si ce n'est d'accéder à ses données. 
Un node regroupe les données suivantes :
	- Un ID correspondant à l'ID de la tuile qu'il représente
	- Une position (ou plutôt un numéro de colonne et un numéro de ligne)
	- Un type : si la tuile est connue, interdite, occupée ou enfin libre 
	- deux tableaux de huit cases :
		- l'un pour stocker les voisins du Node,
		- l'autre pour stocker des informations sur les arrêtes du node.
		
Nous avons choisi de construire le graphe en connectant toutes les tuiles entre elles peu 
importe la présence de mur, ou l'interditction de marcher sur une tuile. Nous avons fait 
cela pour garder plus de flexibilité. Nous pensions que si l'on ne prenait que les tuiles
étant atteignables comme voisins, il faudrait refaire à chaque tour un calcul pour s'assurer 
de l'état des voisins comme un changement de type, un mur qui bouge, etc...
La flexibilité est aussi la raison pour laquelle nous avons deux tableaux de huit cases pour 
les voisins et les arrêtes : couvrir plus facilement (à nos yeux en tout cas) le jour où
les tuiles tourneront. De plus cela nous permet de mieux coller à la représentation du moteur
(respect des index/directions).

	La recherche de Chemin
À ce jour la recherche de chemin est réalisée par un algorithme A*, les seules modifications
que nous avons mises en place sont les suivantes :
	- décrire quels types de tuiles sont interdites et lesquels sont autorisés
	- les murs ne sont pas directement pris en compte dans l'heuristique mais nous regardons si
	  le type d'objet sur nos arrêtes nous permet de passer ou non.
Nous avions ajouté la possibilité de definir les types de tuiles autorisées pour différencier
A* d'exploration avec l'influence d'un A* de déplacement vers une tuile d'arrivée. Dans la 
recherche vers une tuile d'arrivée, nous prenions en compte la possibilité de bouger sur une 
case inconnue. Dans le cas de l'exploration avec l'influence, nous voulions rejoindre la tuile 
avec le plus d'influence en passant uniquement par les tuiles connues, or nous définissions les 
tuiles inconnues comme des tuiles d'influence. Ceci rendait l'exploration difficile et cette 
solution nous a apporté plus de nouveaux problèmes que de solution aux anciens... Nous avons 
donc en partie rebroussé chemin mais avons laissé cette possiblité, car on ne sait jamais ce 
que les game designers nous prépare.

	SearchMap
Par soucis de modulariser les fonctions et les outils, nous avons décidé de séparer l'outil
de recherche de chemin (actuellement le A*) dans une classe spécifique. Ceci a pour but de
rendre l'utilisation de cet algorithme de manière indépendante de la map. Nous pourrons
"facilement" implémenter d'autres outils par la suite et ainsi facilement changer les outils
NPCs pour leur permettre de faire plus de choses.


Influence
---------

C'est la dernière feature que nous avons intégré dans notre solution. C'est encore un mix entre
l'influence et les tuiles non visitées. Nous n'avons pas encore eu le temps de totalement finir
l'implémentation de ce système d'influence dans le sens ou ce n'est pas la seule chose qui guide
notre exploration.
Pour faire notre map d'influence, il nous a fallut trouver et définir des points d'influence, 
nous avons pour cela choisit arbitrairement certains type qui nous intéressé :
	- Une tuile avec des vitres,
	- Une tuile inconnue mais supposée accessible
Cependant nous propageons l'influence uniquement sur les cases connues et accessibles. 
Concernant la propagation de cette influence, nous avons eu quelques soucis a définir la range
dont nous avions besoin. Nous sommes parti sur la range de vision donnée dans le LevelInfo.

Les limites actuelles
---------------------

Nous connaissons certaines limites à notre implémentation actuelle du modèle. Voici quelques
exemples :
	- Si les tuiles tournent, nous avons prévu la structure mais les calculs ne sont pas encore
	implémentés.
	- Il en est de même pour les calculs si jamais les lignes paires sont décalées vers la gauche.
	- Le code est à ce jour pas du tout optimisé.


-----------
 Les Agents
-----------

Lors de la mise en commun du code, nous avons souhaité séparer la logique d'un NPC et la logique
de gestion de tous les NPCs. Pour cela, nous avons mis en place une classe NPC qui a pour but de 
gérer les actions d'un et UN SEUL NPC. MyBotLogic est alors devenu l'agent responsable de tous les
NPCs.

MyBotLogic
----------
Il est responsable de la mise à jour de la map car celle-ci n'a pas connaissance du _turnInfo.
Il s'occupe aussi de l'update des NPCs pour leur permettre de faire leurs actions. Comme nous
voulions qu'il s'occupe de la gestion de tous les NPCs, nous avons mis en place la logique de
suivi de chemin ici. C'est à dire que si deux NPCs veulent aller sur la même case, MyBotLogic
a la possibilité de dire stop à un NPC. Pour choisir lequel doit passer en premier, nous avons
choisi la logique suivante : Soit c'est le NPC qui a le plus long chemin qui passe en premier,
soit, si les deux NPC ont la meme distance a parcourir, l'ordre est plus arbitraire en laissant
la priorité au NPC à l'ID le plus petit.
C'est également ici que l'on attribut les goals aux NPCs et pour cela nous devions faire une
disjonction des cas :
	- Si nous avons plus de goals que de NPCs, nous devions attribuer au NPC le goal le plus
	proche
	- Si nous avions plus de NPCs que de goal, le NPC le plus proche est alors attribué a ce
	goal là.
Nous devions faire ça pour éviter que des NPCs se rendent à des goals qui sont a l'autre bout
de la map.

NPC
---
Concernant les actions requisent par MyBotLogic, celles-ci sont données par le NPC. Au cours de
son update, il stocke ses actions de façon non définitive et il propose des services pour retirer
certains types d'actions de sa liste.
Un NPC a vraiment uniquement pour but de gérer son propre comportement, il ne prend pas en 
compte les autres NPCs.
Le NPC est pour le moment une machine à états finis (FSM) qui possède à ce jour les états suivants :
	- Explore	
	- Moving	
	- Waiting	
	- Interacting	
	- Arrived

Les états Waiting et Interacting ne sont à ce jour pas encore utilisés mais pourront être utile 
par la suite.
	
	Explore
Si jamais c'est le tour d'un NPC et que celui ci n'a pas de goal ou de target (le goal étant une 
tuile de type GOAL alors qu'une target est juste une tuile où aller), on lui demande d'explorer 
la map.
Pour cela, il a plusieurs possibilités : 
	- découvrir la tuile juste à coté de lui qui possède le plus d'influence, dans ce cas il reste
	en EXPLORE
	- et si toutes les cases à coté de lui ont la même influence, alors il cherche à rejoindre 
	la case la plus proche qui n'a pas été vue (Exception faite si jamais il n'a qu'une case 
	possiblement accessible), dans ce cas il passe en état MOVING
	
	Moving
Cet état représente le fait que le NPC doit rejoindre une certaine case en suivant un chemin.
Cet case peut etre:
	- Une target, dans ce cas une fois dessus, le NPC repasse en exploration
	- Un GOAL, dans ce cas une fois dessus, le NPC passe en état ARRIVED
	
	Arrived
Cet état est juste présent pour dire qu'un NPC est bien arrivé sur son GOAL. Pour le moment on
ne peut pas changer mais le but et de pouvoir lui redefinir un état pour qu'il puisse à nouveau
bouger.


--------------
 Loggers/Debug
--------------

Nous avons choisis de mettre en place les loggers de facon indépendantes. C'est a dire que nous
avons un logger pour chaque NPC, pour notre MyBotLogic ainsi que pour nos maps. Cela à pour but 
de bien séparer les états de chacun et avoir les informations que l'on souhaite la ou l'on veut 
et ne pas avoir à chercher.
Les NPCs ont toute la logique de leurs actions, ce qui nous permet de mieux comprendre leur 
fonctionnement et leurs decisions.
La map d'influence nous permet d'avoir un aperçu de l'influence sur chaque tour.
Nous avons un deuxième logger de notre map qui nous permet de voir l'état des tuiles sur chaque 
tour ainsi que la position des NPCs.
Le logger de MyBotLogic nous permet principalement de connaitre les mises à jour de la map pour 
l'instant. C'était ce dont nous avions besoin pour le moment mais tout autre log peut y être ajouté.


--------------------------
 Evaluation du temps passé
--------------------------

Nous pensons avoir approximativement passé entre 25 et 30h sur le développement de l'application.
Dans ce temps la, la fusion de nos deux parties a pris pas mal de temps car nous en avons profité
pour faire du refactoring ainsi que le déplacement de la logique dans le NPC et la map. Nous avons
repensé à notre modèle pour réussir de créer le comportement que l'on voulait.
La mise en place de l'exploration a pris également un peu de temps pour ajouter la logique dans le
NPC en fonction de ses informations, si jamais il n'a pas de goal, il doit le trouver en explorant 
la carte.
La vision n'a pas pris beaucoup de temps car la mise à jour de la map était déjà pensé avant la 
fusion des codes ce qui nous a permis de faire juste quelques ajustements concernant la vision, 
comme par exemple ajouter des types de tile en NONE qui sont à découvrir.
La gestion des murs a été rapidement géré pour empecher le A* de passer par des murs.
Les vitres ont quasiment pas changé le code, la mise à jour de la map était déjá présente, nous 
avons considérés les vitres étant des obstacles comme des murs, donc pas de changements
à ce niveau la non plus. Nous avons simplement du ajouter quelques lignes de codes pour gérer les
goals qui n'étaient pas accessible.
Enfin la map d'influence nous a pris un peu de temps car nous avons dû pas mal penser au 
fonctionnement et à l'implémentation de la diffusion pour que cela fonctionne avec A* et 
notre modèle.

Présentement, nous réussissons toutes les maps (jusqu'à la 40) et nous attendons avec impatience
les maps suivantes :)

Constant & Vincent