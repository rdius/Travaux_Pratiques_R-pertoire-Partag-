////////////////////////////////////////////////////////////////////////
//		TP: Algorithme de Ford-Fulkerson
//	Binome: KAFANDO Rodrique       
//		BARRY   Mamadou Dian		
///////////////////////////////////////////////////////////////////////


#include<iostream>
#include <stdio.h>
#include <string>
using namespace std;

#define WHITE 0
#define GRAY 1
#define BLACK 2
#define MAX_NODES 1000
#define oo 1000000000

int n;  // Nombre de noeuds du graph
int e;  // Nombre d'arretes
int capacity[MAX_NODES][MAX_NODES]; // Capacité de la matrice noeud
int flow[MAX_NODES][MAX_NODES];     // Matrix contenant le flot
int color[MAX_NODES]; // Necessaire pour stcker le premier parcourt              
int pred[MAX_NODES];  // stockage du chemin d'augmentation

int min (int x, int y) {
    return x<y ? x : y;  // returne le min de x et y
}

int head,tail;
int q[MAX_NODES+2];

void enqueue (int x) {
    q[tail] = x;
    tail++;
    color[x] = GRAY;
}
int dequeue () {
    int x = q[head];
    head++;
    color[x] = BLACK;
    return x;
}


int bfs (int start, int target) {
    int u,v;
    for (u=0; u<n; u++) {
	color[u] = WHITE;
    }   
    head = tail = 0;
    enqueue(start);
    pred[start] = -1;
    while (head!=tail) {
	u = dequeue();
        // Recherche de tous les nœuds blancs adjacents v. Si la capacité
         // de u à v dans le réseau résiduel est positif, alors
         // enqueue v.
	for (v=0; v<n; v++) {
	    if (color[v]==WHITE && capacity[u][v]-flow[u][v]>0) {
		enqueue(v);
		pred[v] = u;
	    }
	}
    }
    // Si la couleur du noeud cible est noire maintenant,
     // cela signifie que nous l'avons atteint.
    return color[target]==BLACK;
}
int max_flow (int source, int sink) {
    int i,j,u;
    // Initialisation du flot
    int max_flow = 0;
    for (i=0; i<n; i++) {
	for (j=0; j<n; j++) {
	    flow[i][j] = 0;
	}
    }
	// S'il existe un chemin d'augmentation,
     // incrémenter le flux le long de ce chemin
    while (bfs(source,sink)) {
        // Déterminer l'arrete par laquelle nous pouvons incrémenter le flot
	int increment = oo;
	for (u=n-1; pred[u]>=0; u=pred[u]) {
	    increment = min(increment,capacity[pred[u]][u]-flow[pred[u]][u]);
	}
        // Incremente le flot
	for (u=n-1; pred[u]>=0; u=pred[u]) {
	    flow[pred[u]][u] += increment;
	    flow[u][pred[u]] -= increment;
	}
	max_flow += increment;
    }
    //Fin, plus de chemin possible pour augmenter le flot.
    return max_flow;
}
void read_input_file() {
    int a,b,c,i,j;
    FILE* input = fopen("text_matrice.txt","r");
    // Liecture du nombre de noeuds et d'arteres
    fscanf(input,"%d %d",&n,&e);
    // Matrix d'initialisation de la capacité 
    for (i=0; i<n; i++) {
	for (j=0; j<n; j++) {
	    capacity[i][j] = 0;
	}
    }
    // Lecture de la capacitée des arteres
    for (i=0; i<e; i++) {
	fscanf(input,"%d %d %d",&a,&b,&c);
	capacity[a][b] = c;
    }
    fclose(input);
}

int main () {
    read_input_file();
	//Afficher le flot max
    cout<< "Le flot max est " << max_flow(0,n-1);
	cout<< endl;
    return 0;
}

