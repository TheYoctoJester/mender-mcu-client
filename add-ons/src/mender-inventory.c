/**
 * @file      mender-inventory.c
 * @brief     Mender MCU Inventory add-on implementation
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

#include "mender-api.h"
#include "mender-inventory.h"
#include "mender-log.h"
#include "mender-rtos.h"

#ifdef CONFIG_MENDER_CLIENT_ADD_ON_INVENTORY

/**
 * @brief Default inventory poll interval (seconds)
 */
#define MENDER_INVENTORY_DEFAULT_POLL_INTERVAL (28800)

/**
 * @brief Mender inventory configuration
 */
static mender_inventory_config_t mender_inventory_config;

/**
 * @brief Mender inventory
 */
static mender_inventory_t *mender_inventory       = NULL;
static void *              mender_inventory_mutex = NULL;

/**
 * @brief Mender inventory work handle
 */
static void *mender_inventory_work_handle = NULL;

/**
 * @brief Mender inventory work function
 * @return MENDER_OK if the function succeeds, error code otherwise
 */
static mender_err_t mender_inventory_work_function(void);

mender_err_t
mender_inventory_init(mender_inventory_config_t *config) {

    assert(NULL != config);
    mender_err_t ret;

    /* Save configuration */
    if (NULL == (mender_inventory_config.artifact_name = strdup(config->artifact_name))) {
        mender_log_error("Unable to save artifact name");
        return MENDER_FAIL;
    }
    if (NULL == (mender_inventory_config.device_type = strdup(config->device_type))) {
        mender_log_error("Unable to save device type");
        return MENDER_FAIL;
    }
    if (0 != config->poll_interval) {
        mender_inventory_config.poll_interval = config->poll_interval;
    } else {
        mender_inventory_config.poll_interval = MENDER_INVENTORY_DEFAULT_POLL_INTERVAL;
    }

    /* Create inventory mutex */
    if (MENDER_OK != (ret = mender_rtos_mutex_create(&mender_inventory_mutex))) {
        mender_log_error("Unable to create inventory mutex");
        return ret;
    }

    /* Create mender inventory work */
    mender_rtos_work_params_t inventory_work_params;
    inventory_work_params.function = mender_inventory_work_function;
    inventory_work_params.period   = mender_inventory_config.poll_interval;
    inventory_work_params.name     = "mender_inventory";
    if (MENDER_OK != (ret = mender_rtos_work_create(&inventory_work_params, &mender_inventory_work_handle))) {
        mender_log_error("Unable to create inventory work");
        return ret;
    }

    return MENDER_OK;
}

mender_err_t
mender_inventory_activate(void) {

    mender_err_t ret;

    /* Activate inventory work */
    if (MENDER_OK != (ret = mender_rtos_work_activate(mender_inventory_work_handle))) {
        mender_log_error("Unable to activate inventory work");
        return ret;
    }

    return MENDER_OK;
}

mender_err_t
mender_inventory_set(mender_inventory_t *inventory) {

    mender_err_t ret;

    /* Take mutex used to protect access to the inventory list */
    if (MENDER_OK != (ret = mender_rtos_mutex_take(mender_inventory_mutex, -1))) {
        mender_log_error("Unable to take mutex");
        return ret;
    }

    /* Release previous inventory */
    if (NULL != mender_inventory) {
        size_t index = 0;
        while ((NULL != mender_inventory[index].name) || (NULL != mender_inventory[index].value)) {
            if (NULL != mender_inventory[index].name) {
                free(mender_inventory[index].name);
            }
            if (NULL != mender_inventory[index].value) {
                free(mender_inventory[index].value);
            }
            index++;
        }
        free(mender_inventory);
        mender_inventory = NULL;
    }

    /* Copy the new inventory */
    size_t inventory_length = 0;
    if (NULL != inventory) {
        while ((NULL != inventory[inventory_length].name) && (NULL != inventory[inventory_length].value)) {
            inventory_length++;
        }
    }
    if (NULL == (mender_inventory = (mender_inventory_t *)malloc((inventory_length + 1) * sizeof(mender_inventory_t)))) {
        mender_log_error("Unable to allocate memory");
        mender_rtos_mutex_give(mender_inventory_mutex);
        return MENDER_FAIL;
    }
    memset(mender_inventory, 0, (inventory_length + 1) * sizeof(mender_inventory_t));
    for (size_t index = 0; index < inventory_length; index++) {
        if (NULL == (mender_inventory[index].name = strdup(inventory[index].name))) {
            mender_log_error("Unable to allocate memory");
        }
        if (NULL == (mender_inventory[index].value = strdup(inventory[index].value))) {
            mender_log_error("Unable to allocate memory");
        }
    }

    /* Release mutex used to protect access to the inventory list */
    mender_rtos_mutex_give(mender_inventory_mutex);

    return ret;
}

mender_err_t
mender_inventory_exit(void) {

    /* Deactivate mender inventory work */
    mender_rtos_work_deactivate(mender_inventory_work_handle);

    /* Delete mender inventory work */
    mender_rtos_work_delete(mender_inventory_work_handle);
    mender_inventory_work_handle = NULL;

    /* Release memory */
    if (NULL != mender_inventory_config.artifact_name) {
        free(mender_inventory_config.artifact_name);
        mender_inventory_config.artifact_name = NULL;
    }
    if (NULL != mender_inventory_config.device_type) {
        free(mender_inventory_config.device_type);
        mender_inventory_config.device_type = NULL;
    }
    mender_inventory_config.poll_interval = 0;
    if (NULL != mender_inventory) {
        size_t index = 0;
        while ((NULL != mender_inventory[index].name) || (NULL != mender_inventory[index].value)) {
            if (NULL != mender_inventory[index].name) {
                free(mender_inventory[index].name);
            }
            if (NULL != mender_inventory[index].value) {
                free(mender_inventory[index].value);
            }
            index++;
        }
        free(mender_inventory);
        mender_inventory = NULL;
    }
    mender_rtos_mutex_delete(mender_inventory_mutex);

    return MENDER_OK;
}

static mender_err_t
mender_inventory_work_function(void) {

    mender_err_t ret;

    /* Take mutex used to protect access to the inventory list */
    if (MENDER_OK != (ret = mender_rtos_mutex_take(mender_inventory_mutex, -1))) {
        mender_log_error("Unable to take mutex");
        return ret;
    }

    /* Publish inventory */
    if (MENDER_OK != (ret = mender_api_publish_inventory_data(mender_inventory))) {
        mender_log_error("Unable to publish inventory data");
    }

    /* Release mutex used to protect access to the inventory list */
    mender_rtos_mutex_give(mender_inventory_mutex);

    return ret;
}

#endif /* CONFIG_MENDER_CLIENT_ADD_ON_INVENTORY */
