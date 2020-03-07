#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned char u8;
typedef unsigned long ulong;

void createOut(u8 numb, const char* src, FILE** out) {
    static char* dst = NULL;

    if (dst != NULL)
        free(dst);

    dst = malloc((strlen(src) + 4) * sizeof(char));

    sprintf(dst, "%s.%02d", src, numb);
    printf("%s...\n", dst);

    *out = fopen(dst, "wb");
}

int split(const char* src, ulong blocksize, ulong splitsize) {
    FILE* in, * out;
    ulong progresscounter = 0;
    u8 splitnumb = 0;

    if ((in = fopen(src, "rb")) == NULL)
        return -1;

    size_t readlen = 0;
    u8* buff = NULL;

    buff = calloc(blocksize, sizeof(u8));
    createOut(splitnumb++, src, &out);

    while ((readlen = fread(buff, 1, blocksize, in)) > 0) {
        fwrite(buff, 1, readlen, out);
        progresscounter += readlen;

        printf("Writing: [%lu%%]\r", (progresscounter / (splitsize / (ulong)100)));

        if (progresscounter >= splitsize) {
            fclose(out);

            createOut(splitnumb++, src, &out);
            progresscounter = 0;
        }
    }

    fclose(in);
    fclose(out);

    return 0;
}

int main(int argc, char** argv)
{
    int ret = 0;
    ulong splitsize;

    if (argc < 3) {
        printf("Too little arguments given.\nUsage: Splitfiles.exe [Path] [Splitsize in GB]");
        return -1;
    }

    if ((splitsize = atoi(argv[2])) == 0) {
        printf("Parsing failed");
        return -1;
    }

    splitsize *= 0x400 * 0x400 * 0x400;

    if ((ret = split(argv[1], 0x400 * 0x400 * 64, splitsize)))
        printf("Failed to open in file");
    else
        printf("Done!              ");
}