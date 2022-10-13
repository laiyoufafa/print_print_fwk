/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "print_callback.h"
#include <uv.h>
#include "napi_print_utils.h"
#include "print_log.h"

namespace OHOS::Print {
struct CallbackParam {
    napi_env env;
    napi_ref ref;
    std::mutex* mutexPtr;
    uint32_t state;
    PrinterInfo printerInfo;
    PrintJob jobInfo;

    std::string extensionId;
    std::string info;
};

PrintCallback::PrintCallback(napi_env env, napi_ref ref) : env_(env), ref_(ref)
{
}

PrintCallback::~PrintCallback()
{
    std::lock_guard<std::mutex> autoLock(mutex_);
    PRINT_HILOGD("callback has been destroyed");
    NapiPrintUtils::DeleteReference(env_, ref_);
    ref_ = nullptr;
}

bool PrintCallback::OnCallback()
{
    PRINT_HILOGD("Print Task callback in");
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        PRINT_HILOGE("Failed to get uv event loop");
        return false;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        PRINT_HILOGE("Failed to create uv work");
        return false;
    }
    CallbackParam *param = new (std::nothrow) CallbackParam;
    if (param == nullptr) {
        PRINT_HILOGE("Failed to create callback parameter");
        return false;    
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        param->env = env_;
        param->ref = ref_;
        param->mutexPtr = &mutex_;
    }
    work->data = param;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int statusInt) {
            CallbackParam *cbParam = static_cast<CallbackParam*>(work->data);
            if (cbParam != nullptr) {
                std::lock_guard<std::mutex> autoLock(*cbParam->mutexPtr);
                napi_value callbackFunc = NapiPrintUtils::GetReference(cbParam->env, cbParam->ref);
                napi_value callbackResult = nullptr;
                napi_value callbackValues[NapiPrintUtils::ARGC_ONE] = { 0 };
                callbackValues[NapiPrintUtils::ARGC_ZERO] = NapiPrintUtils::GetUndefined(cbParam->env);
                napi_call_function(cbParam->env, nullptr, callbackFunc, NapiPrintUtils::ARGC_ZERO,
                    callbackValues, &callbackResult);
                if (work != nullptr) {
                    delete work;
                    work = nullptr;
                }
                delete cbParam;
                cbParam = nullptr;
            }
        });
    return true;
}

bool PrintCallback::OnCallback(uint32_t state, const PrinterInfo &info)
{
    PRINT_HILOGD("Print Task callback in");
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        PRINT_HILOGE("Failed to get uv event loop");
        return false;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        PRINT_HILOGE("Failed to create uv work");
        return false;
    }
    CallbackParam *param = new (std::nothrow) CallbackParam;
    if (param == nullptr) {
        PRINT_HILOGE("Failed to create callback parameter");
        return false;    
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        param->env = env_;
        param->ref = ref_;
        param->mutexPtr = &mutex_;
        param->state = state;
        param->printerInfo = info;
    }
    work->data = param;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int statusInt) {
            CallbackParam *cbParam = static_cast<CallbackParam*>(work->data);
            if (cbParam != nullptr) {
                std::lock_guard<std::mutex> autoLock(*cbParam->mutexPtr);
                napi_value callbackFunc = NapiPrintUtils::GetReference(cbParam->env, cbParam->ref);
                napi_value callbackResult = nullptr;
                napi_value callbackValues[NapiPrintUtils::ARGC_TWO] = { 0 };
                callbackValues[NapiPrintUtils::INDEX_ZERO] = NapiPrintUtils::CreateUint32(cbParam->env, cbParam->state);
                callbackValues[NapiPrintUtils::INDEX_ONE] = cbParam->printerInfo.ToJsObject(cbParam->env);
                napi_call_function(cbParam->env, nullptr, callbackFunc, NapiPrintUtils::ARGC_TWO,
                    callbackValues, &callbackResult);
                if (work != nullptr) {
                    delete work;
                    work = nullptr;
                }
                delete cbParam;
                cbParam = nullptr;
            }
        });
    return true;
}

bool PrintCallback::OnCallback(uint32_t state, const PrintJob &info)
{
    PRINT_HILOGD("Print Task callback in");
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        PRINT_HILOGE("Failed to get uv event loop");
        return false;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        PRINT_HILOGE("Failed to create uv work");
        return false;
    }
    CallbackParam *param = new (std::nothrow) CallbackParam;
    if (param == nullptr) {
        PRINT_HILOGE("Failed to create callback parameter");
        return false;    
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        param->env = env_;
        param->ref = ref_;
        param->mutexPtr = &mutex_;
        param->state = state;
        param->jobInfo = info;
    }
    work->data = param;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int statusInt) {
            CallbackParam *cbParam = static_cast<CallbackParam*>(work->data);
            if (cbParam != nullptr) {
                std::lock_guard<std::mutex> autoLock(*cbParam->mutexPtr);
                napi_value callbackFunc = NapiPrintUtils::GetReference(cbParam->env, cbParam->ref);
                napi_value callbackResult = nullptr;
                napi_value callbackValues[NapiPrintUtils::ARGC_TWO] = { 0 };
                callbackValues[NapiPrintUtils::INDEX_ZERO] = NapiPrintUtils::CreateUint32(cbParam->env, cbParam->state);
                callbackValues[NapiPrintUtils::INDEX_ONE] = cbParam->jobInfo.ToJsObject(cbParam->env);
                napi_call_function(cbParam->env, nullptr, callbackFunc, NapiPrintUtils::ARGC_TWO,
                    callbackValues, &callbackResult);
                if (work != nullptr) {
                    delete work;
                    work = nullptr;
                }
                delete cbParam;
                cbParam = nullptr;
            }
        });
    return true;
}

bool PrintCallback::OnCallback(const std::string &extensionId, const std::string &info)
{
    PRINT_HILOGD("Ext Info callback in");
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        PRINT_HILOGE("Failed to get uv event loop");
        return false;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        PRINT_HILOGE("Failed to create uv work");
        return false;
    }
    CallbackParam *param = new (std::nothrow) CallbackParam;
    if (param == nullptr) {
        PRINT_HILOGE("Failed to create callback parameter");
        return false;    
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        param->env = env_;
        param->ref = ref_;
        param->mutexPtr = &mutex_;
        param->extensionId = extensionId;
        param->info = info;
    }
    work->data = param;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int statusInt) {
            CallbackParam *cbParam = static_cast<CallbackParam*>(work->data);
            if (cbParam != nullptr) {
                std::lock_guard<std::mutex> autoLock(*cbParam->mutexPtr);
                napi_value callbackFunc = NapiPrintUtils::GetReference(cbParam->env, cbParam->ref);
                napi_value callbackResult = nullptr;
                napi_value callbackValues[NapiPrintUtils::ARGC_TWO] = { 0 };
                callbackValues[NapiPrintUtils::INDEX_ZERO] =
                    NapiPrintUtils::CreateStringUtf8(cbParam->env, cbParam->extensionId);
                callbackValues[NapiPrintUtils::INDEX_ONE] = 
                    NapiPrintUtils::CreateStringUtf8(cbParam->env, cbParam->info);
                napi_call_function(cbParam->env, nullptr, callbackFunc, NapiPrintUtils::ARGC_TWO,
                    callbackValues, &callbackResult);
                if (work != nullptr) {
                    delete work;
                    work = nullptr;
                }
                delete cbParam;
                cbParam = nullptr;
            }
        });
    return true;
}

} // namespace OHOS::Print
