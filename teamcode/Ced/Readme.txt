   /_\ |_ _| _ ) ___| |_
  / _ \ | || _ \/ _ \  _|
 /_/ \_\___|___/\___/\__|

---------------------
 ÉQUIPE
---------------------
 HardVegetables

---------------------
 MEMBRES
---------------------
 Lloyd Kelly
 Cédric Sentenac

---------------------
 TEMPS DE TRAVAIL TOTAL
---------------------
 ~90-100h/membre

---------------------
 SOMMAIRE DU FICHIER
---------------------
1*/ Mise en contexte

2*/ Description de l'approche

3*/ Limites & améliorations possibles



---------------------
1*/ MISE EN CONTEXTE
---------------------
Le projet AIBot consiste à développer un système d'intelligence artificielle pour un jeu de type tour par tour. Le but étant de permettre à des agents, aussi appelés NPC ("Non-Player Character"), de se rendre d'un point de départ à un point d'arrivée pour remporter une partie. Nous avons à notre disposition les informations de l'environnement qui nous sont transmises à chaque tour, via l'engin (AIBootCamp Engine), telles que la dimension de la carte, à quelle position nos agents sont placés, la présence d'éventuels murs, etc. Avant toute chose, il est donc nécessaire de représenter ces informations dans notre système pour pouvoir ensuite les analyser, les traiter et ainsi déterminer la ou les actions qui seront effectuées par nos agents.


---------------------
2*/ DESCRIPTION DE L'APPROCHE
---------------------

Tout d'abord, il est bien évidemment primordial de représenter la carte sur laquelle nos agents se situent. Pour ce faire, nous avons opté pour l'utilisation d'un graphe contenant une liste de noeuds. Un noeud représente une tuile (ou une case) de la carte et stocke toute l'information qui la concerne : son identifiant, la liste de ses tuiles voisines, une liste de murs si elle en possède, un indicateur permettant de déterminer si cette tuile est occupée par un agent ou non ou encore si elle est accessible ou non (interdite, "forbidden"). Nous avons aussi créé une structure qui nous permet de représenter nos agents et stocke les informations relatives à l'agent en question telles que sa position, son but ainsi que le chemin lui permettant de l'atteindre, les tuiles qu'il aura explorées - surtout nécessaires lorsque le mode vision est activé - ainsi que la liste de ses cibles (potentiels buts) les plus proches.

À l'initialisation du jeu, le graphe est créé à partir des informations fournies par l'engin. Ainsi en mode omniscient, nous pouvons directement accéder et stocker tous les éléments constituants chaque tuile et avons donc par conséquent connaissance de l'emplacement des différents buts possiblement accessibles par les agents. Dans ce cas-là, aucune opération de mise à jour du graphe n'est requise. Par contre, lorsque l'on est en mode vision, nous avons accès à une quantité limitée d'informations, dépendamment du champ de vision de l'agent. Il est alors nécessaire de mettre à jour le graphe à chaque tour pour récupérer les nouvelles données fournies par l'engin.

Afin que les NPCs puissent atteindre leur but respectif, nous avons recours à l'algorithme A*, soit un algorithme de recherche afin de trouver un chemin optimal pour s'y rendre à l'aide des informations fournies par le graphe.

Dans le cas du mode omniscient, nous avons simplement à déterminer la cible la plus proche d'un agent par l'entremise d'une heuristique et l'on détermine le chemin que l'agent devra emprunter pour s'y rendre.

Lorsque l'on est en mode vision, étant donné que nous n'avons pas accès à toute l'information de la carte, quand le contenu d'une tuile n'est pas connu, nous supposons que celle-ci est accessible. Si une ou plusieurs cibles sont disponibles, celles-ci seront assignées à un agent en particulier selon la plus petite distance qui sépare ces cibles des différents agents ne possédant aucun but. Dans le cas contraire, donc si aucune cible n'est disponible, les agents se déplaceront pour explorer la carte. Au fur et à mesure que les agents l'exploreront, les éléments informatifs (murs, cases adjacentes, etc.) liés aux tuiles visibles, selon le champ de vision, seront ajouté dans le graphe. Cette procédure d'exploration est ré-exécutée jusqu'à la découverte d'un chemin potentiellement accessible permettant à l'agent de se déplacer jusqu'à son but. 

Concernant l'approche utilisée pour l'exploration, on détermine d'abord les tuiles accessibles parmi ses voisins directs. À chaque déplacement, une structure permet de stocker la liste des tuiles explorées par un agent et le nombre de fois qu'il l'a visitée. La tuile cible est alors retenue en fonction du nombre de fois que l'agent a visité une tuile, en priorisant celles qui ont été les moins explorées.

Au cours du déplacement d'un agent, il se peut qu'un conflit survienne avec un autre agent. Il se pourrait que la tuile de destination le soit pour cet autre agent. Dans ces circonstances, la priorité est accordée au premier agent à qui cette tuile a été assignée comme destination. Le second devra attendre au moins un tour. Si sa destination est libérée au tour suivant, il pourra s'y rendre, sinon un nouveau chemin sera calculé pour qu'il puisse atteindre son but final - si un autre chemin est disponible. Dans le cas contraire, il repasse en mode d'exploration.


---------------------
3*/ LIMITES & AMÉLIORATIONS
---------------------

Notre implémentation du comportement des agents ne comporte ni machine à état finie, ni "behaviour tree". La maintenance et l'évolution du projet en sont donc grandement limitées dû au manque de flexibilité de l'architecture établie. Nous avons cependant tenté d'introduire un squelette de behaviour tree pour définir le comportement de nos agents. Nous avons rencontré quelques difficultés et le manque de temps ne nous a pas permis d'aboutir à du code fonctionnel. 

Au niveau de l'acquisition de données, ces dernières, relatives aux murs, sont peu fiables. En effet, certains murs ne sont pas présents dans le graphe, ce qui cause donc des problèmes pour les niveaux TC_030 et plus.