#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Course {
	int credit; 	// 学分
	int need[8][8]; // 前置课程
	int cnt;		// 前置课程的种类数
	int grade;		// 得分等级, -1 表示没有读过
}a[110];
char course[110][110];
int course_cnt; // hash表的长度
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

// 按照输入顺序存储课程
int course_input[110];
// 将当前课程的信息保存下来
void Prework(char line[]) {
	// 找当前课程 第1个|之前
	char now[110] = { 0 };
	while (*line != '|') {
		now[++now[0]] = *line;
		line++;
	}
	now[++now[0]] = 0;
	line++;
	int index = Find(now + 1);
	course_input[++course_input[0]] = index;

	// 当前课程的学分 1~2个|之间
	a[index].credit = *line - '0';
	line += 2;

	// 当前课程的依赖课程 2~3个|之间
	while (*line && *line != '|') {
		// 一组依赖课程
		while (*line && *line != ';' && *line != '|') {
			// 一门依赖课程
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

	// 当前课程的得分
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

// 输出GPA
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

// 输出尝试学分
void PrintAttemped() {
	int sum_credit = 0;
	for (int i = 0; i < course_cnt; i++) {
		if (a[i].grade == -1) continue;
		sum_credit += a[i].credit;
	}
	printf("Hours Attempted: %d\n", sum_credit);
}

// 输出已修学分
void PrintCompleted() {
	int sum_credit = 0;
	for (int i = 0; i < course_cnt; i++) {
		if (a[i].grade <= 0) continue;
		sum_credit += a[i].credit;
	}
	printf("Hours Completed: %d\n", sum_credit);
}

// 输出剩余学分
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

// 判断第index个课程是否需要修读
bool NeedToLearn(int index) {
	// 已经修读过并且没有挂科
	if (a[index].grade > 0) return false;
	
	// 没有前置课, 可以直接修读
	if (a[index].cnt == 0) return true;
	
	// 有前置课, 判断是否满足某一组前置课的要求
	for (int i = 0; i < a[index].cnt; i++) {
		// 第i组前置课
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