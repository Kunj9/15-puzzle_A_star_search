#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 4
#define NxN (N*N)
#define TRUE 1
#define FALSE 0

struct state {
	int tiles[N][N];
	int f, g, h;
	short zero_row, zero_column;	/* location (row and colum) of blank tile 0 */
	struct state *next;
	struct state *parent;			/* used to trace back the solution */
};

int goal_rows[NxN];
int goal_columns[NxN];
int succ_states_size;
struct state *start,*goal;
struct state *fringe = NULL, *closed = NULL;
struct state *succ_states[4];



void print_a_state(struct state *pstate) {
	int i,j;
	for (i=0;i<N;i++) {
		for (j=0;j<N;j++) 
			printf("%2d ", pstate->tiles[i][j]);
		printf("\n");
	}
	printf("\n");
}

void printList(struct state* n){
    int i = 0;
    while(n!=NULL) {
        print_a_state(n);
        n=n->next;
    }
}

struct state *initialize(char **argv){
	int i,j,k,index, tile;
	struct state *pstate;

	pstate=(struct state *) malloc(sizeof(struct state));
	index = 1;
	for (j=0;j<N;j++)
		for (k=0;k<N;k++) {
			tile=atoi(argv[index++]);
			pstate->tiles[j][k]=tile;
			if(tile==0) {
				pstate->zero_row=j;
				pstate->zero_column=k;
			}
		}
	pstate->f=0;
	pstate->g=0;
	pstate->h=0;
	pstate->next=NULL;
	pstate->parent=NULL;
	start=pstate;
	printf("initial state\n");
	print_a_state(start);

	pstate=(struct state *) malloc(sizeof(struct state));
	goal_rows[0]=3;
	goal_columns[0]=3;

	for(index=1; index<NxN; index++){
		j=(index-1)/N;
		k=(index-1)%N;
		goal_rows[index]=j;
		goal_columns[index]=k;
		pstate->tiles[j][k]=index;
	}
	pstate->tiles[N-1][N-1]=0;	      /* empty tile=0 */
	pstate->f=0;
	pstate->g=0;
	pstate->h=0;
	pstate->next=NULL;
	goal=pstate; 
	printf("goal state\n");
	print_a_state(goal);

	return start;
}

void swaplist(struct state ** dest, struct state ** source){
    struct state* newstate = *source;
    if(source == NULL){
        return;
    }
    
    *source = newstate->next;
    newstate->next = *dest;
    *dest = newstate;
}

/* merge the remaining states pointed by succ_states[] into fringe list. 
 * Insert states based on their f values --- keep f values sorted. */
void merge_to_fringe() {
    struct state * keystate;
    int i, j;
    
    //sort the array
    for (i = 0; i < succ_states_size; i++) {
        keystate = succ_states[i];
        j = i - 1;
 
        /* Move elements of arr[0..i-1], that are
          greater than key, to one position ahead
          of their current position */
        while (j >= 0 && succ_states[j]->f > keystate->f) {
            succ_states[j + 1] = succ_states[j];
            j = j - 1;
        }
        succ_states[j + 1] = keystate;
    }
    
    //turn array into list
    struct state *arrhead=NULL, *arrtail=NULL, *pstate;
    
    for (int i=0;i<succ_states_size;i++){
        pstate=(struct state*)malloc(sizeof(struct state));
        pstate=succ_states[i];
        pstate->next=NULL; /*ensure next pointer of last state is NULL */
        if(arrhead==NULL){
            arrhead=pstate; //if head is null, current entry becomes new head
        } 
        if(arrtail!=NULL){
            arrtail->next=pstate; //if tail is not null, tail's next pointer becomes current entry
        }
        arrtail=pstate; //new tail is current entry
    }
    
    //merge the sorted lists
    struct state dummy;
    struct state * tail = &dummy, *opit = fringe;
    dummy.next = NULL;
    
    while(1){
        if(arrhead == NULL){
            tail->next = opit;
            break;
        }
        else if (opit == NULL){
            tail->next = arrhead;
            break;
        }
        if(arrhead->f < opit->f){
            swaplist(&(tail->next),&arrhead);
        }
        else{
            swaplist(&(tail->next),&opit);
        }
        
        tail = tail->next;
    }
    
    //set the new merged list as the new fringe
    fringe = dummy.next;
}

/*swap two tiles in a state*/
void swap(int row1,int column1,int row2,int column2, struct state * pstate){
	int tile = pstate->tiles[row1][column1];
	pstate->tiles[row1][column1]=pstate->tiles[row2][column2];
	pstate->tiles[row2][column2]=tile;
}

/*update the f,g,h function values for a state */
void update_fgh(int i) {
	struct state *pstate = succ_states[i];
	int goalrow, goalcol, succrow, succcol, start, dest, htot=0;
    if(pstate==NULL){
        return;
    }
	//cost to travel increases by 1
	pstate->g++;
	
	//calculate h
	//for each row and col of goal
	for (goalrow=0;goalrow<N;goalrow++){
		for (goalcol=0;goalcol<N;goalcol++) {
		    
			//for each row and col of succ item
			dest = goal->tiles[goalrow][goalcol];
			if(dest == 0) goto end;
			for (succrow=0;succrow<N;succrow++){
			    for (succcol=0;succcol<N;succcol++){
			        
			        start = pstate->tiles[succrow][succcol];
			        if(start==dest){
			            //add the difference
			            pstate->h+=abs(succrow-goalrow)+abs(succcol-goalcol);
			            //go to next item
			            goto nextitem;
			        }
			    }//end succcol
			}//end succrow
			nextitem:;
		}// end goalcol
	}//end goalrow
	end:;
	pstate->f=pstate->g+pstate->h;
}

/* 0 goes down by a row */
void move_down(struct state * pstate){
	swap(pstate->zero_row, pstate->zero_column, pstate->zero_row+1, pstate->zero_column, pstate); 
	pstate->zero_row++;
}

/* 0 goes right by a column */
void move_right(struct state * pstate){
	swap(pstate->zero_row, pstate->zero_column, pstate->zero_row, pstate->zero_column+1, pstate); 
	pstate->zero_column++;
}

/* 0 goes up by a row */
void move_up(struct state * pstate){
	swap(pstate->zero_row, pstate->zero_column, pstate->zero_row-1, pstate->zero_column, pstate); 
	pstate->zero_row--;
}

/* 0 goes left by a column */
void move_left(struct state * pstate){
	swap(pstate->zero_row, pstate->zero_column, pstate->zero_row, pstate->zero_column-1, pstate); 
	pstate->zero_column--;
}

/* expand a state, get its children states, and organize the children states using
 * array succ_states.
 */
void expand(struct state *selected) {
    //for each of the cardinal directions
    for(int i = 0; i<N; i++){
        //initialize succ_states then copy struct from mem area selected to mem area succ_states[i]
        succ_states[i] = (struct state *) malloc(sizeof(struct state));
        memcpy(succ_states[i], selected, sizeof(struct state));
        //get the state for each direction
        switch(i){
            case 0:
                if(succ_states[i]->zero_row>0){
                    move_up(succ_states[i]);
                }
                break;
            
            case 1:
                if(succ_states[i]->zero_row<N-1){
                    move_down(succ_states[i]);
                } 
                break;
                
            case 2:
                if(succ_states[i]->zero_column>0){
                    move_left(succ_states[i]);
                } 
                break;
                
            case 3:
                if(succ_states[i]->zero_column<N-1){
                    move_right(succ_states[i]);
                } 
                break;
        }
        succ_states[i]->parent=selected;
        //printf("\nNew direction state %d\n", i);
        //print_a_state(succ_states[i]);
    }
    succ_states_size = 4;
}

int states_same(struct state *a,struct state *b) {
	int flg=FALSE;
	if (memcmp(a->tiles, b->tiles, sizeof(int)*NxN) == 0)
		flg=TRUE;
	return flg;
}

/* Filtering:
 * check the state pointed by succ_states[i] to determine whether this state is repeating.
 * free the state if it is repeating. 
 */ 
void filter(int i, struct state *pstate_list){ 
	struct state *pstate = succ_states[i];
    if(pstate==NULL){
        return;
    }
	//if the tiles in succ_states and pstate_list are the same;
	while(pstate_list != NULL){
	    if(states_same(pstate, pstate_list)==TRUE){
	        free(succ_states[i]);
	        succ_states[i]=NULL;
	        for(int c = i; c < N - 1; c++){
	            succ_states[c] = succ_states[c+1];
	        }
	        succ_states_size--;
	        break;
	    }
	    pstate_list=pstate_list->next;
	}
}

int main(int argc,char **argv) {
	int iter;
	struct state *curr_state, *solution_path;
	int ret, i, pathlen=0, index[N-1];

	solution_path=NULL;
	start=initialize(argv);	
	fringe=start; //insert start state into fringe

	iter=0;

	while (fringe!=NULL) {	
		curr_state=fringe;
		fringe=fringe->next;  /* get the first state from fringe to expand */

		if(states_same(curr_state,goal)){ /* goal is found */
			do{ /* trace back and add the states on the path to a list */
				curr_state->next=solution_path;
				solution_path=curr_state;
				curr_state=curr_state->parent;
				pathlen++;
			} while(curr_state!=NULL);
			printf("Path (length=%d):\n", pathlen); 
			curr_state=solution_path;
			printList(curr_state);
			//print_a_state(curr_state);/* print out the states on the list */
			break;
		}
		expand(curr_state);       /* Find new successors */

		for(i=0;i<succ_states_size;i++){
			filter(i,fringe);
			filter(i,closed);
			update_fgh(i);
		}

		
		merge_to_fringe(); /* New fringe list */
		curr_state->next=closed;
		closed=curr_state;		/* New closed */
		
		iter++;
		if(iter %1000 == 0)
			printf("iter %d\n", iter);
	}
	return 0;
} /* end of main */

