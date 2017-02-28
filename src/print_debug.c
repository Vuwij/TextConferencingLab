#include <stdarg.h>
#include "constants.h"

PRINT_SRC print_src = SERVER;

void print_debug(const char * pString, ...) {
    va_list args;

    switch(print_src) {
        case SERVER:
            vprintf(pString, args);
            break;
        case CLIENT1:
            vprintf("                       ", args);
            vprintf(pString, args);
            break;
        case CLIENT2:
            vprintf("                                              ", args);
            vprintf(pString, args);
            break;
        case CLIENT3:
            vprintf("                                                                      ", args);
            vprintf(pString, args);
            break;
        case CLIENT4:
            vprintf("                                                                                              ", args);
            vprintf(pString, args);
            break;
        default:
            break;
    }
    va_end(args);
}