#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int maxn = 10000 + 10;
int n, a[maxn];

void swap(int* x, int* y) {
	int temp = *x;
	*x = *y;
	*y = temp;
}

int Median3(int a[], int Left, int Right) {
	int Center = (Left + Right) / 2;

	if (a[Left] > a[Center])
		swap(&a[Left], &a[Center]);
	if (a[Left] > a[Right])
		swap(&a[Left], &a[Right]);
	if (a[Center] > a[Right])
		swap(&a[Center], &a[Right]);

	swap(&a[Center], &a[Right - 1]);
	//只需要对Left ~ Right-1 进行分割
	//将pivot换到待分割的队列的末尾
	return a[Right - 1];
}

void Quicksort(int a[], int Left, int Right) {
	if (Left >= Right) return;
	if (Right - Left + 1 == 2) {
		if (a[Left] > a[Right]) swap(&a[Left], &a[Right]);
		return;
	}
	int L = Left, R = Right;
	int Pivot = Median3(a, L, R);
	int i = Left, j = Right - 1;
	//只需要对Left+1 ~ Right-1进行分割
	//初始化i指向Left,j指向Right-1
	while (1) {
		while (a[++i] < Pivot) {}
		while (a[--j] > Pivot) {}
		if (i < j)swap(&a[i], &a[j]);
		else break;
	}
	swap(&a[i], &a[Right - 1]);
	Quicksort(a, Left, i - 1);
	Quicksort(a, i + 1, Right);
}

int main() {
	//freopen("./input/task1.1.in", "r", stdin);
	scanf("%d", &n);
	for (int i = 1; i <= n; i++)
		scanf("%d", &a[i]);
	Quicksort(a, 1, n);
	for (int i = 1; i <= n; i++)
		printf("%d\n", a[i]);
	return 0;
}