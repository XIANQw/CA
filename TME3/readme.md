## Tme2 Compilation Avancée
### Auteur
- XIAN Qiwei

### Introduction
Dans ce TME, on va réaliser une machine universelle qui permet de decoder un fichier binaire et exécuter les codes.

### Structure
- src : le fichier .c contenant les codes sources
- data : les fichiers binaires à exécuter

### Installation
```
make
```

### Execution
- Exécuter le programme sandmark
```
make runSandmark
```
- Exécuter le programme codex
```
make runCodex
```

### Rapport de TME
#### Processus
La machine universelle peut correctement décoder le fichier binaire et appeler les méthodes en fonction de commande sans problème. Mais le méthode 12 `charger programme` n'est pas passé de test. Pour les détails de résultat, ils sont noté dans le fichier `outCodex` et `outSandmark`

#### Structure des données
- plateau
  `unsigned int`

- tableau de plateau : tableau de unsigned int, `unsigned int *`
- la collection des tableaux de plateau => le tableau des adresses de tableau de plateau.
`unsigned int * tableaux[NTAB]`
- L'identificateur de tableau est son indice dans la collection des tableaux.
La taille de collection est définie par `NTAB`

#### Optimization

J'ai aussi un tableau appelé `size_tab`, il conserve la capacité de chaque tableau. Il permet de vérifier si le tableau est disponible, lors que la méthode allocation est appelée, ainsi que si on a besoin de free ce tableau lors que la méthode abandon est appelée.

Pour la méthode charger le programme, si on ajouter une vérification pour éviter de dupliquer le contenu de tableau au lui même, il va gagner beaucoup de temps. Par exemple dupliquer `tableaux[0]` à `tableaux[0]`. On n'a pas besoin de le faire.
