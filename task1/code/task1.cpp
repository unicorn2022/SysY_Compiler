#pragma warning(disable:4996)
#include <stdio.h>

const int maxn = 10000 + 10;
int n, a[maxn];

int ReadInt() {
	int ch = getchar();
	while ((ch > '9' || ch < '0') && ch != '-') {
		ch = getchar();
	}
	int ans = 0;
	int flag = 1;
	if (ch == '-') {
		flag = -1;
		ch = getchar();
	}
	while (ch >= '0' && ch <= '9') {
		ans = ans * 10 + ch - '0';
		ch = getchar();
	}
	return ans * flag;
}

void PrintInt(int x) {
	if (x < 0) {
		putchar('-');
		x = -x;
	}
	if (x >= 10) {
		PrintInt(x / 10);
	}
	putchar(x % 10 + '0');
}

void swap(int* x, int* y) {
	int temp = *x;
	*x = *y;
	*y = temp;
}

int Median3(int a[], int Left, int Right) {
	int Center = (Left + Right) / 2;

	if (a[Left] > a[Center]) {
		swap(&a[Left], &a[Center]);
	}
	if (a[Left] > a[Right]) {
		swap(&a[Left], &a[Right]);
	}
	if (a[Center] > a[Right]) {
		swap(&a[Center], &a[Right]);
	}

	swap(&a[Center], &a[Right - 1]);
	//只需要对Left ~ Right-1 进行分割
	//将pivot换到待分割的队列的末尾
	return a[Right - 1];
}

void Quicksort(int a[], int Left, int Right) {
	if (Left >= Right) {
		return;
	}
	if (Right - Left + 1 == 2) {
		if (a[Left] > a[Right]) swap(&a[Left], &a[Right]);
		return;
	}
	int L = Left;
	int R = Right;
	int Pivot = Median3(a, L, R);
	int i = Left;
	int j = Right - 1;
	//只需要对Left+1 ~ Right-1进行分割
	//初始化i指向Left,j指向Right-1
	while (1) {
		i = i + 1;
		while (a[i] < Pivot) { i = i + 1; }
		j = j - 1;
		while (a[j] > Pivot) { j = j - 1; }
		if (i < j) {
			swap(&a[i], &a[j]);
		}
		else {
			break;
		}
	}
	swap(&a[i], &a[Right - 1]);
	Quicksort(a, Left, i - 1);
	Quicksort(a, i + 1, Right);
}

int main() {
	//freopen("./input/task1.1.in", "r", stdin);
	n = ReadInt();
	//scanf("%d", &n);
	int i = 1;
	while(i <= n) {
		a[i] = ReadInt();
		i = i + 1;
		//scanf("%d", &a[i]);
		//printf("a[%d] = %d\n", i, a[i]);
	}
	Quicksort(a, 1, n);
	i = 1;
	while (i <= n) {
		PrintInt(a[i]);
		i = i + 1;
		putchar('\n');
		//printf("a[%d] = %d\n", i, a[i]);
	}
	return 0;
}