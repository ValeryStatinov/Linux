#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>

int main() {
    printf("Invoking system call...\n");
    char surname[] = "Statinov";
    long int ret_status = syscall(329, surname, strlen(surname));
    if (ret_status == 0) {
        printf("Invoked, use dmesg to see result\n");
    }
    else {
        printf("Syscall did not executed as expected\n");
    }
    return 0;
}