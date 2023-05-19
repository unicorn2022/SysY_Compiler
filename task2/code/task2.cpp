#pragma warning(disable:4996)
#include <stdio.h>

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

int GetIntLength(int x) {
	int ans = 1;
	if (x < 0) {
		x = -x;
		ans = ans + 1;
	}
	if (x >= 10) {
		ans = ans + GetIntLength(x / 10);
	}
	return ans;
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

const int maxn = 25 + 10;
int n1, m1, a1[maxn][maxn];
int n2, m2, a2[maxn][maxn];
int n3, m3, a3[maxn][maxn];

int main() {
	//freopen("./input/task2.1.in", "r", stdin);
	n1 = ReadInt();
	m1 = ReadInt();
	//scanf("%d%d", &n1, &m1);

	for (int i = 1; i <= n1; i++) {
		for (int j = 1; j <= m1; j++) {
			a1[i][j] = ReadInt();
			//scanf("%d", &a1[i][j]);
		}
	}

	n2 = ReadInt();
	m2 = ReadInt();
	//scanf("%d%d", &n2, &m2);

	for (int i = 1; i <= n2; i++) {
		for (int j = 1; j <= m2; j++) {
			a2[i][j] = ReadInt();
			//scanf("%d", &a2[i][j]);
		}
	}

	if (m1 != n2) {
		putchar('I');
		putchar('n');
		putchar('c');
		putchar('o');
		putchar('m');
		putchar('p');
		putchar('a');
		putchar('t');
		putchar('i');
		putchar('b');
		putchar('l');
		putchar('e');
		putchar(' ');
		putchar('D');
		putchar('i');
		putchar('m');
		putchar('e');
		putchar('n');
		putchar('s');
		putchar('i');
		putchar('o');
		putchar('n');
		putchar('s');
		putchar('\n');
		//printf("Incompatible Dimensions\n");
		return 0;
	}
	
	n3 = n1; m3 = m2;
	for (int i = 1; i <= n3; i++) {
		for (int j = 1; j <= m3; j++) {
			for (int k = 1; k <= m1; k++) {
				a3[i][j] += a1[i][k] * a2[k][j];
			}
		}
	}

	for (int i = 1; i <= n3; i++) {
		for (int j = 1; j <= m3; j++) {
			int len = GetIntLength(a3[i][j]);
			//printf("len = %d\n", len);
			for (int k = 1; k <= 10 - len; k++) {
				putchar(' ');
			}
			PrintInt(a3[i][j]);
			//printf("%10d", a3[i][j]);
		}
		putchar('\n');
	}
	
	return 0;
}