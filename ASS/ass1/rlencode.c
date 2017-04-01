//
// Created by yu on 9/03/17.
//
#include <stdio.h>
void int_to_my_type(int a, FILE *b);

void encode(FILE *input, FILE *output, int print_flag){
    char data;
    int a = -1;
    unsigned char pre_data = 0;
    int counter = -2;
    while((data = fgetc(input)) != EOF){
        if(data != pre_data){
            if(counter > -1){
                if(print_flag)
                    printf("[%d]", counter);
                else
                    int_to_my_type(counter, output);
            }else if(counter > -2){
                if(print_flag)
                    printf("%c", pre_data);
                else
                    fputc(pre_data, output);
            }
            if(print_flag)
                printf("%c", data);
            else
                fputc((unsigned char)data, output);
            pre_data = (unsigned char) data;
            counter = -2;
        }else{
            counter++;
        }
    }
    if(counter != -2)
        print_flag ? printf("[%d]", counter) : int_to_my_type(counter, output);
}
/*
 * transfer int to 4 unsigned char parts.
 * each part consist one signal bit and 7 value bits.
 * part1:   1     000 0000
 *        signal     value
 * signal: 0 means this is a character.
 *         1 means this is a counter.
 * use 7 value bits to count how many depulicated.
 * if more than 2^7=128, use one more part to record it.
 * so the total range is 2^14=16384, and so on...
 */
void int_to_my_type(int num, FILE *output){
    if(num > 268435456){ //2^28
        printf("too large number %d\n", num);
        return;
    }
    unsigned char part1 = (num >> 21 & 0x7f| 0x80);
    unsigned char part2 = (num >> 14 & 0x7f| 0x80);
    unsigned char part3 = (num >> 7  & 0x7f| 0x80);
    unsigned char part4 = (num & 0x7f| 0x80);
    fputc(part4, output);
    if(num >= 128)//2^7)
        fputc(part3, output);
    if(num >= 16384)//2^14)
        fputc(part2, output);
    if(num >= 2097152)//2^21)
        fputc(part1, output);
    return;
}

void main(int argc, char *argv[]){
    FILE *in_file;
    in_file = fopen(argv[1], "r");
    if(argc == 2){
        if(in_file)
            encode(in_file, NULL, 1);
    }else if(argc == 3) {
        FILE *out_file;
        out_file = fopen(argv[2], "w");
        if (in_file && out_file) {
            encode(in_file, out_file, 0);
        } else {
            printf("No such file");
            return;
        }
    }
}
