#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <queue>
#include <algorithm>
#include <climits>
#define NONE -1
#define SWAP(a, b) {int t; t=a; a=b; b=t;} //swap macro

typedef struct elm_edge {
	int  vf, vr;  // e = (vf, vr) where vf, vr are vertex array indices
	int  cost;    // edge cost
	bool flag;    // true if the edge is in the SP spanning tree, false otherwise 
	int  fp, rp;  // adj list ptr of forward and reverse (-1 if none)
} edge;

typedef struct elm_vertex {
	int  f_hd, r_hd;	// adj list header(-1 if none)
	int  distance;		// distance from src to this vertex
	int  heap_idx;		// minHeap index
	bool inS;			// true if SP length found(true when move from V-S to S) 
} vertex;

void insertion(vertex* V, int v, int* min, int* len) {
	min[++(*len)] = v;														//insert at the end
	V[v].heap_idx = (*len);													//allocate index

	int child = (*len);
	int parent = (child >> 1);

	while (child > 1 && V[min[parent]].distance > V[min[child]].distance) {
		SWAP(min[parent], min[child]);										//swap child and parent node
		SWAP(V[min[parent]].heap_idx, V[min[child]].heap_idx);				//swap child and parent node's index

		child = parent;
		parent = (child >> 1);
	}
}

int deletion(vertex* V, int* min, int* len) {
	int val = min[1];														//value to return
	V[val].heap_idx = NONE;

	SWAP(min[1], min[*len]);												//move minimum to the end
	min[(*len)--] = -1;														//minHeap[] cross reference. 

	int parent = 1;
	int child = (parent << 1);

	if (child + 1 <= *len) child = (V[min[child]].distance >= V[min[child + 1]].distance) ? child + 1 : child;
	
	//compare parent and vertex node
	while (child <= *len && V[min[parent]].distance > V[min[child]].distance) {
		SWAP(min[parent], min[child]);										//swap child and parent node
		SWAP(V[min[parent]].heap_idx, V[min[child]].heap_idx);				//swap child and parent node's index

		parent = child;
		child = (child << 1);

		if (child + 1 <= *len) child = (V[min[child]].distance >= V[min[child + 1]].distance) ? child + 1 : child;
	}

	return val;																//return original first node
}

void move_up(int val, vertex* V, int* min, int* len) {
	int child = V[val].heap_idx;
	int parent = (child >> 1);

	while (child > 1 && V[min[parent]].distance > V[min[child]].distance) {
		SWAP(min[parent], min[child]);										//swap child and parent node
		SWAP(V[min[parent]].heap_idx, V[min[child]].heap_idx);				//swap child and parent node's index

		child = parent;
		parent = (child >> 1);
	}
}

int back_trace(int src, int Vnum, const vertex* V, edge* E) {
	int e, cost = 0, u = 0, w;
	int max, idx;

	//Iterate through all vertices
	while (u < Vnum) {
		if (u == src) {
			u++;
			continue;
		}
		max = INT_MIN;

		//Iterate through front edges (u, w)
		for (e = V[u].f_hd; e != NONE; e = E[e].fp) {
			if (V[u].distance == V[E[e].vr].distance + E[e].cost) {
				if (V[E[e].vr].distance > max) {
					max = V[E[e].vr].distance;
					idx = e;
				}
			}
		}

		//Iterate through rear edges (u, w)
		for (e = V[u].r_hd; e != NONE; e = E[e].rp) {
			if (V[u].distance == V[E[e].vf].distance + E[e].cost) {
				if (V[E[e].vf].distance > max) {
					max = V[E[e].vf].distance;
					idx = e;
				}
			}
		}

		//Include the edge in SPT and calculate the cost
		E[idx].flag = true;
		cost += E[idx].cost;
		u++;
	}

	return cost;
}

int SPT_Dijkstra(
	int src,	// source vertex index
	// graph structure array
	// 1. the adjacency list structure is the same as PHW02
	// 2. additional fields are added for Dijkstra's algorithm(see .h file)
	int Vnum, vertex *V,	// Vertex array size and the array
	int Enum, edge *E,		// Edge array size and the array

	int *minHeap	// array for min heap (array size = Vnum+1)
		// heap index range is 1 ~ (Vnum - 1) note: src must not in the initial heap
		// just arry is passed. must construct min-heap in this function

	// OUTPUT
	// 1. V[].distance : shortest path length from src to this vertex
	// 2. shortest path spanning tree : set E[].flag = true if the edge is in the tree
	// 3. return the sum of edge costs in the shortest path spanning tree.
	//    should be small as possible (think a way to make it small)
) {
	int treeCost = 0;
	// *** 이 함수를 작성하자 ***
	// 반드시 min-heap을 사용하여 O((n+m)logn) 알고리즘을 구현해야 한다(아니면 trivial한 프로그램임)
	// heap 연산 등 필요한 함수는 자유롭게 작성하여 추가한다.
	// 그러나 global 변수, dynamic array 등은 추가로 사용하지 않는다(실제로 필요 없다)
	int len = 0;														//number of minimum heap nodes
	int i, idx;

	//initialize distance
	i = 0;
	while (i < Vnum) {
		V[i].distance = INT_MAX;
		i++;
	}

	//initialize src
	V[src].inS = true;
	V[src].distance = 0;

	//iterate through front edges
	idx = V[src].f_hd;
	while (idx != NONE) {
		V[E[idx].vr].distance = E[idx].cost;
		insertion(V, E[idx].vr, minHeap, &len);
		idx = E[idx].fp;
	}

	//iterate through rear edges
	idx = V[src].r_hd;
	while (idx != NONE) {
		V[E[idx].vf].distance = E[idx].cost;
		insertion(V, E[idx].vf, minHeap, &len);
		idx = E[idx].rp;
	}

	//insert vertex to minimum heap
	i = 0;
	while (i < Vnum) {
		if (i != src && V[i].distance == INT_MAX) insertion(V, i, minHeap, &len);
		i++;
	}

	/* ===== (O(n+m)logn) Algorithm ===== */
	//iterate through all vertices
	for (i = 0; i < Vnum - 2; i++) {									
		int u;
		u = deletion(V, minHeap, &len);
		V[u].inS = true;

		//iterate through front edges
		idx = V[u].f_hd;
		while (idx != NONE) {
			if (!V[E[idx].vr].inS && V[u].distance + E[idx].cost < V[E[idx].vr].distance) {
				V[E[idx].vr].distance = V[u].distance + E[idx].cost;
				move_up(E[idx].vr, V, minHeap, &len);
			}
			idx = E[idx].fp;
		}

		//iterate through rear edges
		idx = V[u].r_hd;
		while (idx != NONE) {
			if (!V[E[idx].vf].inS && V[u].distance + E[idx].cost < V[E[idx].vf].distance) {
				V[E[idx].vf].distance = V[u].distance + E[idx].cost;
				move_up(E[idx].vf, V, minHeap, &len);
			}
			idx = E[idx].rp;
		}

	}

	return (treeCost = back_trace(src, Vnum, V, E));
}



void Read_Graph(int Vnum, vertex *V, int Enum, edge *E) {
	// Graph 자료구조를 만드는 함수
	// *** 이 함수를 추가하자 ***
	// PHW02의 Read_Graph_adj_array()를 이 과제의 자료구조에 맞춰 살짝 수정하여 사용한다
	// 즉, Read_Graph_adj_array()에서 불필요한 내용을 제거한 후 사용.
	int u, v, c;

	for (int i = 0; i < Vnum; i++) {		//store vertex information
		V[i].f_hd = V[i].r_hd = NONE;
		V[i].distance = INT_MAX;
		V[i].heap_idx = NONE;
		V[i].inS = false;
	}

	for (int i = 0; i < Enum; i++) {		//store edge information
		scanf_s("%d%d%d", &u, &v, &c);

		E[i].vf = u;
		E[i].vr = v;
		E[i].cost = c;
		E[i].flag = false;

		E[i].fp = V[u].f_hd;
		E[i].rp = V[v].r_hd;

		V[u].f_hd = i;
		V[v].r_hd = i;
	}
}

// the following functions are for testing if the submitted program is correct.
int  Tree_Check(int Vnum, vertex *V, int Enum, edge *E, int *visited);
bool SPT_test(int src, int Vnum, vertex *V, int Enum, edge *E, int *minHeap);
void Error_Exit(const char *s);

int main ( void ) {
	int		src;
	vertex *V;		int Vnum;
	edge   *E;		int Enum;
	int    *minHeap;
	int    Tree_cost;
	int    Tnum; 		// # of test cases
	clock_t start, finish;
	double cmpt;

	scanf_s("%d", &Tnum);		// read # of tests

	for (int t = 0; t < Tnum; t++ ) {
		scanf_s("%d %d %d", &Vnum, &Enum, &src);
		V = new vertex [Vnum];
		E = new edge [Enum];
		minHeap = new int[Vnum + 1];	// heap array allocation
		if ( V == NULL || E == NULL || minHeap == NULL ) {
			Error_Exit("Memory Allocation Error");
		}
		Read_Graph(Vnum, V, Enum, E);

		/**/start = clock();	// start timer

		Tree_cost = SPT_Dijkstra(src, Vnum, V, Enum, E, minHeap);	// code by students

		/**/finish = clock();	// stop timer
		cmpt = ((double)(finish - start)) / (double)CLK_TCK;

		// 아래 Tree_Check와 SPT_test 함수는 첨부한 SPT_test.obj 파일에 있다.
		// 이 테스트에서 오류로 인하여 프로그램이 정지하면 뭔가 잘못된 것이다(제출해도 0점)
        if (Tree_Check(Vnum, V, Enum, E, minHeap) == 0) {
			Error_Exit("   ERROR The result is not a spanning tree");
		}
		if (SPT_test(src, Vnum, V, Enum, E, minHeap) == false) {
			Error_Exit("** Something wrong in applying Dijkstra's");
		}
		if ( t != 0 ) 
			printf("\n");

		printf("**T%d (Dijkstra) (V = %d, E = %d, time = %.3f sec) Tree Cost = %d\n", 
			t+1, Vnum, Enum, cmpt, Tree_cost);

		delete [] minHeap; delete [] V; delete [] E;
	}
	return 0;
}

void Error_Exit (const char *s ) {
  printf("%s\n", s);
  exit(-1);
}
