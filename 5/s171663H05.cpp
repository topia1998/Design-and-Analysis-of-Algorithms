#include <string.h>

#define MIN(x,y) (x)<(y)?(x):(y)						//macro to return minimum value between 2 integers
#define MIN3(x, y, z) (MIN(x, y))<(z)?(MIN(x, y)):(z)	//macro to return minimum value between 3 integers

void Edit_Distance(
	// inputs
	char* SS,									// Source string array
	char* TS,									// Edited string array
	int ins_cost, int del_cost, int sub_cost,	// insertion, deletion, substitution cost
	// outputs
	int** Table,								// Cost Table Calculated (2-d array (|SS|+1) x (|TS|+1) will be set)
	char** SR,									// Source string('*' implies insertion of a char to T)
	char** OP,									// Operation sequence('i', 'd', 's', or '.' (nop))
	char** TR,									// Edited string('*' implies deletion of char from S)
	int* Mem_Allocated							// allocation한 SR, OR, TR의 크기 합을 저장하여 메인에 전달
) {
	int m = strlen(SS), n = strlen(TS);			//m : length of SS, n = length of TS
	int i, j, alpha;

	//Table calculation by bottom-up
	for (i = 0; i <= m; i++) {
		Table[i][0] = i * del_cost;
	}
	for (j = 0; j <= n; j++) {
		Table[0][j] = j * ins_cost;
	}

	//complete the rest Table
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			alpha = (SS[i - 1] == TS[j - 1]) ? 0 : sub_cost;	//if SS[i - 1] == TS[j - 1] we add nothing but if not we have to add sub_cost
			Table[i][j] = MIN3(Table[i][j - 1] + ins_cost, Table[i - 1][j] + del_cost, Table[i - 1][j - 1] + alpha);	//By using min function, store minimum value to Table
		}
	}

	int op_cnt = 0;
	i = m, j = n;

	while (i > 0 || j > 0) {					//finding the operations by top-down
		//no operation or substitution
		alpha = (SS[i - 1] == TS[j - 1]) ? 0 : sub_cost;

		if ((j > 0 && (Table[i][j] == Table[i][j - 1] + ins_cost)) || i == 0) j--;			//insertion
		else if ((i > 0 && (Table[i][j] == Table[i - 1][j] + del_cost)) || j == 0) i--;		//deletion
		else if (Table[i][j] == Table[i - 1][j - 1] + alpha) {								//substitution or nop
			i--; j--;
		}
		op_cnt++;
	}

	int len = op_cnt + 1;

	//memories to store result
	*SR = new char[len];
	*OP = new char[len];
	*TR = new char[len];
	*Mem_Allocated = 3 * sizeof(char) * (len);

	//put null at the end of memories
	(*SR)[op_cnt] = (*TR)[op_cnt] = (*OP)[op_cnt] = '\0';

	int index = op_cnt - 1;
	i = m, j = n;

	while (i > 0 && j > 0) {
		alpha = (SS[i - 1] == TS[j - 1]) ? 0 : sub_cost;
		if (Table[i][j] == Table[i][j - 1] + ins_cost) {
			//If insertion and deletion are both bossible and ins_cost > del_cost, then deletion occurs
			if ((Table[i][j - 1] + ins_cost == Table[i - 1][j] + del_cost) && (ins_cost > del_cost)) {	//Deletion
				(*OP)[index] = 'd';
				(*SR)[index] = SS[--i];
				(*TR)[index] = '*';
			}
			else {		//Insertion
				(*OP)[index] = 'i';
				(*SR)[index] = '*';
				(*TR)[index] = TS[--j];
			}
		}
		else if (Table[i][j] == Table[i - 1][j] + del_cost) {	// Deletion
			(*OP)[index] = 'd';
			(*SR)[index] = SS[--i];
			(*TR)[index] = '*';
		}
		else if (Table[i][j] == Table[i - 1][j - 1] + alpha) {			// Substitution or Nop
			(*OP)[index] = (SS[i - 1] == TS[j - 1] ? '.' : 's');
			(*SR)[index] = SS[--i];
			(*TR)[index] = TS[--j];
		}
		index--;
	}

	//Back trace rest insertion
	while (j > 0 || i > 0) {
		if (j > 0) {
			(*OP)[index] = 'i';
			(*SR)[index] = '*';
			(*TR)[index] = TS[--j];
			index--;
		}
		else if (i > 0) {
			(*OP)[index] = 'd';
			(*SR)[index] = SS[--i];
			(*TR)[index] = '*';
			index--;
		}
	}
}