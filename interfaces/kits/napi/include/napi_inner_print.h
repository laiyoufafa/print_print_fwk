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

#ifndef NAPI_INNER_PRINT_H
#define NAPI_INNER_PRINT_H

#include <string>
#include <vector>

#include "async_call.h"
#include "iprint_callback.h"
#include "napi/native_api.h"
#include "napi_print_ext.h"
#include "noncopyable.h"
#include "print_extension_info.h"
#include "print_job.h"
#include "print_page_size.h"
#include "print_resolution.h"
#include "print_task.h"
#include "printer_capability.h"
#include "printer_info.h"

namespace OHOS::Print {
enum EventType {
    NO_ARG_EVENT,
    ONE_ARG_EVENT,
    TWO_ARG_EVENT,
};
class NapiInnerPrint {
public:
    explicit NapiInnerPrint(uint32_t taskId);
    ~NapiInnerPrint();

    static napi_value QueryExtensionInfo(napi_env env, napi_callback_info info);
    static napi_value StartDiscovery(napi_env env, napi_callback_info info);
    static napi_value StopDiscovery(napi_env env, napi_callback_info info);
    static napi_value ConnectPrint(napi_env env, napi_callback_info info);
    static napi_value DisconnectPrint(napi_env env, napi_callback_info info);
    static napi_value StartPrintJob(napi_env env, napi_callback_info info);
    static napi_value CancelPrintJob(napi_env env, napi_callback_info info);
    static napi_value RequestPreview(napi_env env, napi_callback_info info);
    static napi_value QueryCapability(napi_env env, napi_callback_info info);
    static napi_value On(napi_env env, napi_callback_info info);
    static napi_value Off(napi_env env, napi_callback_info info);
    static int32_t GetEventType(const std::string &type);

private:
    static sptr<IPrintCallback> CreateNotify(napi_env env, const std::string &type, napi_ref callbackRef);

    struct OperationContext : public AsyncCall::Context {
        int32_t dummy = 0;
        std::vector<PrinterExtensionInfo> arrayPrinterExtensionInfo;
        std::vector<uint32_t> extensionList;
        uint32_t state;
        PrinterInfo info;
        std::string previewResult = "";
        std::string type = "";
        PrintJob jobinfo;
        PrintTask *task_ = nullptr;
        std::string title_ = "";
        std::string description_ = "";
        std::string filePath_ = "";
        uint32_t speedLimit_ = 0;
        uint32_t priority_ = 0;
        PrinterCapability printerCapability;
        bool result = false;
        std::string stateType_ = "";
        PrintJob printStartJob;
        PrintJob printCancelJob;
        PrintJob printReqPreviewJob;
        uint32_t printCapacityId = 0;
        uint32_t printConnId = 0;
        uint32_t printDisConnId = 0;
        std::vector<uint32_t> extensionVector;
        napi_status status = napi_generic_failure;
        OperationContext() : Context(nullptr, nullptr) {};
        OperationContext(InputAction input, OutputAction output) : Context(std::move(input), std::move(output)) {};
        virtual ~OperationContext() {};
    };
};
} // namespace OHOS::Print
#endif // NAPI_INNER_PRINT_H