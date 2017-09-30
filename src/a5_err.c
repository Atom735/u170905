#include "a5_err.h"

const char *A5ERR(int err) {
    switch(err) {
        case A5E_OK: return "OK";
    } return "???";
}
