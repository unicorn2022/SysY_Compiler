int ReadLine(int s[]) {
    int ch = getch();
    if (ch <= 0) {
        //printf("ch = %d\n", ch);
        return -1;
    }
    int len = 0;
    while (ch != '\n' && ch > 0) {
        s[len] = ch;
        ch = getch();
        len = len + 1;
    }
    s[len] = 0;
    return len;
}

void PrintLine(int s[]) {
    int i = 0;
    while (s[i] != 0) {
        putch(s[i]);
        i = i + 1;
    }
    putch('\n');
}

void PrintInt(int x) {
    if (x < 0) {
        putch('-');
        x = -x;
    }
    if (x >= 10) {
        PrintInt(x / 10);
    }
    putch(x % 10 + '0');
}

void Print1FDiv(int x, int y) {
    int ans = x * 100 / y;
    int Up = (ans + 5) / 100;
    int Down = (ans + 5) % 100;
    //printf("x = %d, y = %d, ans = %d, Up = %d, Down = %d\n", x, y, ans, Up, Down);

    if (Down >= 100) {
        Up = Up + 1;
        Down = Down - 100;
    }

    // ��Ҫ��λ
    PrintInt(Up);
    putch('.');
    putch(Down / 10 + '0');
    
}

//struct Course {
//    int credit;      // ѧ��
//    int need[8][8];  // ǰ�ÿγ�
//    int cnt;         // ǰ�ÿγ̵�������
//    int grade;       // �÷ֵȼ�, -1 ��ʾû�ж���
//} a[110];
int credit[110];        // ѧ��
int need[110][8][8];  // ǰ�ÿγ�
int cnt[110];         // ǰ�ÿγ̵�������
int grade[110];       // �÷ֵȼ�, -1 ��ʾû�ж���
int course[110][110];
int course_cnt;  // hash���ĳ���
int Insert(int s[]) {
    int now = 0;
    while (s[now] != 0) {
        course[course_cnt][now] = s[now];
        now = now + 1;
    }
    course_cnt = course_cnt + 1;
    return course_cnt - 1;
}
int GetLen(int s[]) {
    int len = 0;
    while (s[len] != 0) {
        len = len + 1;
    }
    return len;
}
int comp(int s1[], int s2[]) {
    int len1 = GetLen(s1);
    int len2 = GetLen(s2);
    if (len1 != len2) {
        return 0;
    }
    int i = 0;
    while (i < len1) {
        if (s1[i] != s2[i]) {
            return 0;
        }
        i = i + 1;
    }
    return 1;
}
int Find(int s[]) {
    int i = 0;
    while (i < course_cnt) {
        if (comp(s, course[i])) {
            return i;
        }
        i = i + 1;
    }
    return Insert(s);
}

// ��������˳��洢�γ�
int course_input[110];
// ����ǰ�γ̵���Ϣ��������
void Prework(int line[]) {
    if (GetLen(line) == 0) {
        return;
    }
    // �ҵ�ǰ�γ� ��1��|֮ǰ
    int now_count = 0;
    int now[110];
    // initialize: int now[110] = {0}
    int init_i = 0;
    while (init_i < 110) {
        now[init_i] = 0;
        init_i = init_i + 1;
    }
    int tmp = 0;
    int i = 0;
    while (line[i] != '|') {
        now_count = now_count + 1;
        tmp = now_count - 1;
        now[tmp] = line[i];
        i = i + 1;
    }

    now_count = now_count + 1;
    tmp = now_count - 1;
    now[tmp] = 0;
    i = i + 1; 


    int index = Find(now);
    course_input[0] = course_input[0] + 1;
    tmp = course_input[0];
    course_input[tmp] = index;

    // ��ǰ�γ̵�ѧ�� 1~2��|֮��
    credit[index] = line[i] - '0';
    i = i + 2;

    // ��ǰ�γ̵������γ� 2~3��|֮��
    while (line[i] != 0 && line[i] != '|') {
        // һ�������γ�
        while (line[i] != 0 && line[i] != ';' && line[i] != '|') {
            // һ�������γ�
            now_count = 0;
            while (line[i] != 0 && line[i] != ',' && line[i] != ';' && line[i] != '|') {
                now_count = now_count + 1;
                tmp = now_count - 1;
                now[tmp] = line[i];
                i = i + 1;
            }
            now_count = now_count + 1;
            tmp = now_count - 1;
            now[tmp] = 0;

            int count = cnt[index];
            need[index][count][0] = need[index][count][0] + 1;
            tmp = need[index][count][0];
            need[index][count][tmp] = Find(now);
            if (line[i] == ';' || line[i] == '|') break;
            i = i + 1;
        }
        cnt[index] = cnt[index] + 1;
        if (line[i] == '|') break;
        i = i + 1;
    }
    if (line[i] != 0) i = i + 1;

    // ��ǰ�γ̵ĵ÷�
    if (line[i] == 'A') {
        grade[index] = 4;
    } else if (line[i] == 'B') {
        grade[index] = 3;
    } else if (line[i] == 'C') {
        grade[index] = 2;
    } else if (line[i] == 'D') {
        grade[index] = 1;
    } else if (line[i] == 'F') {
        grade[index] = 0;
    } else {
        grade[index] = -1;
    }
}

// ���GPA
void PrintGPA() {
    //double sum_credit_f = 0, sum_grade_f = 0;
    int sum_credit = 0;
    int sum_grade = 0;
    int i = 0;
    while (i < course_cnt) {
        if (grade[i] == -1) {
            i = i + 1;
            continue;
        }
        sum_credit = sum_credit + credit[i];
        //sum_credit_f += credit[i];
        sum_grade = sum_grade + grade[i] * credit[i];
        //sum_grade_f += grade[i] * credit[i];
        i = i + 1;
    }

    // if (sum_credit == 0)
    //     printf("GPA: 0.0\n");
    // else
    //     printf("GPA: %.1f\n", sum_grade / sum_credit);

    if (sum_credit == 0) {
        putch('G');
        putch('P');
        putch('A');
        putch(':');
        putch(' ');
        putch('0');
        putch('.');
        putch('0');
        putch('\n');
    } else {
        putch('G');
        putch('P');
        putch('A');
        putch(':');
        putch(' ');
        Print1FDiv(sum_grade, sum_credit);
        putch('\n');
    }
}

// �������ѧ��
void PrintAttemped() {
    int sum_credit = 0;
    int i = 0; 
    while (i < course_cnt) {
        if (grade[i] == -1) {
            i = i + 1;
            continue;
        }
        sum_credit = sum_credit + credit[i];
        i = i + 1;
    }
    putch('H');
    putch('o');
    putch('u');
    putch('r');
    putch('s');
    putch(' ');
    putch('A');
    putch('t');
    putch('t');
    putch('e');
    putch('m');
    putch('p');
    putch('t');
    putch('e');
    putch('d');
    putch(':');
    putch(' ');
    PrintInt(sum_credit);
    putch('\n');
    //printf("Hours Attempted: %d\n", sum_credit);
}

// �������ѧ��
void PrintCompleted() {
    int sum_credit = 0;
    int i = 0; 
    while (i < course_cnt) {
        if (grade[i] <= 0) {
            i = i + 1;
            continue;
        }
        sum_credit = sum_credit + credit[i];
        i = i + 1;
    }
    putch('H');
    putch('o');
    putch('u');
    putch('r');
    putch('s');
    putch(' ');
    putch('C');
    putch('o');
    putch('m');
    putch('p');
    putch('l');
    putch('e');
    putch('t');
    putch('e');
    putch('d');
    putch(':');
    putch(' ');
    PrintInt(sum_credit);
    putch('\n');
    //printf("Hours Completed: %d\n", sum_credit);
}

// ���ʣ��ѧ��
int credit_remaining;
void PrintRemaining() {
    int sum_credit = 0;
    int i = 0;
    while (i < course_cnt) {
        if (grade[i] > 0) {
            i = i + 1;
            continue;
        }
        sum_credit = sum_credit + credit[i];
        i = i + 1;
    }
    credit_remaining = sum_credit;
    putch('C');
    putch('r');
    putch('e');
    putch('d');
    putch('i');
    putch('t');
    putch('s');
    putch(' ');
    putch('R');
    putch('e');
    putch('m');
    putch('a');
    putch('i');
    putch('n');
    putch('i');
    putch('n');
    putch('g');
    putch(':');
    putch(' ');
    PrintInt(sum_credit);
    putch('\n');
    //printf("Credits Remaining: %d\n", sum_credit);
}

// �жϵ�index���γ��Ƿ���Ҫ�޶�
int NeedToLearn(int index) {
    // �Ѿ��޶�������û�йҿ�
    if (grade[index] > 0) return 0;

    // û��ǰ�ÿ�, ����ֱ���޶�
    if (cnt[index] == 0) return 1;

    // ��ǰ�ÿ�, �ж��Ƿ�����ĳһ��ǰ�ÿε�Ҫ��
    int i = 0; 
    while (i < cnt[index]) {
        // ��i��ǰ�ÿ�
        int can_learn = 1;
        int j = 1; 
        while (j <= need[index][i][0]) {
            int need_course = need[index][i][j];
            if (grade[need_course] <= 0) can_learn = 0;
            j = j + 1;
        }
        if (can_learn) return 1;
        i = i + 1;
    }
    return 0;
}

int main() {
     //freopen("./input/task3.1.in", "r", stdin);
    int line[1000];
    //while (scanf("%s", line) != EOF) {
    while (ReadLine(line) != -1) {
        Prework(line);
        //fprintf(stderr, "%s\n", line);
    }
    PrintGPA();
    PrintAttemped();
    PrintCompleted();
    PrintRemaining();
    
    putch('\n');
    putch('P');
    putch('o');
    putch('s');
    putch('s');
    putch('i');
    putch('b');
    putch('l');
    putch('e');
    putch(' ');
    putch('C');
    putch('o');
    putch('u');
    putch('r');
    putch('s');
    putch('e');
    putch('s');
    putch(' ');
    putch('t');
    putch('o');
    putch(' ');
    putch('T');
    putch('a');
    putch('k');
    putch('e');
    putch(' ');
    putch('N');
    putch('e');
    putch('x');
    putch('t');
    putch('\n');
    //printf("\nPossible Courses to Take Next\n");
    int i = 1;
    while (i <= course_input[0]) {
        int index = course_input[i];
        if (NeedToLearn(index)) {
            putch(' ');
            putch(' ');
            PrintLine(course[index]);
        }
        i = i + 1;
    }
    if (credit_remaining == 0) {
        putch(' ');
        putch(' ');
        putch('N');
        putch('o');
        putch('n');
        putch('e');
        putch(' ');
        putch('-');
        putch(' ');
        putch('C');
        putch('o');
        putch('n');
        putch('g');
        putch('r');
        putch('a');
        putch('t');
        putch('u');
        putch('l');
        putch('a');
        putch('t');
        putch('i');
        putch('o');
        putch('n');
        putch('s');
        putch('!');
        putch('\n');
        //printf("  None - Congratulations!\n");
    }
    return 0;
}