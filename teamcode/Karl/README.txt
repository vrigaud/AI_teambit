-------------------
INF781 - AIBot
[README]
-------------------
Karl-Étienne Perron
Alexandre Sendra
-------------------

---------------
Nombre d'heures
---------------
> 15h/semaine/équipe

-------------------------
Description de l'approche
-------------------------
Dans le cadre du laboratoire "Boot camp" du cours INF781, nous avons mis en place notre propre AIBot. Notre "MyBotLogic" cherche à répondre à l'essentiel des aspects MODEL et THINK du modèle SENSE-THINK-ACT. En effet, notre système de prise de décision utilise, à chaque tour et pour chaque "npc", les données fournies par le "turnInfo" afin de pouvoir interpréter l'environnement. Grâce à cette interprétation, il nous est alors possible de faire évoluer les npcs dans le temps grâce à différentes composantes.

En termes de l'aspect MODEL, la pièce maîtresse de notre "MyBotLogic" est le "Graph". Le "Graph" représente, au niveau de notre système, comment l'environnement est modélisé. Il conserve l'information en continue à propos des différents npcs, objets, cibles ainsi que les "tuiles" elles-mêmes. C'est à partir de lui que la logique des différentes composantes se basent lors de l'exécution d'un tour. Afin de déplacer les npcs, nous modélisons également des "MyBot" servant d'avatars logiques dans nos algorithmes. Ces derniers conservent des "Paths" afin que nous puissions tracer leur cheminement vers les différentes cibles de l'environnement, laissant l'impression que les npcs cherchent à les atteindre de manière optimale. En ce qui concerne les tuiles dans le "Graph", nous les modélisons grâce à nos "Nodes" (portant également la signification de "noeud" dans un graphe). Ces "Nodes" sont utiles pour conserver l'information sur une tuile tels que l'identifiant et les objets leur étant associés. Ils sont également composés de "MyNeighbours", une structure conservant le lien positionnel logique entre la tuile et ses tuiles voisines. D'ailleurs, un déplacement d'une tuile vers sa voisine est un "FromTo", un petit "struct" offrant le coût de la transition entre ces dernières.

Du côté THINK, chez une instance de "MyBot", les étapes dans le processus de prise de décision sont représentées par différents états. Nous faisons évoluer ces états selon un modèle de "machine à états finis". En fonction de l'information disponible dans le "Graph" ainsi que chez les autres "npcs", le "MyBot" change d'état selon des transitions précises. Par exemple, un "MyBot" ne remplissant pas la condition comme quoi il possède un chemin vers une cible précise dans la modélisation de l'environnement causera une "transition" vers un état de "recherche de chemin vers une cible". Plus particulièrement, dans le cas où une cible est représentée comme étant "visible" chez un "MyBot, le modèle de recherche de chemin principal est présentement implémenté selon l'algorithme A*. Lors de parties multi-agents, il est possible,dans la "machine à état", d'utiliser l'information des différents "MyBots" les représentant afin d'offrir une impression de coordination entre les "npcs" dans l'environnement. Par exemple, lorsque nous affectons une même tuile cible chez plusieurs "MyBot", une transition permet de modifier l'état de l'un ou l'autre des "MyBot" afin de forcer l'obtention d'une second tuile cible. Lors de la recherche de chemin vers une cible, il est possible d'effectuer, pour un même "MyBot", une recherche de chemin pour plus d'une tuile cible. Or, à des fins d'optimisation, au lieu d'effectuer cette recherche de manière séquentielle, nous profitons du mécanisme de "multithreading" afin d'effectuer la tâche en parallèle. Ainsi, la recherche de chemin pour N tuiles cibles correspond, environ, au temps requis pour une seule et unique tuile cible. Une telle optimisation aurait été envisageable pour l'évaluation des états des différents "MyBot", mais la complexité d'un système de communication (tel que celui décrit ci-dessus) parallèle nous a poussé à écarter cette possibilité. Dans le cas où l'environnement n'est pas pleinement représenté au travers des données fournies, les "npcs" seront interprétés comme ayant une vision partielle de l'envrionnement. À ce moment, il est possible que les "MyBot" ne puisse acquérir de chemin vers une tuile cible à défaut d'avoir une tuile catégorisée de la sorte dans le "turnInfo". Dans cette situation, le "npc" se retrouve obligé "d'explorer" jusqu'à la perception d'une tuile cible. Pour cela, nous avons opté pour une approche au hasard. En effet, ne possédant l'information nécessaire pour déterminer vers où se trouverait une éventuelle tuile cible, toutes les directions de déplacement sont bonnes à explorer. Toutefois, à partir du moment où certaines tuiles ont déjà été visités, nous pouvons restreindre le domaine de notre hasard. Par exemple, suite au déplacement d'un "npc" vers une tuile se "situant à l'est", nous pouvons réduire la probabilité de retourner par hasard vers la tuile logiquement à l'ouest puisque cela ne permettrait pas l'acquisition de nouvelles informations.

Ainsi, la mise en relation et l'interdépendance de ces différentes composantes nous permet de faire évoluer de manière plutôt convaincante les différents "npcs" dans l'envrionnement fournie dans le laboratoire. La composante "hasard" dans les environnements décrits avec des informations partielles, bien que légèrement dirigée, laisse toutefois place à des comportements parfois peu désirables. Parfois, un "npc" finira par explorer à nouveau des zones déjà visitées. Aussi, notre "système de vision" reste limité et peu optimisé, limitant l'information que nous pouvons déduire à distance de plus d'une tuile. Il y a donc place à amélioration dans notre système de prise de décision et notre système de "vision". En effet, nous n'avons toujours pas de carte d'influence (pas de véritable du moins) qui nous permettrait un déplacement conséquent des "npcs", et ce, même dans un environnement à information limitée. Nous devrions également raffiner notre système de vision afin d'obtenir plus d'informations à partir des données disponibles.


------------------------
Déroulement d'une partie
------------------------
> La partie débute;
> Nous construisons le "Graph" à partir des dimensions fournies dans le "levelInfo";
> Nous commençons un tour;
> Nous mettons à jour le "Graph" (cibles, objets et autres...) grâce aux données du "turnInfo";
> Nous lançons l'évaluation de l'état des différents "MyBot" grâce à un modèle de machine à états finis;
>En fonction de l'état:
>> Si le "Graph" possède de l'information sur des tuiles cibles, les "MyBot" passeront dans un état de recherche de chemin vers celles-ci (A*);
>> Nous utilisons l'information des "MyBot" afin de gérer les chemins conflictuels entre ces derniers;
>> Si un "MyBot" est assuré de poursuivre un chemin vers la tuile cible, il ajoute à l'"actionList", son déplacement vers la tuile correspondante;
>> Si un "MyBot" ne peut pas poursuivre un chemin vers une tuile cible, il recherche un chemin vers une autre tuile cible. En cas d'échec, il ajoute à l'"actionList", un déplacement semi-aléatoire (influencé par les tuiles déjà visitées);
>> Si un "MyBot" a atteint sa tuile cible, il ne se déplace plus, ou bien, se déplace seulement pour laisser la place à un "MyBot" mieux placé que lui;
> Le tour se termine quant l'état de tous les "MyBot" s'est stabilisé;
> Nous passons au prochain tour;