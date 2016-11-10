___________________________________________________________________________________________

										IA_VOID
___________________________________________________________________________________________
Brunel Constant																 Rigaud Vincent


-------------
 Mod�lisation
-------------

L'une des premi�res t�ches que nous avons du r�aliser fut celle de trouver une fa�on de 
mod�liser notre environnement, et ce gr�ce aux informations qui nous sont envoy�es par 
le moteur IABootCamp.


L'environnement
---------------

	Le graphe
Le monde dans lequel �voluent nos npcs est un monde en "fausse" 2D, il est compos� de tuiles 
communiquant les unes avec les autres. Nous avons donc d�cider, sans grande originalit�, de 
repr�senter notre monde sous la forme d'un graphe. Il est alors repr�senter par un singleton 
nomm� Map dans notre code. Ce singleton est compos� d'une liste de nodes et propose diff�rentes 
fonctions relatives � ces nodes comme par exemple obtenir un chemin d'un node vers un autre.

	Les Nodes
Les nodes sont des objets que nous avons design� pour repr�senter toutes les tuiles de notre 
environnement. Pour cela un node est essentiellement une structure de donn�es et propose peu 
de service si ce n'est d'acc�der � ses donn�es. 
Un node regroupe les donn�es suivantes :
	- Un ID correspondant � l'ID de la tuile qu'il repr�sente
	- Une position (ou plut�t un num�ro de colonne et un num�ro de ligne)
	- Un type : si la tuile est connue, interdite, occup�e ou enfin libre 
	- deux tableaux de huit cases :
		- l'un pour stocker les voisins du Node,
		- l'autre pour stocker des informations sur les arr�tes du node.
		
Nous avons choisi de construire le graphe en connectant toutes les tuiles entre elles peu 
importe la pr�sence de mur, ou l'interditction de marcher sur une tuile. Nous avons fait 
cela pour garder plus de flexibilit�. Nous pensions que si l'on ne prenait que les tuiles
�tant atteignables comme voisins, il faudrait refaire � chaque tour un calcul pour s'assurer 
de l'�tat des voisins comme un changement de type, un mur qui bouge, etc...
La flexibilit� est aussi la raison pour laquelle nous avons deux tableaux de huit cases pour 
les voisins et les arr�tes : couvrir plus facilement (� nos yeux en tout cas) le jour o�
les tuiles tourneront. De plus cela nous permet de mieux coller � la repr�sentation du moteur
(respect des index/directions).

	La recherche de Chemin
� ce jour la recherche de chemin est r�alis�e par un algorithme A*, les seules modifications
que nous avons mises en place sont les suivantes :
	- d�crire quels types de tuiles sont interdites et lesquels sont autoris�s
	- les murs ne sont pas directement pris en compte dans l'heuristique mais nous regardons si
	  le type d'objet sur nos arr�tes nous permet de passer ou non.
Nous avions ajout� la possibilit� de definir les types de tuiles autoris�es pour diff�rencier
A* d'exploration avec l'influence d'un A* de d�placement vers une tuile d'arriv�e. Dans la 
recherche vers une tuile d'arriv�e, nous prenions en compte la possibilit� de bouger sur une 
case inconnue. Dans le cas de l'exploration avec l'influence, nous voulions rejoindre la tuile 
avec le plus d'influence en passant uniquement par les tuiles connues, or nous d�finissions les 
tuiles inconnues comme des tuiles d'influence. Ceci rendait l'exploration difficile et cette 
solution nous a apport� plus de nouveaux probl�mes que de solution aux anciens... Nous avons 
donc en partie rebrouss� chemin mais avons laiss� cette possiblit�, car on ne sait jamais ce 
que les game designers nous pr�pare.

	SearchMap
Par soucis de modulariser les fonctions et les outils, nous avons d�cid� de s�parer l'outil
de recherche de chemin (actuellement le A*) dans une classe sp�cifique. Ceci a pour but de
rendre l'utilisation de cet algorithme de mani�re ind�pendante de la map. Nous pourrons
"facilement" impl�menter d'autres outils par la suite et ainsi facilement changer les outils
NPCs pour leur permettre de faire plus de choses.


Influence
---------

C'est la derni�re feature que nous avons int�gr� dans notre solution. C'est encore un mix entre
l'influence et les tuiles non visit�es. Nous n'avons pas encore eu le temps de totalement finir
l'impl�mentation de ce syst�me d'influence dans le sens ou ce n'est pas la seule chose qui guide
notre exploration.
Pour faire notre map d'influence, il nous a fallut trouver et d�finir des points d'influence, 
nous avons pour cela choisit arbitrairement certains type qui nous int�ress� :
	- Une tuile avec des vitres,
	- Une tuile inconnue mais suppos�e accessible
Cependant nous propageons l'influence uniquement sur les cases connues et accessibles. 
Concernant la propagation de cette influence, nous avons eu quelques soucis a d�finir la range
dont nous avions besoin. Nous sommes parti sur la range de vision donn�e dans le LevelInfo.

Les limites actuelles
---------------------

Nous connaissons certaines limites � notre impl�mentation actuelle du mod�le. Voici quelques
exemples :
	- Si les tuiles tournent, nous avons pr�vu la structure mais les calculs ne sont pas encore
	impl�ment�s.
	- Il en est de m�me pour les calculs si jamais les lignes paires sont d�cal�es vers la gauche.
	- Le code est � ce jour pas du tout optimis�.


-----------
 Les Agents
-----------

Lors de la mise en commun du code, nous avons souhait� s�parer la logique d'un NPC et la logique
de gestion de tous les NPCs. Pour cela, nous avons mis en place une classe NPC qui a pour but de 
g�rer les actions d'un et UN SEUL NPC. MyBotLogic est alors devenu l'agent responsable de tous les
NPCs.

MyBotLogic
----------
Il est responsable de la mise � jour de la map car celle-ci n'a pas connaissance du _turnInfo.
Il s'occupe aussi de l'update des NPCs pour leur permettre de faire leurs actions. Comme nous
voulions qu'il s'occupe de la gestion de tous les NPCs, nous avons mis en place la logique de
suivi de chemin ici. C'est � dire que si deux NPCs veulent aller sur la m�me case, MyBotLogic
a la possibilit� de dire stop � un NPC. Pour choisir lequel doit passer en premier, nous avons
choisi la logique suivante : Soit c'est le NPC qui a le plus long chemin qui passe en premier,
soit, si les deux NPC ont la meme distance a parcourir, l'ordre est plus arbitraire en laissant
la priorit� au NPC � l'ID le plus petit.
C'est �galement ici que l'on attribut les goals aux NPCs et pour cela nous devions faire une
disjonction des cas :
	- Si nous avons plus de goals que de NPCs, nous devions attribuer au NPC le goal le plus
	proche
	- Si nous avions plus de NPCs que de goal, le NPC le plus proche est alors attribu� a ce
	goal l�.
Nous devions faire �a pour �viter que des NPCs se rendent � des goals qui sont a l'autre bout
de la map.

NPC
---
Concernant les actions requisent par MyBotLogic, celles-ci sont donn�es par le NPC. Au cours de
son update, il stocke ses actions de fa�on non d�finitive et il propose des services pour retirer
certains types d'actions de sa liste.
Un NPC a vraiment uniquement pour but de g�rer son propre comportement, il ne prend pas en 
compte les autres NPCs.
Le NPC est pour le moment une machine � �tats finis (FSM) qui poss�de � ce jour les �tats suivants :
	- Explore	
	- Moving	
	- Waiting	
	- Interacting	
	- Arrived

Les �tats Waiting et Interacting ne sont � ce jour pas encore utilis�s mais pourront �tre utile 
par la suite.
	
	Explore
Si jamais c'est le tour d'un NPC et que celui ci n'a pas de goal ou de target (le goal �tant une 
tuile de type GOAL alors qu'une target est juste une tuile o� aller), on lui demande d'explorer 
la map.
Pour cela, il a plusieurs possibilit�s : 
	- d�couvrir la tuile juste � cot� de lui qui poss�de le plus d'influence, dans ce cas il reste
	en EXPLORE
	- et si toutes les cases � cot� de lui ont la m�me influence, alors il cherche � rejoindre 
	la case la plus proche qui n'a pas �t� vue (Exception faite si jamais il n'a qu'une case 
	possiblement accessible), dans ce cas il passe en �tat MOVING
	
	Moving
Cet �tat repr�sente le fait que le NPC doit rejoindre une certaine case en suivant un chemin.
Cet case peut etre:
	- Une target, dans ce cas une fois dessus, le NPC repasse en exploration
	- Un GOAL, dans ce cas une fois dessus, le NPC passe en �tat ARRIVED
	
	Arrived
Cet �tat est juste pr�sent pour dire qu'un NPC est bien arriv� sur son GOAL. Pour le moment on
ne peut pas changer mais le but et de pouvoir lui redefinir un �tat pour qu'il puisse � nouveau
bouger.


--------------
 Loggers/Debug
--------------

Nous avons choisis de mettre en place les loggers de facon ind�pendantes. C'est a dire que nous
avons un logger pour chaque NPC, pour notre MyBotLogic ainsi que pour nos maps. Cela � pour but 
de bien s�parer les �tats de chacun et avoir les informations que l'on souhaite la ou l'on veut 
et ne pas avoir � chercher.
Les NPCs ont toute la logique de leurs actions, ce qui nous permet de mieux comprendre leur 
fonctionnement et leurs decisions.
La map d'influence nous permet d'avoir un aper�u de l'influence sur chaque tour.
Nous avons un deuxi�me logger de notre map qui nous permet de voir l'�tat des tuiles sur chaque 
tour ainsi que la position des NPCs.
Le logger de MyBotLogic nous permet principalement de connaitre les mises � jour de la map pour 
l'instant. C'�tait ce dont nous avions besoin pour le moment mais tout autre log peut y �tre ajout�.


--------------------------
 Evaluation du temps pass�
--------------------------

Nous pensons avoir approximativement pass� entre 25 et 30h sur le d�veloppement de l'application.
Dans ce temps la, la fusion de nos deux parties a pris pas mal de temps car nous en avons profit�
pour faire du refactoring ainsi que le d�placement de la logique dans le NPC et la map. Nous avons
repens� � notre mod�le pour r�ussir de cr�er le comportement que l'on voulait.
La mise en place de l'exploration a pris �galement un peu de temps pour ajouter la logique dans le
NPC en fonction de ses informations, si jamais il n'a pas de goal, il doit le trouver en explorant 
la carte.
La vision n'a pas pris beaucoup de temps car la mise � jour de la map �tait d�j� pens� avant la 
fusion des codes ce qui nous a permis de faire juste quelques ajustements concernant la vision, 
comme par exemple ajouter des types de tile en NONE qui sont � d�couvrir.
La gestion des murs a �t� rapidement g�r� pour empecher le A* de passer par des murs.
Les vitres ont quasiment pas chang� le code, la mise � jour de la map �tait d�j� pr�sente, nous 
avons consid�r�s les vitres �tant des obstacles comme des murs, donc pas de changements
� ce niveau la non plus. Nous avons simplement du ajouter quelques lignes de codes pour g�rer les
goals qui n'�taient pas accessible.
Enfin la map d'influence nous a pris un peu de temps car nous avons d� pas mal penser au 
fonctionnement et � l'impl�mentation de la diffusion pour que cela fonctionne avec A* et 
notre mod�le.

Pr�sentement, nous r�ussissons toutes les maps (jusqu'� la 40) et nous attendons avec impatience
les maps suivantes :)

Constant & Vincent