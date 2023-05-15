#pragma warning(disable:4996)
#include <stdio.h>

const int maxn = 25 + 10;
int n1, m1, a1[maxn][maxn];
int n2, m2, a2[maxn][maxn];
int n3, m3, a3[maxn][maxn];

int main() {
	//freopen("./input/task2.1.in", "r", stdin);
	scanf("%d%d", &n1, &m1);
	for (int i = 1; i <= n1; i++)
		for(int j = 1; j <= m1; j++)
			scanf("%d", &a1[i][j]);
	scanf("%d%d", &n2, &m2);
	for (int i = 1; i <= n2; i++)
		for (int j = 1; j <= m2; j++)
			scanf("%d", &a2[i][j]);

	if (m1 != n2) {
		printf("Incompatible Dimensions\n");
		return 0;
	}
	
	n3 = n1; m3 = m2;
	for (int i = 1; i <= n3; i++)
		for (int j = 1; j <= m3; j++)
			for (int k = 1; k <= m1; k++)
				a3[i][j] += a1[i][k] * a2[k][j];

	for (int i = 1; i <= n3; i++) {
		for (int j = 1; j <= m3; j++)
			printf("%10d", a3[i][j]);
		printf("\n");
	}
	
	return 0;
}