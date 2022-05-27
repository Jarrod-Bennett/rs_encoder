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

#ifndef RS_ENCODER_H
#define RS_ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Maximum bits per symbol supported.
#define RS_MAX_SYMBOL_SIZE      (5)

// Maximum number of parity symbols. Choose a smaller number to save (minimal)
// space
#define RS_MAX_PARITY_SYMBOLS   (8)

// Maximum message length. Again, smaller max message size chosen for space
// reasons
#define RS_MAX_MESSAGE_LENGTH   (16)

// Encode a message as a Reed-Solomon code. Message should be provided as a
// uint8_t array of the message symbols to encode. The symbol size of the
// message elements must not be larger than the symbol size of the RS code.
// Since the RS coding is a systematic codeword (the message appears as a
// subsequence of the codeword) only the parity symbols are stored in the
// code buffer. The user can then set arrange the parity symbols as desired.
//
// @param   msg: the message to be encoded.
// @param   k: the number of symbols in the message.
// @param   t: the number of errors that can be corrected. For standard (non-
//             erasure) RS codes, 2t symbols are appended for t corrections.
// @param   m: the symbol size in bits per symbol. Only m == 4 or m == 5 are
//             supported.
// @param   code: the parity symbol buffer. This must be able to contain at
//                least t elements in order for the buffer to not overflow.
// @return  0 if the message was successfully encoded, otherwise non-zero if an
//          error occurred.
int rs_encode_message(const uint8_t * msg, int k, int t, int m,
                      uint8_t * code);

#ifdef __cplusplus
}
#endif

#endif //RS_ENCODER_H
