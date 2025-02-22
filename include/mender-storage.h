/**
 * @file      mender-storage.h
 * @brief     Mender storage interface
 *
 * MIT License
 *
 * Copyright (c) 2022-2023 joelguittet and mender-mcu-client contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __MENDER_STORAGE_H__
#define __MENDER_STORAGE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "mender-common.h"

/**
 * @brief Initialize mender storage
 * @return MENDER_OK if the function succeeds, error code otherwise
 */
mender_err_t mender_storage_init(void);

/**
 * @brief Erase authentication keys
 * @return MENDER_OK if the function succeeds, error code otherwise
 */
mender_err_t mender_storage_erase_authentication_keys(void);

/**
 * @brief Get authentication keys
 * @param private_key Private key from storage, NULL if not found
 * @param private_key_length Private key length from storage, 0 if not found
 * @param public_key Public key from storage, NULL if not found
 * @param public_key_length Public key length from storage, 0 if not found
 * @return MENDER_OK if the function succeeds, error code otherwise
 */
mender_err_t mender_storage_get_authentication_keys(unsigned char **private_key,
                                                    size_t *        private_key_length,
                                                    unsigned char **public_key,
                                                    size_t *        public_key_length);

/**
 * @brief Set authentication keys
 * @param private_key Private key to store
 * @param private_key_length Private key length
 * @param public_key Public key to store
 * @param public_key_length Public key length
 * @return MENDER_OK if the function succeeds, error code otherwise
 */
mender_err_t mender_storage_set_authentication_keys(unsigned char *private_key, size_t private_key_length, unsigned char *public_key, size_t public_key_length);

/**
 * @brief Get OTA deployment
 * @param ota_id OTA ID from storage, NULL if not found
 * @param ota_id_length OTA ID length from storage, 0 if not found
 * @param ota_artifact_name Artifact name from storage, NULL if not found
 * @param ota_artifact_name_length Artifact name length from storage, 0 if not found
 * @return MENDER_OK if the function succeeds, error code otherwise
 */
mender_err_t mender_storage_get_ota_deployment(char **ota_id, size_t *ota_id_length, char **ota_artifact_name, size_t *ota_artifact_name_length);

/**
 * @brief Set OTA deployment
 * @param ota_id OTA ID to store
 * @param ota_artifact_name Artifact name to store
 * @return MENDER_OK if the function succeeds, error code otherwise
 */
mender_err_t mender_storage_set_ota_deployment(char *ota_id, char *ota_artifact_name);

/**
 * @brief Erase OTA deployment
 * @return MENDER_OK if the function succeeds, error code otherwise
 */
mender_err_t mender_storage_clear_ota_deployment(void);

/**
 * @brief Release mender storage
 * @return MENDER_OK if the function succeeds, error code otherwise
 */
mender_err_t mender_storage_exit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MENDER_STORAGE_H__ */
