#include <math.h>
#include <gcrypt.h>
#include <gpg-error.h>

// Symmetric cryptography available ciphers https://gnupg.org/documentation/manuals/gcrypt.pdf
#define CRYPTO_ALG GCRY_CIPHER_BLOWFISH
// Symmetric cryptography available cipher modes https://gnupg.org/documentation/manuals/gcrypt.pdf#page=35&zoom=100,120,96
#define MODE_OF_OP GCRY_CIPHER_MODE_CBC

int main()
{
    gcry_cipher_hd_t cipher_hd;
    gpg_error_t err;

    // Get size of key and iv for specified cryptography algorithm
    size_t KEY_SIZE = gcry_cipher_get_algo_keylen(CRYPTO_ALG);
    size_t BLOCK_SIZE = gcry_cipher_get_algo_blklen(CRYPTO_ALG);
    unsigned char* key = (unsigned char*)malloc(KEY_SIZE);
    gcry_randomize(key, KEY_SIZE, GCRY_WEAK_RANDOM);
    unsigned char* iv = (unsigned char*)malloc(BLOCK_SIZE);
    gcry_randomize(iv, BLOCK_SIZE, GCRY_WEAK_RANDOM);

    // plaintext is taken from https://en.wikipedia.org/wiki/Advanced_Encryption_Standard#Description_of_the_ciphers
    unsigned char* plaintext =
        (unsigned char*)"AES is based on a design principle known as a substitution¡Vpermutation network, and is efficient in both software and hardware. Unlike its predecessor DES, AES does not use a Feistel network. AES is a variant of Rijndael, with a fixed block size of 128 bits, and a key size of 128, 192, or 256 bits. By contrast, Rijndael per se is specified with block and key sizes that may be any multiple of 32 bits, with a minimum of 128 and a maximum of 256 bits.\x00";
    int text_len = ceil((double)strlen((char*)plaintext) / BLOCK_SIZE) * BLOCK_SIZE;
    unsigned char* ciphertext = (unsigned char*)malloc(text_len);
    unsigned char* decryptedtext = (unsigned char*)malloc(text_len);


    // Setup encryption handle, key and iv
    // The fourth argument ```flags``` can be passed as 0 or as the bit-wise OR of the following constants
    // GCRY_CIPHER_SECURE: Make sure that all operations are allocated in secure memory
    // GCRY_CIPHER_ENABLE_SYNC
    // GCRY_CIPHER_CBC_CTS: Enable cipher text stealing (CTS) for the CBC mode
    // GCRY_CIPHER_CBC_MAC: Compute CBC-MAC keyed checksums
    err = gcry_cipher_open(&cipher_hd, CRYPTO_ALG, MODE_OF_OP, GCRY_CIPHER_CBC_CTS);
    err = gcry_cipher_setkey(cipher_hd, key, KEY_SIZE);
    err = gcry_cipher_setiv(cipher_hd, iv, BLOCK_SIZE);

    // Encryption
    err = gcry_cipher_encrypt(cipher_hd, ciphertext, text_len, plaintext, text_len);

    // Decryption
    // Remember to reset iv
    gcry_cipher_setiv(cipher_hd, iv, BLOCK_SIZE);
    err = gcry_cipher_decrypt(cipher_hd, decryptedtext, text_len, ciphertext, text_len);

    printf("%s", decryptedtext);
    gcry_cipher_close(cipher_hd);
    return 0;
}