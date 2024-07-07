/*
 * This is bin2c program, which allows you to convert binary file to
 * C language array, for use as embedded resource, for instance you can
 * embed graphics or audio file directly into your program.
 * This is public domain software, use it on your own risk.
 * Contact Serge Fukanchik at fuxx@mail.ru  if you have any questions.
 *
 * Some modifications were made by Gwilym Kuiper (kuiper.gwilym@gmail.com)
 * I have decided not to change the licence.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_BZ2
#include <bzlib.h>
#endif

int
main(int argc, char *argv[])
{
#ifdef USE_BZ2
    char *bz2_buf;
    unsigned uncompressed_size, bz2_size;
#endif

    if (argc < 4) {
        fprintf(stderr, "Usage: %s binary_file output_file array_name\n",
                argv[0]);
        return -1;
    }

    FILE *infile = fopen(argv[1], "rb");
    if (infile == NULL) {
        fprintf(stderr, "%s: can't open %s for reading\n", argv[0], argv[1]);
        return -1;
    }

    // Get the file length
    fseek(infile, 0, SEEK_END);
    int file_size = (int)ftell(infile);
    fseek(infile, 0, SEEK_SET);

    char *buf = (char *) malloc(file_size);
    assert(buf);

    fread(buf, file_size, 1, infile);
    fclose(infile);

#ifdef USE_BZ2
    // allocate for bz2.
    unsigned bz2_size =
      (file_size + file_size / 100 + 1) + 600; // as per the documentation

    char *bz2_buf = (char *) malloc(bz2_size);
    assert(bz2_buf);

    // compress the data
    int status =
      BZ2_bzBuffToBuffCompress(bz2_buf, &bz2_size, buf, file_size, 9, 1, 0);

    if (status != BZ_OK) {
        fprintf(stderr, "Failed to compress data: error %i\n", status);
        return -1;
    }

    // and be very lazy
    free(buf);
    unsigned uncompressed_size = file_size;
    file_size = bz2_size;
    buf = bz2_buf;
#endif

    FILE *outfile = fopen(argv[2], "w");
    if (outfile == NULL) {
        fprintf(stderr, "%s: can't open %s for writing\n", argv[0], argv[1]);
        return -1;
    }

    char *ident = strdup(argv[3]);
    while( strchr(ident, '-') ) *strchr(ident, '-') = '_';
    while( strchr(ident, ';') ) *strchr(ident, ';') = '_';

    fprintf(outfile, "// Auto-generated file. Do not edit.\n\n");

    fprintf(outfile, "const char %s[] = {", ident);
    for (int i = 0; i < file_size; ++i) {
        fprintf(outfile, "%s", i == 0 ? "" : ",");
        if ((i % 16) == 0)
            fprintf(outfile, "\n/*%06x*/ ", i);
        fprintf(outfile, "0x%.2x", buf[i] & 0xff);
    }
    fprintf(outfile, "\n};\n\n");

    fprintf(outfile, "const unsigned %s_length = (unsigned)sizeof(%s);\n", ident, ident);

#ifdef USE_BZ2
    fprintf(outfile, "const unsigned %s_length_uncompressed = (unsigned)%i;\n", ident,
            uncompressed_size);
#endif

    fclose(outfile);

    return 0;
}
