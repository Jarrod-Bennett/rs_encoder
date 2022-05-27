//
// Encode messages as a Reed-Solomon code. This library is designed to be
// lightweight for use on resource constrained devices. Increased space is
// traded for reduced computations.
//
// You can read about Reed-Solomon codes at
// https://en.wikipedia.org/wiki/Reed%E2%80%93Solomon_error_correction
//
// Up to a maximum of 8-bit symbols is (theoretically) supported as 8-bit
// integers are used for space efficiency. In practice, only up to 5-bit
// symbols are supported for space efficiency with up to 8 parity symbols
// (though this number is more flexible and can be reconfigured by altering
// the corresponding #define with very little space consequences)
//
// This implementation is based off of the MATLAB rsenc() function. Many
// components including the generator polynomial are fixed and have been
// precomputed via a MATLAB script in order to reduce encoding complexity.
// The API does not support configurable generator polynomials or similar.
// @ref https://au.mathworks.com/help/comm/ref/rsenc.html
//
// @author Jarrod Bennett
//

#include "rs_encoder.h"

static int galois_field_add(int m, int l, int r);

// Multiply an element l by the corresponding generator polynomial element
// genIndex based on m-bits per symbol galois multiplication.
// Returns the generated galois value.
static int galois_field_multiply_generator(int m, int l, int genIndex);

int rs_encode_message(const uint8_t * msg, int k, int t, int m,
                      /* no restrict? */ uint8_t * parity) {

    // TODO: restrict
    // TODO: since the msg appears in the codeword (systematic code) no need to
    //       include message in code output variable

    // Encoded code length
    int n = k + t;

    // We need to calculate the full length of RS code with length n since RS
    // operates on blocks of sizes of powers of 2. We therefore calculate how
    // much the codeword must be shortened (padded with 0s) for this.
    int blockSize = 1 << m; // Quick and dirty 2^m w/o math libraries/floats
    int shortened = (blockSize - 1) - n;

    uint8_t msgZ[RS_MAX_PARITY_SYMBOLS + RS_MAX_MESSAGE_LENGTH] = {0};

    // Pad the message with leading 0s as above
    for (int i = 0; i < shortened; i++) {
        msgZ[i] = 0;
    }

    // Copy the message
    for (int i = 0; i < k; i++) {
        msgZ[i + shortened] = msg[i];
    }

    // Perform the RS encoding process by implementing the following MATLAB
    // code behaviour:
    // for j=1:size(msgZ,2) = 1:shortened+length(msg)
    //      parity = [parity(2:T2) zeros(1)] + (msgZ(j)+parity(1))*genpoly;
    // end
    // Note the addition and multiplication on the RHS are /* galois */ ops
    for (int i = 0; i < shortened + k; i++) {
        // compute the RHS

        int rhs[RS_MAX_PARITY_SYMBOLS] = {0};
        int msgZJPlusParity1 = galois_field_add(m, msgZ[i], parity[0]);

        for (int j = 0; j < m; j++) {
            rhs[j] = galois_field_multiply_generator(m, msgZJPlusParity1, j);
        }

        // Sum the LHS and the RHS
        for (int j = 0; j < m - 1; j++) {
            parity[j] = galois_field_add(m, parity[j + 1], rhs[j]);
        }
        parity[m - 1] = galois_field_add(m, 0, rhs[m - 1]);
    }

    return 0;
}

static const uint8_t GALOIS_PRODUCTS_4[16][4] = {
        0,	0,	0,	0,
        13,	12,	8,	7,
        9,	11,	3,	14,
        4,	7,	11,	9,
        1,	5,	6,	15,
        12,	9,	14,	8,
        8,	14,	5,	1,
        5,	2,	13,	6,
        2,	10,	12,	13,
        15,	6,	4,	10,
        11,	1,	15,	3,
        6,	13,	7,	4,
        3,	15,	10,	2,
        14,	3,	2,	5,
        10,	4,	9,	12,
        7,	8,	1,	11,
};

static const uint8_t GALOIS_ADDITIONS_4[16][16] = {
        0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
        1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14,
        2,3,0,1,6,7,4,5,10,11,8,9,14,15,12,13,
        3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12,
        4,5,6,7,0,1,2,3,12,13,14,15,8,9,10,11,
        5,4,7,6,1,0,3,2,13,12,15,14,9,8,11,10,
        6,7,4,5,2,3,0,1,14,15,12,13,10,11,8,9,
        7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8,
        8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,
        9,8,11,10,13,12,15,14,1,0,3,2,5,4,7,6,
        10,11,8,9,14,15,12,13,2,3,0,1,6,7,4,5,
        11,10,9,8,15,14,13,12,3,2,1,0,7,6,5,4,
        12,13,14,15,8,9,10,11,4,5,6,7,0,1,2,3,
        13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2,
        14,15,12,13,10,11,8,9,6,7,4,5,2,3,0,1,
        15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,
};

static int galois_field_add(int m, int l, int r) {

    switch (m) {
        case 4:
            return GALOIS_ADDITIONS_4[l][r];
        case 5:
            // TODO:
            return 0;
        default:
            /* unimplemented error or unreachable()! */
//            assert(0);
            return -1;
    }
}

static int galois_field_multiply_generator(int m, int l, int genIndex) {

    switch (m) {
        case 4:
            return GALOIS_PRODUCTS_4[l][genIndex];
        case 5:
            // TODO:
            return 0;
        default:
            /* unimplemented error or unreachable()! */
//            assert(0);
            return -1;
    }
}