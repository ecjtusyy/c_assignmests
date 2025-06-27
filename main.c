#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SCHEDULE 100

typedef struct {
    int id;                 // 班次号
    char departure_time[6];  // 发车时间 "HH:MM"
    char start[20];          // 起点站
    char end[20];            // 终点站
    float duration;          // 行车时间（小时）
    int capacity;            // 额定载客量
    int booked;              // 已定票人数
} Schedule;

Schedule schedules[MAX_SCHEDULE];
int schedule_count = 0;

// 文件路径
const char *FILENAME = "schedule.dat";

// 函数声明
void load_schedules();
void save_schedules();
void add_schedule();
void display_schedules();
void search_schedule();
void sell_ticket();
void refund_ticket();
void delete_schedule();
int compare_time(const char *t1, const char *t2);
void get_current_time_str(char *time_str);

// 主程序入口
int main() {
    load_schedules();
    int choice;
    do {
        printf("\n========= 车票管理系统 =========\n");
        printf("1. 增加班次\n");
        printf("2. 浏览班次\n");
        printf("3. 查询班次\n");
        printf("4. 售票\n");
        printf("5. 退票\n");
        printf("6. 删除班次\n");
        printf("0. 退出\n");
        printf("请输入选项：");
        scanf("%d", &choice);
        getchar(); // 吃掉回车

        switch (choice) {
            case 1: add_schedule(); break;
            case 2: display_schedules(); break;
            case 3: search_schedule(); break;
            case 4: sell_ticket(); break;
            case 5: refund_ticket(); break;
            case 6: delete_schedule(); break;
            case 0: save_schedules(); printf("退出系统，数据已保存。\n"); break;
            default: printf("无效选项，请重新输入。\n");
        }
    } while (choice != 0);

    return 0;
}

// 加载数据
void load_schedules() {
    FILE *fp = fopen(FILENAME, "rb");
    if (fp != NULL) {
        fread(&schedule_count, sizeof(int), 1, fp);
        fread(schedules, sizeof(Schedule), schedule_count, fp);
        fclose(fp);
    } else {
        // 文件不存在，初始化数据
        schedule_count = 4;
        schedules[0] = (Schedule){1, "08:00", "郫县", "广汉", 2.0, 45, 30};
        schedules[1] = (Schedule){2, "06:30", "郫县", "成都", 0.5, 40, 40};
        schedules[2] = (Schedule){3, "07:00", "郫县", "成都", 0.5, 40, 20};
        schedules[3] = (Schedule){4, "10:00", "郫县", "成都", 0.5, 40, 2};
        save_schedules();
        printf("首次运行：已初始化车次数据。\n");
    }
}


// 保存数据
void save_schedules() {
    FILE *fp = fopen(FILENAME, "wb");
    if (fp != NULL) {
        fwrite(&schedule_count, sizeof(int), 1, fp);
        fwrite(schedules, sizeof(Schedule), schedule_count, fp);
        fclose(fp);
    } else {
        printf("保存文件失败！\n");
    }
}

// 增加班次
void add_schedule() {
    if (schedule_count >= MAX_SCHEDULE) {
        printf("班次已满，无法添加。\n");
        return;
    }
    Schedule s;
    printf("输入班次号：");
    scanf("%d", &s.id);
    printf("输入发车时间(HH:MM)：");
    scanf("%s", s.departure_time);
    printf("输入起点站：");
    scanf("%s", s.start);
    printf("输入终点站：");
    scanf("%s", s.end);
    printf("输入行车时间(小时)：");
    scanf("%f", &s.duration);
    printf("输入额定载客量：");
    scanf("%d", &s.capacity);
    s.booked = 0;

    schedules[schedule_count++] = s;
    save_schedules();
    printf("添加班次成功！\n");
}

// 显示所有班次
void display_schedules() {
    char now[6];
    get_current_time_str(now);

    printf("\n%-5s %-6s %-10s %-10s %-6s %-6s %-6s 状态\n", 
        "班次", "发车", "起点", "终点", "时间", "载量", "已定");

    for (int i = 0; i < schedule_count; i++) {
        Schedule *s = &schedules[i];
        printf("%-5d %-6s %-10s %-10s %-6.1f %-6d %-6d ", 
            s->id, s->departure_time, s->start, s->end, s->duration, s->capacity, s->booked);
        if (compare_time(now, s->departure_time) >= 0) {
            printf("此班已发出\n");
        } else {
            printf("未发车\n");
        }
    }
}

// 查询班次
void search_schedule() {
    int option;
    printf("1. 按班次号查询\n2. 按终点站查询\n选择：");
    scanf("%d", &option);
    if (option == 1) {
        int id;
        printf("输入班次号：");
        scanf("%d", &id);
        for (int i = 0; i < schedule_count; i++) {
            if (schedules[i].id == id) {
                printf("找到班次：%d %s %s %s %.1f %d %d\n", schedules[i].id, schedules[i].departure_time, 
                    schedules[i].start, schedules[i].end, schedules[i].duration, 
                    schedules[i].capacity, schedules[i].booked);
                return;
            }
        }
        printf("未找到该班次。\n");
    } else if (option == 2) {
        char end[20];
        printf("输入终点站：");
        scanf("%s", end);
        int found = 0;
        for (int i = 0; i < schedule_count; i++) {
            if (strcmp(schedules[i].end, end) == 0) {
                printf("班次：%d %s %s %s %.1f %d %d\n", schedules[i].id, schedules[i].departure_time, 
                    schedules[i].start, schedules[i].end, schedules[i].duration, 
                    schedules[i].capacity, schedules[i].booked);
                found = 1;
            }
        }
        if (!found) printf("未找到到该终点的班次。\n");
    } else {
        printf("无效选项。\n");
    }
}

// 售票
void sell_ticket() {
    int id;
    printf("输入班次号售票：");
    scanf("%d", &id);
    char now[6];
    get_current_time_str(now);

    for (int i = 0; i < schedule_count; i++) {
        Schedule *s = &schedules[i];
        if (s->id == id) {
            if (compare_time(now, s->departure_time) >= 0) {
                printf("班次已发出，不能售票。\n");
                return;
            }
            if (s->booked >= s->capacity) {
                printf("座位已满，不能售票。\n");
                return;
            }
            s->booked++;
            save_schedules();
            printf("售票成功！已定票人数：%d\n", s->booked);
            return;
        }
    }
    printf("未找到该班次。\n");
}

// 退票
void refund_ticket() {
    int id;
    printf("输入班次号退票：");
    scanf("%d", &id);
    char now[6];
    get_current_time_str(now);

    for (int i = 0; i < schedule_count; i++) {
        Schedule *s = &schedules[i];
        if (s->id == id) {
            if (compare_time(now, s->departure_time) >= 0) {
                printf("班次已发出，不能退票。\n");
                return;
            }
            if (s->booked <= 0) {
                printf("无票可退。\n");
                return;
            }
            s->booked--;
            save_schedules();
            printf("退票成功！已定票人数：%d\n", s->booked);
            return;
        }
    }
    printf("未找到该班次。\n");
}

// 删除班次
void delete_schedule() {
    int id;
    printf("输入班次号删除：");
    scanf("%d", &id);
    for (int i = 0; i < schedule_count; i++) {
        if (schedules[i].id == id) {
            for (int j = i; j < schedule_count - 1; j++) {
                schedules[j] = schedules[j + 1];
            }
            schedule_count--;
            save_schedules();
            printf("删除成功。\n");
            return;
        }
    }
    printf("未找到该班次。\n");
}

// 比较时间，t1 >= t2 返回 >=0，否则<0
int compare_time(const char *t1, const char *t2) {
    int h1, m1, h2, m2;
    sscanf(t1, "%d:%d", &h1, &m1);
    sscanf(t2, "%d:%d", &h2, &m2);
    if (h1 != h2) return h1 - h2;
    return m1 - m2;
}

// 获取当前时间字符串
void get_current_time_str(char *time_str) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    sprintf(time_str, "%02d:%02d", t->tm_hour, t->tm_min);
}
