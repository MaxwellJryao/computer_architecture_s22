void _printf_num(int num);
void _printf_char(char ch);
void _read_int(char* file_name, int* byte_address, int length);
void _return();
// --------------------------------------- !!! TODO: FILL IN YOUR FILE PATH !!! ---------------------------------------------
char read_path[] = "/home/your_user_name/yao-archlab-s22/lab1/run/simple_add_data.txt";
//------------------------------------------------------------------------------------------------------------------


int main() {
    int data[2];
    _read_int(read_path, data, 2);
    int res = data[0] + data[1];
    _printf_num(res);
    _return();
}

void _return() {
    // DO NOT MODIFY THIS!!!
    __asm__ __volatile__ (
        "li a7, 10\n\t"
        "ecall\n\t"
        : /* no output*/
        : /* no input */
        : "a7"
    );
}

void _printf_num(int num) {
    __asm__ __volatile__ (
        "li a7, 1\n\t"
        "mv a0, %[print_num]\n\t"
        "ecall \n\t"
        : /* no output*/
        : [print_num]"r"(num)
        : "a0", "a7"
    );
}

void _printf_char(char ch) {
    __asm__ __volatile__ (
        "li a7, 11\n\t"
        "mv a0, %[print_num]\n\t"
        "ecall \n\t"
        : /* no output*/
        : [print_num]"r"(ch)
        : "a0", "a7"
    );
}

void _read_int(char* file_name, int* byte_address, int length) {
    __asm__ __volatile__ (
        "li a7, 1024\n\t"
        "li a1, 0\n\t"
        "ecall\n\t"
        "mv t3, a0\n\t"
        "li a7, 65\n\t"
        "mv a1, %[addr]\n\t"
        "mv a2, %[len]\n\t"
        "ecall\n\t"
        "li a7, 57\n\t"
        "mv a0, t3\n\t"
        "ecall\n\t"
        : /*no output*/
        : [addr]"r"(byte_address), [len]"r"(length)
        : "a0", "a1", "a2", "t3", "a7"
    );
}