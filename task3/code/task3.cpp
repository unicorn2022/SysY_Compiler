#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Course {
	int credit; 	// ѧ��
	int need[8][8]; // ǰ�ÿγ�
	int cnt;		// ǰ�ÿγ̵�������
	int grade;		// �÷ֵȼ�, -1 ��ʾû�ж���
}a[110];
char course[110][110];
int course_cnt; // hash��ĳ���
int Insert(char s[]) {
	int now = 0;
	while (s[now]) {
		course[course_cnt][now] = s[now];
		now = now + 1;
	}
	course_cnt = course_cnt + 1;
	return course_cnt - 1;
}
bool comp(char s1[], char s2[]) {
	int len1 = strlen(s1), len2 = strlen(s2);
	if (len1 != len2) return false;
	for (int i = 0; i < len1; i++)
		if (s1[i] != s2[i]) return false;
	return true;
}
int Find(char s[]) {
	for (int i = 0; i < course_cnt; i++)
		if (comp(s, course[i])) return i;
	return Insert(s);
}

// ��������˳��洢�γ�
int course_input[110];
// ����ǰ�γ̵���Ϣ��������
void Prework(char line[]) {
	// �ҵ�ǰ�γ� ��1��|֮ǰ
	char now[110] = { 0 };
	while (*line != '|') {
		now[++now[0]] = *line;
		line++;
	}
	now[++now[0]] = 0;
	line++;
	int index = Find(now + 1);
	course_input[++course_input[0]] = index;

	// ��ǰ�γ̵�ѧ�� 1~2��|֮��
	a[index].credit = *line - '0';
	line += 2;

	// ��ǰ�γ̵������γ� 2~3��|֮��
	while (*line && *line != '|') {
		// һ�������γ�
		while (*line && *line != ';' && *line != '|') {
			// һ�������γ�
			now[0] = 0;
			while (*line && *line != ',' && *line != ';' && *line != '|') {
				now[++now[0]] = *line;
				line++;
			}
			now[++now[0]] = 0;
			int cnt = a[index].cnt;
			a[index].need[cnt][++a[index].need[cnt][0]] = Find(now + 1);
			if (*line == ';' || *line == '|') break;
			line++;
		}
		a[index].cnt++;
		if (*line == '|') break;
		line++;
	}
	if (*line) line++;

	// ��ǰ�γ̵ĵ÷�
	if (*line == 'A') a[index].grade = 4;
	else if (*line == 'B') a[index].grade = 3;
	else if (*line == 'C') a[index].grade = 2;
	else if (*line == 'D') a[index].grade = 1;
	else if (*line == 'F') a[index].grade = 0;
	else  a[index].grade = -1;
}

void Debug() {
	for (int i = 0; i < course_cnt; i++) {
		printf("No.%d\n", i);
		printf("course: %s\n", course[i]);
		printf("credit: %d\n", a[i].credit);
		printf("need:\n");
		for (int j = 0; j < a[i].cnt; j++) {
			printf("(");
			for (int k = 1; k <= a[i].need[j][0]; k++)
				printf("%s,", course[a[i].need[j][k]]);
			printf(")\n");
		}
		printf("grade: %d\n\n", a[i].grade);
	}
}

// ���GPA
void PrintGPA() {
	double sum_credit = 0, sum_grade = 0;
	for (int i = 0; i < course_cnt; i++) {
		if (a[i].grade == -1) continue;
		sum_credit += a[i].credit;
		sum_grade += a[i].grade * a[i].credit;
	}
	if (sum_credit == 0) printf("GPA: 0.0\n");
	else printf("GPA: %.1f\n", sum_grade / sum_credit);
}

// �������ѧ��
void PrintAttemped() {
	int sum_credit = 0;
	for (int i = 0; i < course_cnt; i++) {
		if (a[i].grade == -1) continue;
		sum_credit += a[i].credit;
	}
	printf("Hours Attempted: %d\n", sum_credit);
}

// �������ѧ��
void PrintCompleted() {
	int sum_credit = 0;
	for (int i = 0; i < course_cnt; i++) {
		if (a[i].grade <= 0) continue;
		sum_credit += a[i].credit;
	}
	printf("Hours Completed: %d\n", sum_credit);
}

// ���ʣ��ѧ��
int credit_remaining;
void PrintRemaining() {
	int sum_credit = 0;
	for (int i = 0; i < course_cnt; i++) {
		if (a[i].grade > 0) continue;
		sum_credit += a[i].credit;
	}
	credit_remaining = sum_credit;
	printf("Credits Remaining: %d\n", sum_credit);
}

// �жϵ�index���γ��Ƿ���Ҫ�޶�
bool NeedToLearn(int index) {
	// �Ѿ��޶�������û�йҿ�
	if (a[index].grade > 0) return false;
	
	// û��ǰ�ÿ�, ����ֱ���޶�
	if (a[index].cnt == 0) return true;
	
	// ��ǰ�ÿ�, �ж��Ƿ�����ĳһ��ǰ�ÿε�Ҫ��
	for (int i = 0; i < a[index].cnt; i++) {
		// ��i��ǰ�ÿ�
		bool can_learn = true;
		for (int j = 1; j <= a[index].need[i][0]; j++) {
			int need_course = a[index].need[i][j];
			if (a[need_course].grade <= 0) can_learn = false;
		}
		if (can_learn) return true;
	}
	return false;
}

int main() {
	freopen("./input/task3.1.in", "r", stdin);
	char line[1000];
	while (scanf("%s", line) != EOF) {
		Prework(line);
	}
	PrintGPA();
	PrintAttemped();
	PrintCompleted();
	PrintRemaining();
	printf("\nPossible Courses to Take Next\n");
	for (int i = 1; i <= course_input[0]; i++) {
		int index = course_input[i];
		if (NeedToLearn(index)) {
			printf("  %s\n", course[index]);
		}
	}
	if (credit_remaining == 0) {
		printf("  None - Congratulations!\n");
	}
	return 0;
}