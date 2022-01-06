
#include "Graph_adj_array.h"

void Read_Graph_adj_array (int Vnum, vertex V[], int Enum, edge E[]) {
	// read graph information
	// V와 E의 name field는 각각 자신의 인덱스를 저장한다
	// V[].flag과 E[].flag은 반드시 false로 설정해야 한다.
	
	// ***이 함수를 작성한다
	int u, v, c;

	for (int i = 0; i < Vnum; i++) {
		V[i].name = i;
		V[i].flag = false;
		V[i].f_hd = V[i].r_hd = NONE;
	}

	for (int i = 0; i < Enum; i++) {
		scanf_s("%d%d%d", &u, &v, &c);

		E[i].name = i;
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

int DFS_Tree_adj_array(
	int     src,   // source node index
	int     Vnum,  // number of vertices
	vertex *V,     // vertex structure array (starting index 0)
	int     Enum,  // number of edges
	edge   *E     // edge structure array (starting index 0) 
) {  
	// DFS를 사용하여 DFS tree에 속하는 에지의 flag을 true로 설정한다.
	// DFS 시작 vertex는 입력 파일에서 지정된다(src).
	// DFS tree에 속한 에지의 cost 함을 return 한다(모든 각 edge cost가 1이면 unique)
	// recursive 함수로 작성해도 무방하다.

	// ***이 함수를 작성한다***
	int cost = 0;
	int e, i;
	V[src].flag = true;

	e = V[src].f_hd;
	for (i = V[src].f_hd; i != NONE; i = E[i].fp) {
		edge& f = E[e];
		if (!V[f.vr].flag) {
			f.flag = true;
			cost += f.cost + DFS_Tree_adj_array(f.vr, Vnum, V, Enum, E);
		}
		e = f.fp;
	}

	e = V[src].r_hd;
	for (i = V[src].r_hd; i != NONE; i = E[i].rp) {
		edge& r = E[e];
		if (!V[r.vf].flag) {
			r.flag = true;
			cost += r.cost + DFS_Tree_adj_array(r.vf, Vnum, V, Enum, E);
		}
		e = r.rp;
	}

	return cost;
}
