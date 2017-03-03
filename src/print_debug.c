#include <stdarg.h>
#include "constants.h"

PRINT_SRC print_src = SERVER;

void PRINT(const char * pString, ...) {
    va_list args;

    switch(print_src) {
        case SERVER:
            break;
        case CLIENT1:
            printf("                                 ");
            break;
        case CLIENT2:
            printf("                                                                  ");
            break;
        case CLIENT3:
            printf("                                                                                          ");
            break;
        case CLIENT4:
            printf("                                                                                                                  ");    
            break;
        default:
            break;
    }
    
    va_start( args, pString );
    vprintf(pString, args);
    va_end(args);
}