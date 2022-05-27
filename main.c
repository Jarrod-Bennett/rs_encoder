#include <stdio.h>
#include <stdint.h>
#include "rs_encoder.h"

int main() {

    int k = 10;
    int t = 4;

    uint8_t msgBuffer16fsk[14] = {0x2, 0x5, 0x6, 0x6, 0x0, 0xB, 0xF, 0xC, 0x1, 0xB};

    int err = rs_encode_message(msgBuffer16fsk, k, t, 4, &msgBuffer16fsk[10]);
    if (err) {
        // error
        fprintf(stderr, "Error encoding RS message, code = %d\n", err);
    }

    // success
    printf("Successfully encoded message. Message = \n0x");
    for (int i = 0; i < k + t; i++) {
        uint8_t elem = msgBuffer16fsk[i];
        if (i < k + t - 1) {
            printf("%x ", elem);
        } else {
            printf("%x\n", elem);
        }
    }

    return 0;
}
