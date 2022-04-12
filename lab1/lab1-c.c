#ifdef COMPILE_X86
#include <stdio.h>
#include <stdlib.h>
#endif

#define MAXN 100
#define MAXM 100
unsigned int n = 0, m = 0;
typedef unsigned int pixel;
int img[MAXN * MAXM];
pixel result_img[MAXN * MAXM];
//--------------------------------------- !!! TODO: FILL IN YOUR FILE PATH !!! ---------------------------------------------
char read_path[] = "/home/arch_stu2020011031/yao-archlab-s22/lab1/sample/turing_before.pixel";
char write_path[] = "/home/arch_stu2020011031/yao-archlab-s22/lab1/sample/c_result.pixel";
//------------------------------------------------------------------------------------------------------------------

void _printf_num(int num);
void _printf_char(char ch);
void _write_int(char* file_name, int* byte_address, int length) ;
void _return();
void image_process();
void image_input();
void image_output();
void image_to_file();

int main() {
    image_input();
    image_process();
    image_output();
    image_to_file();
    _return();
}

void _printf_num(int num) {
    // DO NOT MODIFY THIS!!!
#ifdef COMPILE_X86
    printf("%d", num);
#else
    __asm__ __volatile__ (
        "li a7, 1\n\t"
        "mv a0, %[print_num]\n\t"
        "ecall \n\t"
        : /* no output*/
        : [print_num]"r"(num)
        : "a0", "a7"
    );
#endif
}

void _printf_char(char ch) {
    // DO NOT MODIFY THIS!!!
#ifdef COMPILE_X86
    printf("%c", ch);
#else
    __asm__ __volatile__ (
        "li a7, 11\n\t"
        "mv a0, %[print_num]\n\t"
        "ecall \n\t"
        : /* no output*/
        : [print_num]"r"(ch)
        : "a0", "a7"
    );
#endif
}

void _return() {
#ifdef COMPILE_X86
    exit(0);
#else
    // DO NOT MODIFY THIS!!!
    __asm__ __volatile__ (
        "li a7, 10\n\t"
        "ecall\n\t"
        : /* no output*/
        : /* no input */
        : "a7"
    );
#endif
}

void image_process() {
    //--------------------------------------- TODO: FILL IN CODE HERE ---------------------------------------
    // Image border (first and last row, column) is not considered.
    // The input data is in img[]. The output data should be in result_img[].
    int K[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}}; // Te kernel matrix
    /* for (int x = 0; x < n; ++x) {
        result_img[x*m] = img[x*m];
        result_img[x*m+m-1] = img[x*m+m-1];
    }
    int k = (n-1)*m;
    for (int y = 1; y < m-1; ++y) {
        result_img[y] = img[y];
        result_img[k+y] = img[k+y];
    } */
    for (int x = 1; x < n - 1; ++x)
        for (int y = 1; y < m - 1; ++y) {
            result_img[x*m+y] = 0;
            if (x == 0 || x == n-1 || y == 0 || y == m-1) {
                result_img[x*m+y] = img[x*m+y];
                continue;
            }
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    result_img[x*m+y] += img[(x+i-1)*m+y+j-1]*K[i][j];
            result_img[x*m+y] /= 16;
        }
    return;
    //-------------------------------------------------------------------------------------------------------
}

void image_input() {
    // DO NOT MODIFY THIS!!!
#ifdef COMPILE_X86
    FILE* file = fopen(read_path, "r");
    fscanf(file, "%d", &n);
    fscanf(file, "%d", &m);
    int i = 0;
    while (fscanf(file, "%d", &img[i]) == 1) {
        ++i;
        if (i == n * m)
            break;
    }
    fclose(file);
#else
    int size[2];
    __asm__ __volatile__ (
        "li a7, 1024\n\t"
        "li a1, 0\n\t"
        "mv a0, %[path]\n\t"
        "ecall\n\t"
        "mv t3, a0\n\t"
        "li a7, 65\n\t"
        "mv a1, %[size_addr]\n\t"
        "li a2, 2\n\t"
        "ecall\n\t"
        "li a7, 65\n\t"
        "mv a0, t3\n\t"
        "mv a1, %[addr]\n\t"
        "mv a2, %[len]\n\t"
        "ecall\n\t"
        "li a7, 57\n\t"
        "mv a0, t3\n\t"
        "ecall\n\t"
        : 
        : [path]"r"(read_path), [addr]"r"(img), [len]"r"(MAXN * MAXM), [size_addr]"r"(size)
        : "a0", "a1", "a2", "t3", "a7"
    );
    n = size[0];
    m = size[1];
#endif
}

void image_output() {
    // DO NOT MODIFY THIS!!!
    _printf_num(n);
    _printf_char(' ');
    _printf_num(m);
    _printf_char('\n');
    int col = 0;
    for (int i = 0; i < n * m; ++i) {
        _printf_num(result_img[i]); 
        _printf_char(' ');
        ++col;
        if (col == m) {
            col = 0;
            _printf_char('\n');
        }
    }
}

void image_to_file() {
    // DO NOT MODIFY THIS!!!
#ifdef COMPILE_X86
    FILE* file = fopen(write_path, "w");
    fprintf(file, "%d %d\n", n, m);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j)
            fprintf(file, "%d ", result_img[i * m + j]);
        fprintf(file, "\n");
    }
    fclose(file);
#else    
    int size[2];
    size[0] = n;
    size[1] = m;
    __asm__ __volatile__ (
        "li a7, 1024\n\t"
        "li a1, 1\n\t"
        "mv a0, %[path]\n\t"
        "ecall\n\t"
        "mv t3, a0\n\t"

        "li a7, 66\n\t"
        "mv a1, %[size_addr]\n\t"
        "li a2, 2\n\t"
        "ecall\n\t"

        "mv t2, %[addr]\n\t"
        "addi t4, zero, 0\n\t"
    "OUTPUT_ONE_LINE: \n\t"
        "mv a0, t3\n\t"
        "mv a1, t2\n\t"
        "mv a2, %[m]\n\t"
        "li a7, 66\n\t"
        "ecall\n\t"

        "slli a3, %[m], 2\n\t"
        "add t2, t2, a3\n\t"
        "addi t4, t4, 1\n\t"
        "blt t4, %[n], OUTPUT_ONE_LINE\n\t"

        "li a7, 57\n\t"
        "mv a0, t3\n\t"
        "ecall\n\t"
        : /*no output*/
        : [path]"r"(write_path), [addr]"r"(result_img), [n]"r"(n), [m]"r"(m), [size_addr]"r"(size)
        : "a0", "a1", "a2", "a3", "t2", "t3", "t4", "a7"
    );
#endif
}
