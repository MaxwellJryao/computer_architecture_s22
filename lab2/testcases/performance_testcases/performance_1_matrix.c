/**
 * Compute the product of two WxW matricies. 
 * NOTE: In this program, we provide our own startup code and do not rely on any
 * C standard library features.
 * As such, this program compiles safely with the "-nostdlib" linker flag.
 */

asm("li sp, 0x100000"); // SP set to 1 MB
asm("jal main");        // call main
asm("mv a1, a0");       // save return value in a1
asm("li a7, 10");       // prepare ecall exit
asm("ecall");           // now your simulator should stop

#define W 10            // Matrix order

void mmul(const int a[W][W], const int b[W][W], int c[W][W]) {
	for(int row = 0; row < W; row++) {
        for(int col = 0; col < W; col++) {
            for(int k = 0; k < W; k++) {
                if (a[row][k] > 2 * b[k][col]) {
                    c[row][col] += a[row][k] * b[k][col];
                }
                if (a[row][k] <= 2 * b[k][col]) {
                    c[row][col] += a[row][k] + b[k][col];
                }
            }
            printf("%d ", c[row][col]);
        }
        printf("\n");
    }
}

int main() {
	int A[W][W];
    int B[W][W];
	int C[W][W];
    for(int row = 0; row < W; row++) {
        for(int col = 0; col < W; col++) {
            A[row][col] = row + col;
            B[col][row] = row - col;
        }
    }

    mmul(A, B, C);
    return 0;
}
