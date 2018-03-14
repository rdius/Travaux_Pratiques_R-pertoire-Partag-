import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.List;
import java.util.Queue;


class DIGJUMP {
 
 
    public static void main(String args[]) throws IOException {
        BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
        StringBuilder minJump = new StringBuilder();
 
        char[] sequence = in.readLine().toCharArray();
 
        int length = sequence.length;
  
        //List contient la liste d'adjonction
        List<ArrayList<Integer>> graphe = new ArrayList<ArrayList<Integer>>(10); 
        //création d'une liste d'adjonction par valeur
        for(int i = 0 ; i < 10 ; i++)  
        {
            graphe.add(new ArrayList<Integer>());
        }
        //liste d'ajouts 
        for(int i = 0 ; i < length ; i++) 
        {
       	//recuperation  et ajout de la position de chaque valeur de la séquence dans la graphe
        	graphe.get(sequence[i]-'0').add(i); 
        }
        //List des queues, le nombre de queue max à visiter est le double de la taille de la sequence
        Queue<Integer> q = new ArrayDeque<Integer>(length*2); 
        //un noeud est soit visité ou non
        boolean[] noeudVisite = new boolean[length];     
        boolean[] red = new boolean[10];
        noeudVisite[0] = true; //on initialise le premier element de la sequence à true
        //initialisation des valeurs
        q.add(0); 
        q.add(0);
        int moves = 0;
 
        //Parcours dfs
        while(!q.isEmpty())
        {
        	//vérifier le sommet
            int u = q.remove();   
            moves = q.remove();
            int v = sequence[u]-'0';
            //verification valeur initiale et valeur finale
            if(u == length-1) 
            {
                minJump.append(moves).append("\n");
                break;
            }
            moves++;
          //si le sommet adjacent est valide et non visité
            if(u > 0 && !noeudVisite[u-1])  
            {
            	//marquer comme visité
                noeudVisite[u-1] = true; 
                q.add(u-1);     //ajouter la valeur adjacente à la queue
                q.add(moves);
            }
            //si le sommet adjacent est valide et non visité
            if(u < length-1 && !noeudVisite[u+1])  
            {
            	//marquer comme visité
                noeudVisite[u+1] = true;    
                q.add(u+1);          
                q.add(moves);
            }
            
            if(!red[v])
            {
                red[v] = true;
                //passer la liste d'adjacence des nœud
                for( int visitlist: graphe.get(v)) 
                {
                	//si un noeud est deja visité, il n'est pas prise en compte
                    if(!noeudVisite[visitlist]) 
                    {
                        noeudVisite[visitlist] = true;
                        q.add(visitlist);
                        q.add(moves);
                    }
 
                }
            }
        }
    System.out.print(minJump);
    }
}


