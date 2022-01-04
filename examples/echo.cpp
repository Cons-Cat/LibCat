#include <linux>

void meow() {
    i4 argc = load_argc();
    char8_t** argv = load_argv();
    for (i4 i = 1; i < argc; i++) {
        i4 length = 0;
        while (argv[i][length] != 0) {
            length++;
        }
        argv[i][length] = ' ';
        write(1, argv[i], length + 1).discard_result();
    }
    write(1, u8"\n", 1).discard_result();
    exit(0);
}
