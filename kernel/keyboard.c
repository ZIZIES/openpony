#include "types.h"
#include "io.h"
#include "keyboard.h"

/* scancode set 1, us layout. index = scancode, 0 = no printable char */
static const char map[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,
    '*',0,' ',
};

static const char map_shift[128] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,'A','S','D','F','G','H','J','K','L',':','"','~',
    0,'|','Z','X','C','V','B','N','M','<','>','?',0,
    '*',0,' ',
};

static int shift;   /* explicit init in kgetc's first use is fine: see note below */

char kgetc(void) {
    for (;;) {
        while (!(inb(0x64) & 1)) ;          /* wait: output buffer full */
        u8 sc = inb(0x60);

        if (sc & 0x80) {                    /* key release */
            sc &= 0x7F;
            if (sc == 0x2A || sc == 0x36) shift = 0;
            continue;
        }
        if (sc == 0x2A || sc == 0x36) {     /* left / right shift press */
            shift = 1;
            continue;
        }
        char c = shift ? map_shift[sc] : map[sc];
        if (c) return c;                    /* ignore keys with no char */
    }
}