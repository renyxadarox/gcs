#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#define max(a,b) ((a) > (b) ? a : b)
#define min(a,b) ((a) < (b) ? a : b)

#define MAX_ROW_SIZE 1000000

uint64_t starttime, endtime;
struct timeval timecheck;
uint64_t duration;

FILE * finput, * foutput;
const char * input_file_name = "input.txt";
const char * output_file_name = "output.txt";

uint16_t ** a;
uint32_t ** r;
uint32_t a0_size, a1_size, record_cnt = 0, max_seq_cnt = 0, min_seq_cnt = INT32_MAX;

void create_element_array(int rows, int cols){
    int i;
    a = malloc(sizeof(uint16_t*)*rows);
    for(i=0; i<rows; i++){
        a[i]=calloc(cols, sizeof(uint16_t));
    }
}

void create_link_array(int rows, int cols){
    int i;
    r = malloc(sizeof(struct uint32_t*)*rows);
    for(i=0; i<rows; i++){
        r[i]=calloc(cols, sizeof(uint32_t));
    }
}

char ** erase_element_array(int rows){
    int i;
    for(i=0; i<rows; i++){
        free(a[i]);
    }
    free(a);
    return NULL;
}

char ** erase_link_array(int rows){
    int i;
    for(i=0; i<rows; i++){
        free(r[i]);
    }
    free(r);
    return NULL;
}

int read_input(void)
{
    char * estr, * istr;
    char sep[3]=" ,|";
    char s[MAX_ROW_SIZE];
    int i;
    uint32_t j, seq_cnt;
    finput = fopen(input_file_name, "r");
    if(finput == NULL)
        return (EXIT_FAILURE);
    for (i=0; i<2; i++) {
        estr = fgets(s, MAX_ROW_SIZE, finput);
        if (estr == NULL) {
            exit(EXIT_FAILURE);
        }
        seq_cnt = 0;
        istr = strtok(s,sep);
        while (istr != NULL) {
            seq_cnt++;
            istr = strtok(NULL,sep);
        }
        if (i == 0) a0_size = seq_cnt;
        if (i == 1) a1_size = seq_cnt;
        max_seq_cnt = max(max_seq_cnt, seq_cnt);
        min_seq_cnt = min(min_seq_cnt, seq_cnt);
    }
    create_element_array(2, max_seq_cnt);
    create_link_array(2, min_seq_cnt);
    rewind(finput);
    for (i=0; i<2; i++) {
        j = 0;
        fgets(s, MAX_ROW_SIZE, finput);
        istr = strtok(s,sep);
        while (istr != NULL) {
            sscanf(istr, "%" SCNu16, &a[i][j++]);
            istr = strtok(NULL,sep);
        }
    }
    fclose(finput);
    return 0;
}

int print_element_array(int i, uint32_t size)
{
    for (uint32_t j=0; j<size; j++)
        printf("%" PRIu16 " ", a[i][j]);
    return 0;
}

int print_record_array(int i, uint32_t size)
{
    for (uint32_t j=0; j<size; j++)
        printf("%" PRIu16 " ", a[0][r[i][j]]);
    return 0;
}

void print_progress(int force)
{
    gettimeofday(&timecheck, NULL);
    endtime = (uint64_t)timecheck.tv_sec * 1000 + (uint64_t)timecheck.tv_usec / 1000;
    if ((endtime - starttime) / 1000 > duration || force) {
        duration = (endtime - starttime) / 1000;
        uint64_t dif = endtime - starttime;
        printf("\rRecord: %" PRIu32 ". Elapsed time: %02d:%02d:%02d.%03d", record_cnt, (unsigned char)(dif/60/60/1000), (unsigned char)((dif/60/1000)%60), (unsigned char)((dif/1000)%60), (unsigned char)(dif%1000));
    }
}

void find_gcs(uint32_t x, uint32_t y, uint32_t z)
{
    uint16_t e;
    uint32_t i, j, k, l;
    if (z > record_cnt) {
        for (k=0; k < z; k++) {
            r[1][k] = r[0][k];
        }
        record_cnt++;
        printf("\r");
        print_record_array(1, record_cnt);
        printf("\n");
    }
    if (a1_size - y + z <= record_cnt) return;
    i = x;
    print_progress(0);
    while (a0_size - i + z > record_cnt) {
        e = a[0][i];
        l = x;
        while (l < i && a[0][l] != e) l++;
        if (l != i) {
            i++;
            continue;
        }
        j = y;
        while (a1_size - j + z > record_cnt) {
            if (a[1][j] == e) {
                r[0][z] = i;
                find_gcs(i+1, j+1, z+1);
            }
            j++;
        }
        i++;
    }
}

int write_output(void)
{
    foutput = fopen(output_file_name, "w");
    if(foutput == NULL)
        return (EXIT_FAILURE);
    for (uint32_t i=0; i<record_cnt; i++) {
        fprintf(foutput, "%" PRIu16, a[0][r[1][i]]);
        if (i<record_cnt-1)
            fprintf(foutput, " ");
    }
    fprintf(foutput, "\n");
    fflush(foutput);
    fclose(foutput);
    return 0;
}

int main()
{
    gettimeofday(&timecheck, NULL);
    starttime = (uint64_t)timecheck.tv_sec * 1000 + (uint64_t)timecheck.tv_usec / 1000;

    read_input();
    printf("Input Sequences:\n");
   if (max_seq_cnt < 20) {
        print_element_array(0, a0_size);
        printf("\n");
        print_element_array(1, a1_size);
        printf("\n");
    } else {
        printf("1st sequence has %" PRIu32 " elements\n", a0_size);
        printf("2nd sequence has %" PRIu32 " elements\n", a1_size);
    };
    printf("Greatest Common Sequence:\n");
    find_gcs(0, 0, 0);
    printf("\r");
    printf("\nGreatest Common Sequence:\n");
    print_record_array(1, record_cnt);
    printf("\n");
    write_output();

    erase_link_array(2);
    erase_element_array(2);

    print_progress(1);
    printf("\n");

    return 0;
}
