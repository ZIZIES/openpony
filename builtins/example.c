/*

    when compiling your own image, you can add your own programs!
    you can make a folder with your source files (/builtins/fetch is an example), with the foldername as what you will call in the tty
    you can also just make a .c file with the name of the command you will call in the tty

    whatever is in builtins will be compilied with the os.
    openpont has no persistent memory or /bin like your probably used to, though it may be a cool thing in the future!

*/

// the basic include header file. check it out, if it doesn't have what you need, add your own!
#include "basic.h"

// now you can just write C as you would for posix
int main(void) {
    long int exampleInt = 33483294;
    char example[] = "";
    printf("this is an example program!\n");
    printf("example number: %ld\n", exampleInt);
    printf("example string/char array: %s\n", example);
    return 0;
}