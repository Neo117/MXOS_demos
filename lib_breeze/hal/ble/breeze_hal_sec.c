/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <stdint.h>
#include <breeze_hal_sec.h>
//#include "ali_crypto.h"
#include "mdebug.h"
#include "mxos_security.h"

#define app_log(format, ...)  custom_log("aes", format, ##__VA_ARGS__)

#define KEY_LEN 16

void *ais_aes128_init(const uint8_t *key, const uint8_t *iv)
{
    Aes *aes_ctx;
    uint8_t          *p;

    size_t alloc_siz = sizeof(Aes) + KEY_LEN * 2 + sizeof(bool);
    aes_ctx = malloc(alloc_siz);
    if (aes_ctx == NULL) {
        app_log("kmalloc(%d) fail", (int)sizeof(Aes));
        return NULL;
    }
    memset(aes_ctx, 0, alloc_siz);

    p = (uint8_t *)aes_ctx + sizeof(Aes);
    memcpy(p, key, KEY_LEN);
    p += KEY_LEN;
    memcpy(p, iv, KEY_LEN);

    return aes_ctx;
}

int ais_aes128_destroy(void *aes)
{
    if (aes) {
        free(aes);
    }

    return 0;
}

static int platform_aes128_encrypt_decrypt(void *aes_ctx, const void *src,
                                           size_t siz, void *dst, bool is_enc)
{
    size_t            dlen, in_len = siz, ctx_siz;
    uint8_t          *p, *key, *iv;
    if (aes_ctx == NULL) {
        app_log("platform_aes128_encrypt_decrypt aes_ctx is NULL");
        return -1;
    }

    ctx_siz = sizeof(Aes);

    p   = (uint8_t *)aes_ctx + ctx_siz;
    key = p;
    p += KEY_LEN;
    iv = p;

    in_len <<= 4;
    dlen = in_len;

    if(is_enc){
        AesSetKey(aes_ctx, key, AES_BLOCK_SIZE, iv, AES_ENCRYPTION);
        AesCbcEncrypt(aes_ctx, dst, src, dlen);
    }
    else{
        AesSetKey(aes_ctx, key, AES_BLOCK_SIZE, iv, AES_DECRYPTION);
        AesCbcDecrypt(aes_ctx, dst, src, dlen);
    }

    return 0;
}

int ais_aes128_cbc_encrypt(void *aes, const void *src, size_t block_num,
                           void *dst)
{
    return platform_aes128_encrypt_decrypt(aes, src, block_num, dst, true);
}

int ais_aes128_cbc_decrypt(void *aes, const void *src, size_t block_num,
                           void *dst)
{
    return platform_aes128_encrypt_decrypt(aes, src, block_num, dst, false);
}
