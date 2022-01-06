#define _CRT_SECURE_NO_WARNINGS
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include <cmath>
#define SWAP(a, b) { int t; t = a; a = b; b = t; }

void merge(int left, int mid, int right, double* X, unsigned* Xid, unsigned* TMP) {
	int i = left, j = mid + 1, k = left, l;
	//분할된 list 합침
	while (i <= mid && j <= right) {
		if (X[Xid[i]] <= X[Xid[j]]) TMP[k++] = Xid[i++];
		else TMP[k++] = Xid[j++];
	}
	//남은 값 복사
	if (i > mid) {
		for (l = j; l <= right; l++)
			TMP[k++] = Xid[l];
	}
	//남은 값 복사
	else {
		for (l = i; l <= mid; l++)
			TMP[k++] = Xid[l];
	}
	for (l = left; l <= right; l++)
		Xid[l] = TMP[l];
}


void mergesort(int left, int right, double* X, unsigned* Xid, unsigned* TMP) {
	int mid;
	if (left < right) {
		mid = (left + right) / 2;					//divide

		mergesort(left, mid, X, Xid, TMP);			//conquer
		mergesort(mid + 1, right, X, Xid, TMP);		//conquer
		
		merge(left, mid, right, X, Xid, TMP);		//combine
	}
}

void selectionSort(unsigned L, unsigned R, double* Y, unsigned* Yid) {
	unsigned min;
	for (int i = L; i <= R; i++) {
		min = i;
		for (int j = i + 1; j <= R; j++) {							
			if (Y[Yid[j]] < Y[Yid[min]])
				min = j;							//더 작은 값 찾으면 바꿈
		}
		SWAP(Yid[i], Yid[min]);
	}
}

void sortXid(double* X, unsigned* Xid, unsigned* TMP, unsigned N) {		// Xid[]를 정렬하는 함수
	mergesort(0, N - 1, X, Xid, TMP);
}

double combine(
	unsigned l, unsigned r, unsigned* pt1, unsigned* pt2,
	double* X, double* Y, unsigned* Xid, unsigned* Yid, unsigned* TMP,
	double d_LR, double d_min
) {
	int i = l, j, x = 0;
	unsigned m = (l + r) / 2;
	double x_mid = (X[Xid[m]] + X[Xid[m + 1]]) / 2, d;
	while (i <= r) {
		if (x_mid - d_LR <= X[Yid[i]] && X[Yid[i]] <= x_mid + d_LR) {
			TMP[x++] = Yid[i];
		}
		i++;
	}
	if (x != 0) {
		i = 0;
		while (i < x - 1) {
			j = i + 1;
			while (j < x){
				if (Y[TMP[j]] - Y[TMP[i]] >= d_min) break;

				double dis = sqrt(pow(X[TMP[i]] - X[TMP[j]], 2) + pow(Y[TMP[i]] - Y[TMP[j]], 2));

				if (dis < d_min) {
					d_min = dis;
					*pt1 = TMP[i];
					*pt2 = TMP[j];
				}
				j++;
			}
			i++;
		}
	}
	return d_min;														// 중간 영역까지 고려한 이후의 최소 거리 반환.
}


double closestPairDC(
	unsigned L, unsigned R,												// current leftmost and rightmost indices
	unsigned* pt1, unsigned* pt2,										// closest pair points indices
	double* X, double* Y,												// (x,y) positions array(input)
	unsigned* Xid,														// point index array(sorted by x coordinates)
	unsigned* Yid,														// point index array(gradually sorted by y coordinates)
	unsigned* TMP,														// temporal index array
	unsigned THR														// threshold value
) {
	double min = DBL_MAX;										//최소 거리 저장할 변수
	unsigned k, M = (L + R) / 2;
	int i, j;
	if (R - L + 1 <= THR) {
		i = L;
		while (i <= R) {								//Xid[L:R]을 Yid[L:R]로 복사
			Yid[i] = Xid[i];
			i++;
		}
		selectionSort(L, R, Y, Yid);					//O(n^2)인 selectionsort 사용해서 y좌표 오름차순 정렬

		i = L;
		while (i < R) {					//brute force 방법으로 계산
			j = i + 1;
			while (j <= R) {
				if (Y[Yid[j]] - Y[Yid[i]] >= min) break;

				double dis = sqrt(pow(X[Yid[i]] - X[Yid[j]], 2) + pow(Y[Yid[i]] - Y[Yid[j]], 2));

				if (dis < min) {						//더 작은 값 있으면 갱신
					min = dis;
					*pt1 = Yid[i];
					*pt2 = Yid[j];
				}
				j++;
			}
			i++;
		}
	}
	else {
		unsigned p1, p2, p3, p4;
		double d_L = closestPairDC(L, M, &p1, &p2 , X, Y, Xid, Yid, TMP, THR);
		double d_R = closestPairDC(M + 1, R, &p3, &p4, X, Y, Xid, Yid, TMP, THR);

		merge(L, M, R, Y, Yid, TMP);
		double d_LR;
		if (d_L <= d_R) {							//d_L이 d_R보다 작을 경우
			*pt1 = p1;
			*pt2 = p2;
			d_LR = d_L;
		}
		else {										//d_R이 d_L보다 작을 경우
			*pt1 = p3;
			*pt2 = p4;
			d_LR = d_R;
		}

		min = d_LR;
		unsigned pt_1, pt_2;

		double d_c = combine(L, R, &pt_1, &pt_2, X, Y, Xid, Yid, TMP, d_LR, min);
		if (d_c < d_LR) {							//중간 영역 최소거리가 더 작은 경우
			*pt1 = pt_1;
			*pt2 = pt_2;
			min = d_c;
		}
	}
	return min;										//최종 거리 return
}