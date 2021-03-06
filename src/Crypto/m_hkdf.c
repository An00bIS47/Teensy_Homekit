//
// m_hkdf.c
// Teensy_Homekit
//
//  Created on: 10.05.2021
//      Author: michael
//
#include "m_hkdf.h"
#include <mbedtls/hkdf.h>
#include <stdio.h>


struct hkdf_salt_info {
    char* salt;
    char* info;
};

static struct hkdf_salt_info _hkdf_salt_info[] PROGMEM = {
    {   /* HKDF_KEY_TYPE_PAIR_SETUP_ENCRYPT */
        .salt = "Pair-Setup-Encrypt-Salt",
        .info = "Pair-Setup-Encrypt-Info",
    },
    {   /* HKDF_KEY_TYPE_PAIR_SETUP_CONTROLLER */
        .salt = "Pair-Setup-Controller-Sign-Salt",
        .info = "Pair-Setup-Controller-Sign-Info",
    },
    {   /* HKDF_KEY_TYPE_PAIR_SETUP_ACCESSORY */
        .salt = "Pair-Setup-Accessory-Sign-Salt",
        .info = "Pair-Setup-Accessory-Sign-Info",
    },
    {   /* HKDF_KEY_TYPE_PAIR_VERIFY_ENCRYPT */
        .salt = "Pair-Verify-Encrypt-Salt",
        .info = "Pair-Verify-Encrypt-Info",
    },
    {   /* HKDF_KEY_TYPE_CONTROL_READ */
        .salt = "Control-Salt",
        .info = "Control-Read-Encryption-Key",
    },
    {   /* HKDF_KEY_TYPE_CONTROL_WRITE */
        .salt = "Control-Salt",
        .info = "Control-Write-Encryption-Key",
    }
};

static struct hkdf_salt_info* _salt_info_get(enum hkdf_key_type type)
{
    return &_hkdf_salt_info[type];
}


int hkdf_key_get(enum hkdf_key_type type, uint8_t* inkey, int inkey_len, uint8_t* outkey)
{
    struct hkdf_salt_info* salt_info = _salt_info_get(type);
    const mbedtls_md_info_t *sha512_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA512);

    int err = mbedtls_hkdf(sha512_info,(const uint8_t*)salt_info->salt,strlen(salt_info->salt),inkey,
    inkey_len,(const uint8_t*)salt_info->info,strlen(salt_info->info), outkey, HKDF_KEY_LEN);

    return err;
}