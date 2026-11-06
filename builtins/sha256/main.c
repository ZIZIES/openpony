// help: sha-256 of the arguments
#include "basic.h"
#include "sha256.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: sha256 <text>\n");
        return 1;
    }

    SHA256_CTX ctx;
    BYTE hash[SHA256_BLOCK_SIZE];

    sha256_init(&ctx);
    for (int i = 1; i < argc; i++) {
        if (i > 1) sha256_update(&ctx, (const BYTE *)" ", 1);   /* args joined by a space, like echo */
        sha256_update(&ctx, (const BYTE *)argv[i], strlen(argv[i]));
    }
    sha256_final(&ctx, hash);

    for (int i = 0; i < SHA256_BLOCK_SIZE; i++)
        printf("%02x", hash[i]);
    putchar('\n');
    return 0;
}