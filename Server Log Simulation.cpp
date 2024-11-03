#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 256
#define SERVER_DB "server_db.csv"
#define PERSON_DB "person_db.csv"
#define ACCESS_DB "access_db.csv"

void add_server();
void add_user();
void add_access();
void view_report();
void load_data();
void sort_access_records();
int compare_dates(const void *a, const void *b);

typedef struct {
    char ip[16];
    char name[50];
} Server;

typedef struct {
    char name[50];
    char id[20];
} User;

typedef struct {
    char id[20];
    char name[50];
    char ip[16];
    char server_name[50];
    char checkin[13];
    char checkout[13];
} AccessRecord;

Server servers[100];
User users[100];
AccessRecord access_records[100];
int num_servers = 0, num_users = 0, num_access_records = 0;

int main() {
    int choice;

    load_data();
    
    while (1) {
        printf("\nSilahkan Pilih Menu:\n");
        printf("1. Masukan Data Server\n");
        printf("2. Masukan Data Person\n");
        printf("3. Masukan Izin Masuk ke Dalam Server\n");
        printf("4. Report Akses Server\n");
        printf("5. Keluar\n");
        printf("Masukan pilihan: ");
        scanf("%d", &choice);
        getchar(); 

        switch (choice) {
            case 1:
                add_server();
                break;
            case 2:
                add_user();
                break;
            case 3:
                add_access();
                break;
            case 4:
                view_report();
                break;
            case 5:
                exit(0);
            default:
                printf("Pilihan tidak ada, silahkan memilih lagi.\n");
        }
    }

    return 0;
}



void add_server() {
    FILE *file = fopen("server_db", "a");
    if (!file) {
        perror("tidak bisa membuka file");
        return;
    }

    Server server;
    printf("Masukan ip address:  ");
    fgets(server.ip, sizeof(server.ip), stdin);
    server.ip[strcspn(server.ip, "\n")] = '\0'; 

    printf("Masukan nama dns server ");
    fgets(server.name, sizeof(server.name), stdin);
    server.name[strcspn(server.name, "\n")] = '\0'; 

    fprintf(file, "%s;%s\n", server.ip, server.name);
    fclose(file);

    servers[num_servers++] = server;
}

int find_server_name(const char *ip, char *name) {
    for (int i = 0; i < num_servers; i++) {
        if (strcmp(servers[i].ip, ip) == 0) {
            strcpy(name, servers[i].name);
            return 1; 
        }
    }
    return 0; 
}


void add_user() {
    FILE *file = fopen("person_db", "a");
    if (!file) {
        perror("Tidak bisa membuka file");
        return;
    }

    User user;
	printf("Masukan id KTP:  ");
    fgets(user.id, sizeof(user.id), stdin);
    user.id[strcspn(user.id, "\n")] = '\0'; 
    
    printf("Masukan nama:  ");
    fgets(user.name, sizeof(user.name), stdin);
    user.name[strcspn(user.name, "\n")] = '\0'; 

    fprintf(file, "%s;%s\n", user.name, user.id);
    fclose(file);

    users[num_users++] = user;
}


int find_user_id(const char *name, char *id) {
    for (int i = 0; i < num_users; i++) {
        if (strcmp(users[i].name, name) == 0) {
            strcpy(id, users[i].id);
            return 1; 
        }
    }
    return 0; 
}

void add_access() {
    FILE *file = fopen("access_db", "a");
    if (!file) {
        perror("Unable to open access_db file");
        return;
    }

    AccessRecord record;

    printf("Masukan nama anda:  ");
    fgets(record.name, sizeof(record.name), stdin);
    record.name[strcspn(record.name, "\n")] = '\0';

    if (!find_user_id(record.name, record.id)) {
        printf("Nama pengguna tidak ditemukan. Akses ditolak.\n");
        fclose(file);
        return;
    }
    
    printf("Masukan ip address server: ");
    fgets(record.ip, sizeof(record.ip), stdin);
    record.ip[strcspn(record.ip, "\n")] = '\0';

    if (!find_server_name(record.ip, record.server_name)) {
        printf("IP server tidak ditemukan. Akses ditolak.\n");
        fclose(file);
        return;
    }

    printf("masukan tanggal jam masuk dan keluar server(yyyyMMddhhmmyyyyMMddhhmm): ");
    fgets(record.checkin, sizeof(record.checkin), stdin);
    record.checkin[strcspn(record.checkin, "\n")] = '\0';
    fgets(record.checkout, sizeof(record.checkout), stdin);
    record.checkout[strcspn(record.checkout, "\n")] = '\0';

    fprintf(file, "%s;%s;%s;%s;%s;%s\n", record.id, record.name, record.ip, record.server_name, record.checkin, record.checkout);
    fclose(file);

    access_records[num_access_records++] = record;
}

void view_report() {
    sort_access_records();

    printf("\nReport:\n");
    printf("ID;Nama;IP;Nama Server;Jam Masuk;Jam Keluar\n");
    for (int i = 0; i < num_access_records; i++) {
        printf("%s;%s;%s;%s;%s;%s\n", access_records[i].id, access_records[i].name, access_records[i].ip, access_records[i].server_name, access_records[i].checkin, access_records[i].checkout);
    }
}

void load_data() {
    FILE *file;
    char line[MAX_LINE_LENGTH];

    file = fopen(SERVER_DB, "r");
    if (file) {
        while (fgets(line, sizeof(line), file)) {
            sscanf(line, "%15[^;];%49[^\n]", servers[num_servers].ip, servers[num_servers].name);
            num_servers++;
        }
        fclose(file);
    }

    file = fopen(PERSON_DB, "r");
    if (file) {
        while (fgets(line, sizeof(line), file)) {
            sscanf(line, "%49[^;];%19[^\n]", users[num_users].name, users[num_users].id);
            num_users++;
        }
        fclose(file);
    }

    file = fopen(ACCESS_DB, "r");
    if (file) {
        while (fgets(line, sizeof(line), file)) {
            sscanf(line, "%19[^;];%49[^;];%15[^;];%49[^;];%12[^;];%12[^\n]",
                   access_records[num_access_records].id,
                   access_records[num_access_records].name,
                   access_records[num_access_records].ip,
                   access_records[num_access_records].server_name,
                   access_records[num_access_records].checkin,
                   access_records[num_access_records].checkout);
            num_access_records++;
        }
        fclose(file);
    }
}

void sort_access_records() {
    qsort(access_records, num_access_records, sizeof(AccessRecord), compare_dates);
}

int compare_dates(const void *a, const void *b) {
    const AccessRecord *recordA = (const AccessRecord *)a;
    const AccessRecord *recordB = (const AccessRecord *)b;
    return strcmp(recordA->checkin, recordB->checkin);
}

