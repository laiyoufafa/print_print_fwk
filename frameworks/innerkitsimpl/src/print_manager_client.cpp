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

#include "print_manager_client.h"

#include "iservice_registry.h"
#include "print_constant.h"
#include "print_extension_callback_stub.h"
#include "print_log.h"
#include "print_sync_load_callback.h"
#include "print_utils.h"
#include "system_ability_definition.h"

namespace OHOS::Print {
std::mutex PrintManagerClient::instanceLock_;
sptr<PrintManagerClient> PrintManagerClient::instance_ = nullptr;

PrintManagerClient::PrintManagerClient() : printServiceProxy_(nullptr), deathRecipient_(nullptr) {}

PrintManagerClient::~PrintManagerClient() {}

sptr<PrintManagerClient> PrintManagerClient::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> autoLock(instanceLock_);
        if (instance_ == nullptr) {
            instance_ = new PrintManagerClient;
        }
    }
    return instance_;
}

bool PrintManagerClient::GetPrintServiceProxy()
{
    if (printServiceProxy_ != nullptr) {
        return true;
    }
    bool result = false;
    std::lock_guard<std::mutex> lock(proxyLock_);
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager != nullptr) {
        auto systemAbility = systemAbilityManager->GetSystemAbility(PRINT_SERVICE_ID, "");
        if (systemAbility != nullptr) {
            deathRecipient_ = new (std::nothrow) PrintSaDeathRecipient();
            if (deathRecipient_ != nullptr) {
                systemAbility->AddDeathRecipient(deathRecipient_);
                printServiceProxy_ = iface_cast<IPrintService>(systemAbility);
                PRINT_HILOGD("Getting PrintManagerClientProxy succeeded.");
            }
        }
    }
    PRINT_HILOGD("Getting PrintManagerClientProxy ret[%{public}d].", result);
    return result;
}

void PrintManagerClient::OnRemoteSaDied(const wptr<IRemoteObject> &remote)
{
    PRINT_HILOGD("start");
    if (remote == nullptr) {
        PRINT_HILOGE("remote is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(proxyLock_);
    if (printServiceProxy_ == nullptr) {
        PRINT_HILOGE("printServiceProxy_ is null");
        return;
    }
    auto serviceRemote = printServiceProxy_->AsObject();
    if ((serviceRemote != nullptr) && (serviceRemote == remote.promote())) {
        PRINT_HILOGD("need reset");
        serviceRemote->RemoveDeathRecipient(deathRecipient_);
        printServiceProxy_ = nullptr;
        deathRecipient_ = nullptr;
    }
}

int32_t PrintManagerClient::StartPrint(const std::vector<std::string> &fileList,
    const std::vector<uint32_t> &fdList, std::string &taskId)
{
    PRINT_HILOGD("PrintManagerClient StartPrint start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->StartPrint(fileList, fdList, taskId);
        PRINT_HILOGD("PrintManagerClient StartPrint out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::StopPrint(const std::string &taskId)
{
    PRINT_HILOGD("PrintManagerClient StopPrint start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->StopPrint(taskId);
        PRINT_HILOGD("PrintManagerClient StopPrint out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::ConnectPrinter(const std::string &printerId)
{
    PRINT_HILOGD("PrintManagerClient ConnectPrinter start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->ConnectPrinter(printerId);
        PRINT_HILOGD("PrintManagerClient ConnectPrinter out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::DisconnectPrinter(const std::string &printerId)
{
    PRINT_HILOGD("PrintManagerClient DisconnectPrinter start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->DisconnectPrinter(printerId);
        PRINT_HILOGD("PrintManagerClient DisconnectPrinter out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::QueryAllExtension(std::vector<PrintExtensionInfo> &extensionInfos)
{
    PRINT_HILOGD("PrintManagerClient QueryAllExtension start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->QueryAllExtension(extensionInfos);
        PRINT_HILOGD("PrintManagerClient QueryAllExtension out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::StartDiscoverPrinter(const std::vector<std::string> &extensionList)
{
    PRINT_HILOGD("PrintManagerClient StartDiscoverPrinter start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->StartDiscoverPrinter(extensionList);
        PRINT_HILOGD("PrintManagerClient StartDiscoverPrinter out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::StopDiscoverPrinter()
{
    PRINT_HILOGD("PrintManagerClient StopDiscoverPrinter start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->StopDiscoverPrinter();
        PRINT_HILOGD("PrintManagerClient StopDiscoverPrinter out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::StartPrintJob(const PrintJob &jobinfo)
{
    PRINT_HILOGD("PrintManagerClient StartPrintJob start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->StartPrintJob(jobinfo);
        PRINT_HILOGD("PrintManagerClient StartPrintJob out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::CancelPrintJob(const std::string &jobId)
{
    PRINT_HILOGD("PrintManagerClient CancelPrintJob start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->CancelPrintJob(jobId);
        PRINT_HILOGD("PrintManagerClient CancelPrintJob out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::AddPrinters(const std::vector<PrinterInfo> &printerInfos)
{
    PRINT_HILOGD("PrintManagerClient AddPrinters start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->AddPrinters(printerInfos);
        PRINT_HILOGD("PrintManagerClient AddPrinters out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::RemovePrinters(const std::vector<std::string> &printerIds)
{
    PRINT_HILOGD("PrintManagerClient RemovePrinters start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->RemovePrinters(printerIds);
        PRINT_HILOGD("PrintManagerClient RemovePrinters out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::UpdatePrinters(const std::vector<PrinterInfo> &printerInfos)
{
    PRINT_HILOGD("PrintManagerClient UpdatePrinters start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->UpdatePrinters(printerInfos);
        PRINT_HILOGD("PrintManagerClient UpdatePrinters out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::UpdatePrinterState(const std::string &printerId, uint32_t state)
{
    PRINT_HILOGD("PrintManagerClient UpdatePrinterState start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->UpdatePrinterState(printerId, state);
        PRINT_HILOGD("PrintManagerClient UpdatePrinterState out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::UpdatePrintJobState(const std::string &jobId, uint32_t state, uint32_t subState)
{
    PRINT_HILOGD("PrintManagerClient UpdatePrintJobState start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->UpdatePrintJobState(jobId, state, subState);
        PRINT_HILOGD("PrintManagerClient UpdatePrintJobState out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::UpdateExtensionInfo(const std::string &extInfo)
{
    PRINT_HILOGD("PrintManagerClient UpdateExtensionInfo start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->UpdateExtensionInfo(extInfo);
        PRINT_HILOGD("PrintManagerClient UpdateExtensionInfo out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::RequestPreview(const PrintJob &jobinfo, std::string &previewResult)
{
    PRINT_HILOGD("PrintManagerClient RequestPreview start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->RequestPreview(jobinfo, previewResult);
        PRINT_HILOGD("PrintManagerClient RequestPreview out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::QueryPrinterCapability(const std::string &printerId)
{
    PRINT_HILOGD("PrintManagerClient QueryPrinterCapability start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->QueryPrinterCapability(printerId);
        PRINT_HILOGD("PrintManagerClient QueryPrinterCapability out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::QueryAllPrintJob(std::vector<PrintJob> &printJobs)
{
    PRINT_HILOGD("PrintManagerClient QueryAllPrintJob start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->QueryAllPrintJob(printJobs);
        PRINT_HILOGD("PrintManagerClient QueryAllPrintJob out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::QueryPrintJobById(std::string &printJobId, PrintJob &printjob)
{
    PRINT_HILOGD("PrintManagerClient QueryPrintJobById start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->QueryPrintJobById(printJobId, printjob);
        PRINT_HILOGD("PrintManagerClient QueryPrintJobById out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::On(const std::string &taskId, const std::string &type, const sptr<IPrintCallback> &listener)
{
    PRINT_HILOGD("PrintManagerClient On start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->On(taskId, type, listener);
        PRINT_HILOGD("PrintManagerClient On out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::Off(const std::string &taskId, const std::string &type)
{
    PRINT_HILOGD("PrintManagerClient Off start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->Off(taskId, type);
        PRINT_HILOGD("PrintManagerClient Off out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::RegisterExtCallback(const std::string &extensionId,
    uint32_t callbackId, PrintExtCallback cb)
{
    PRINT_HILOGD("PrintManagerClient RegisterExtCallback start.");
    if (callbackId >= PRINT_EXTCB_MAX) {
        PRINT_HILOGE("Invalid callback id [%{public}d].", callbackId);
        return E_PRINT_INVALID_PARAMETER;
    }
    sptr<PrintExtensionCallbackStub> callbackStub = nullptr;
    std::string extensionCID = PrintUtils::EncodeExtensionCid(extensionId, callbackId);
    auto it = extCallbackMap_.find(extensionCID);
    if (it == extCallbackMap_.end()) {
        callbackStub = new (std::nothrow) PrintExtensionCallbackStub;
        if (callbackStub != nullptr) {
            callbackStub->SetExtCallback(cb);
            extCallbackMap_.insert(std::make_pair(extensionCID, callbackStub));
        }
    } else {
        callbackStub = it->second;
        callbackStub->SetExtCallback(cb);
    }
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->RegisterExtCallback(extensionCID, callbackStub);
        PRINT_HILOGD("PrintManagerClient RegisterExtCallback out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::RegisterExtCallback(const std::string &extensionId,
    uint32_t callbackId, PrintJobCallback cb)
{
    PRINT_HILOGD("PrintManagerClient RegisterExtCallback start.");
    if (callbackId >= PRINT_EXTCB_MAX) {
        PRINT_HILOGE("Invalid callback id [%{public}d].", callbackId);
        return E_PRINT_INVALID_PARAMETER;
    }

    sptr<PrintExtensionCallbackStub> callbackStub = nullptr;
    std::string extensionCID = PrintUtils::EncodeExtensionCid(extensionId, callbackId);
    auto it = extCallbackMap_.find(extensionCID);
    if (it == extCallbackMap_.end()) {
        callbackStub = new (std::nothrow) PrintExtensionCallbackStub;
        if (callbackStub != nullptr) {
            callbackStub->SetPrintJobCallback(cb);
            extCallbackMap_.insert(std::make_pair(extensionCID, callbackStub));
        }
    } else {
        callbackStub = it->second;
        callbackStub->SetPrintJobCallback(cb);
    }

    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->RegisterExtCallback(extensionCID, callbackStub);
        PRINT_HILOGD("PrintManagerClient RegisterExtCallback out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::RegisterExtCallback(const std::string &extensionId,
    uint32_t callbackId, PrinterCapabilityCallback cb)
{
    PRINT_HILOGD("PrintManagerClient RegisterExtCallback start.");
    if (callbackId >= PRINT_EXTCB_MAX) {
        PRINT_HILOGE("Invalid callback id [%{public}d].", callbackId);
        return E_PRINT_INVALID_PARAMETER;
    }

    sptr<PrintExtensionCallbackStub> callbackStub = nullptr;
    std::string extensionCID = PrintUtils::EncodeExtensionCid(extensionId, callbackId);
    auto it = extCallbackMap_.find(extensionCID);
    if (it == extCallbackMap_.end()) {
        callbackStub = new (std::nothrow) PrintExtensionCallbackStub;
        if (callbackStub != nullptr) {
            callbackStub->SetCapabilityCallback(cb);
            extCallbackMap_.insert(std::make_pair(extensionCID, callbackStub));
        }
    } else {
        callbackStub = it->second;
        callbackStub->SetCapabilityCallback(cb);
    }

    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->RegisterExtCallback(extensionCID, callbackStub);
        PRINT_HILOGD("PrintManagerClient RegisterExtCallback out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::RegisterExtCallback(const std::string &extensionId,
    uint32_t callbackId, PrinterCallback cb)
{
    PRINT_HILOGD("PrintManagerClient RegisterExtCallback start.");
    if (callbackId >= PRINT_EXTCB_MAX) {
        PRINT_HILOGE("Invalid callback id [%{public}d].", callbackId);
        return E_PRINT_INVALID_PARAMETER;
    }

    sptr<PrintExtensionCallbackStub> callbackStub = nullptr;
    std::string extensionCID = PrintUtils::EncodeExtensionCid(extensionId, callbackId);
    auto it = extCallbackMap_.find(extensionCID);
    if (it == extCallbackMap_.end()) {
        callbackStub = new (std::nothrow) PrintExtensionCallbackStub;
        if (callbackStub != nullptr) {
            callbackStub->SetPrinterCallback(cb);
            extCallbackMap_.insert(std::make_pair(extensionCID, callbackStub));
        }
    } else {
        callbackStub = it->second;
        callbackStub->SetPrinterCallback(cb);
    }

    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->RegisterExtCallback(extensionCID, callbackStub);
        PRINT_HILOGD("PrintManagerClient RegisterExtCallback out ret = [%{public}d].", ret);
    }
    return ret;
}

int32_t PrintManagerClient::UnregisterAllExtCallback(const std::string &extensionId)
{
    PRINT_HILOGD("PrintManagerClient UnregisterAllExtCallback start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->UnregisterAllExtCallback(extensionId);
        PRINT_HILOGD("PrintManagerClient UnregisterAllExtCallback out ret = [%{public}d].", ret);
    }
    extCallbackMap_.clear();
    return ret;
}

int32_t PrintManagerClient::LoadExtSuccess(const std::string &extensionId)
{
    PRINT_HILOGD("PrintManagerClient LoadExtSuccess start.");
    int32_t ret = E_PRINT_RPC_FAILURE;
    if (LoadServer() && GetPrintServiceProxy()) {
        ret = printServiceProxy_->LoadExtSuccess(extensionId);
        PRINT_HILOGD("PrintManagerClient LoadExtSuccess out ret = [%{public}d].", ret);
    }
    return ret;
}

bool PrintManagerClient::LoadServer()
{
    if (ready_) {
        return true;
    }
    std::lock_guard<std::mutex> lock(loadMutex_);
    if (ready_) {
        return true;
    }

    auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sm == nullptr) {
        PRINT_HILOGE("GetSystemAbilityManager return null");
        return false;
    }

    sptr<PrintSyncLoadCallback> loadCallback_ = new (std::nothrow) PrintSyncLoadCallback();
    if (loadCallback_ == nullptr) {
        PRINT_HILOGE("new PrintSyncLoadCallback fail");
        return false;
    }

    int32_t result = sm->LoadSystemAbility(PRINT_SERVICE_ID, loadCallback_);
    if (result != ERR_OK) {
        PRINT_HILOGE("LoadSystemAbility %{public}d failed, result: %{public}d", PRINT_SERVICE_ID, result);
        return false;
    }

    {
        std::unique_lock<std::mutex> conditionLock(conditionMutex_);
        auto waitStatus = syncCon_.wait_for(
            conditionLock, std::chrono::milliseconds(LOAD_SA_TIMEOUT_MS), [this]() { return ready_; });
        if (!waitStatus) {
            PRINT_HILOGE("print server load sa timeout");
            return false;
        }
    }
    return true;
}

void PrintManagerClient::LoadServerSuccess()
{
    std::unique_lock<std::mutex> lock(conditionMutex_);
    ready_ = true;
    syncCon_.notify_one();
    PRINT_HILOGD("load print server success");
}

void PrintManagerClient::LoadServerFail()
{
    ready_ = false;
    PRINT_HILOGE("load print server fail");
}
void PrintManagerClient::SetProxy(const sptr<IRemoteObject> &obj)
{
    if (printServiceProxy_ != nullptr) {
        auto serviceRemote = printServiceProxy_->AsObject();
        if (serviceRemote != nullptr) {
            serviceRemote->RemoveDeathRecipient(deathRecipient_);
            printServiceProxy_ = nullptr;
            deathRecipient_ = nullptr;
        }
    }
    deathRecipient_ = new (std::nothrow) PrintSaDeathRecipient();
    if (deathRecipient_ != nullptr) {
        obj->AddDeathRecipient(deathRecipient_);
        PRINT_HILOGD("Getting PrintManagerClientProxy succeeded.");
    }
    printServiceProxy_ = iface_cast<IPrintService>(obj);
}
void PrintManagerClient::ResetProxy()
{
    printServiceProxy_ = nullptr;
}
} // namespace OHOS::Print
