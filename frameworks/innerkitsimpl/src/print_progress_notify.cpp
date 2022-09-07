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

#include "print_progress_notify.h"

#include <uv.h>

#include "napi_print_utils.h"
#include "print_job.h"
#include "print_log.h"
#include "print_page_size.h"
#include "print_resolution.h"
#include "printer_capability.h"
#include "printer_info.h"

namespace OHOS::Print {
PrintProgressNotify::PrintProgressNotify(napi_env env, const std::string &type, napi_ref ref)
    : PrintBaseNotify(env, type, ref)
{}

PrintProgressNotify::~PrintProgressNotify()
{
    PRINT_HILOGD("");
}

void PrintProgressNotify::DataReadInfo(PrinterInfo &info, MessageParcel &data)
{
    PRINT_HILOGD("");
}

void PrintProgressNotify::DataReadJob(PrintJob &job, MessageParcel &data)
{
    PRINT_HILOGD("");
}

void PrintProgressNotify::WriteInfoJsObject(napi_env env, PrinterInfo info, napi_value *result)
{
    PRINT_HILOGD("");
}

void PrintProgressNotify::notifyDataBase(NotifyData *notifyData, MessageParcel &data)
{
    PRINT_HILOGD("Progress callback in");
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        PRINT_HILOGE("Failed to get uv event loop");
        return;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        PRINT_HILOGE("Failed to create uv work");
        return;
    }

    notifyData = GetNotifyData();
    notifyData->env = env_;
    notifyData->ref = ref_;
    notifyData->type = type_;
    notifyData->objectType = data.ReadString();
    notifyData->firstArgv = data.ReadUint32();
    if (notifyData->objectType == "PrinterInfo") {
        DataReadInfo(notifyData->secondArgv, data);
    } else if (notifyData->objectType == "PrintJob") {
        DataReadJob(notifyData->thirdArgv, data);
    }
}

void PrintProgressNotify::OnCallBack(MessageParcel &data)
{
    NotifyData *notifyData = nullptr;
    notifyDataBase(notifyData, data);
    work->data = notifyData;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int statusInt) {
            NotifyData *notifyData = static_cast<NotifyData *>(work->data);
            if (notifyData != nullptr) {
                napi_value undefined = 0;
                napi_get_undefined(notifyData->env, &undefined);
                napi_value callbackFunc = nullptr;
                napi_get_reference_value(notifyData->env, notifyData->ref, &callbackFunc);
                napi_value result = nullptr;
                napi_value callbackVal[NapiPrintUtils::TWO_ARG] = { 0 };
                napi_create_uint32(notifyData->env, notifyData->firstArgv, &callbackVal[NapiPrintUtils::FIRST_ARGV]);
                if (notifyData->objectType == "PrinterInfo") {
                    WriteInfoJsObject(
                        notifyData->env, notifyData->secondArgv, &callbackVal[NapiPrintUtils::SECOND_ARGV]);
                } else if (notifyData->objectType == "PrintJob") {
                    WriteJobJsObject(notifyData->env, notifyData->thirdArgv, &callbackVal[NapiPrintUtils::SECOND_ARGV]);
                }
                napi_call_function(
                    notifyData->env, nullptr, callbackFunc, NapiPrintUtils::TWO_ARG, callbackVal, &result);
                if (work != nullptr) {
                    delete work;
                    work = nullptr;
                }
                delete notifyData;
                notifyData = nullptr;
            }
        });
}
} // namespace OHOS::Print