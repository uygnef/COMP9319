//
// Created by yu on 10/03/17.
//
#include <stdio.h>

void decode(FILE *file, FILE *outfile, int flag);

void main(int argc, char *argv[]){
    FILE *in_file;
    in_file = fopen(argv[1], "r");
    if(argc == 2){                  //decide print or write on file
        if(in_file)
            decode(in_file, NULL, 0);
    }else if(argc == 3) {
        FILE *out_file;
        out_file = fopen(argv[2], "w");
        if (in_file && out_file) {
            decode(in_file, out_file, 1);
        } else {
            printf("No such file %s.\n", argv[1]);
            return;
        }
    }
}

/*
 * transfer character on file to int.
 * int = char[4]<<21+char[3]<<14+char[2]<<7+char[1]
 * char will get rid of first bit which is a signal bit
 * represent this character is a counter.
 * char[0]: 1byte    1      0000000
 *                   ^     ----^----
 *                signal  number bits
 */
unsigned int trans_to_int(unsigned char *num, int length){
    unsigned int result = 0;
    unsigned char temp;
    for(int i=0;i<length;i++){
        temp = num[i];
        result += ((temp & 0x7f) << (7*i));
    }
    return result;
}
void decode(FILE *input, FILE *output, int out_flag) {
    unsigned int data;
    unsigned char all_part[4];
    int i=0;
    char previous_char;

    while ((data = fgetc(input)) != EOF) {
        if(data < 0x80){
            if(i != 0){
                int number;
                number = trans_to_int(all_part, i);
                i = 0;
                if(out_flag == 1 ){
                    for(int j=0;j < number+2;j++){
                        fputc(previous_char, output);
                    }
                    fputc(data, output);
                } else
                    printf("[%d]%c", number, data);
            }else
                out_flag == 1 ? fputc(data, output) : printf("%c", data);
            previous_char = data;
        }else{
            all_part[i] = data;
            i++;
        }
    }
    if(i != 0){
        unsigned int number = trans_to_int(all_part, i);
        if(out_flag){
            for(int j=0;j < number+2;j++)
                fputc(previous_char, output);
        }else
            printf("[%d]", number);
    }
}
