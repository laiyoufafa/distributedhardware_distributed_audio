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

#define private public
#include "daudio_ipc_callback_test.h"
#undef private

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void DAudioIpcCallbackTest::SetUpTestCase(void) {}

void DAudioIpcCallbackTest::TearDownTestCase(void) {}

void DAudioIpcCallbackTest::SetUp(void)
{
    dAudioIpcCallback_ = new DAudioIpcCallback();
}

void DAudioIpcCallbackTest::TearDown(void)
{
    dAudioIpcCallback_ = nullptr;
}

/**
 * @tc.name: OnNotifyRegResult_001
 * @tc.desc: Verify the OnNotifyRegResult function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DAudioIpcCallbackTest, OnNotifyRegResult_001, TestSize.Level1)
{
    const std::string devId = "devId";
    const std::string dhId = "dhId";
    const std::string reqId = "reqIdReg";
    int32_t status = 0;
    const std::string data = "data";
    std::shared_ptr<RegisterCallback> callback = std::make_shared<RegisterCallbackTest>();
    dAudioIpcCallback_->PushRegisterCallback(reqId, callback);
    int32_t ret = dAudioIpcCallback_->OnNotifyRegResult(devId, dhId, reqId, status, data);
    dAudioIpcCallback_->PopRegisterCallback(reqId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: OnNotifyRegResult_002
 * @tc.desc: Verify the OnNotifyRegResult function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DAudioIpcCallbackTest, OnNotifyRegResult_002, TestSize.Level1)
{
    const std::string devId = "devId";
    const std::string dhId = "dhId";
    const std::string reqId = "reqId";
    int32_t status = 0;
    const std::string data = "data";
    int32_t ret = dAudioIpcCallback_->OnNotifyRegResult(devId, dhId, reqId, status, data);
    EXPECT_EQ(ERR_DH_AUDIO_SA_REGISTERCALLBACK_NOT_FOUND, ret);
}

/**
 * @tc.name: OnNotifyRegResult_003
 * @tc.desc: Verify the OnNotifyRegResult function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DAudioIpcCallbackTest, OnNotifyRegResult_003, TestSize.Level1)
{
    size_t  DAUDIO_MAX_DEVICE_ID_LEN = 101;
    std::string devId ;
    devId.resize(DAUDIO_MAX_DEVICE_ID_LEN);
    const std::string dhId = "dhId";
    const std::string reqId = "reqId";
    int32_t status = 0;
    const std::string data = "data";
    int32_t ret = dAudioIpcCallback_->OnNotifyRegResult(devId, dhId, reqId, status, data);
    EXPECT_EQ(ERR_DH_AUDIO_SA_DEVID_ILLEGAL, ret);
}

/**
 * @tc.name: OnNotifyUnregResult_001
 * @tc.desc: Verify the OnNotifyUnregResult function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DAudioIpcCallbackTest, OnNotifyUnregResult_001, TestSize.Level1)
{
    const std::string devId = "devId";
    const std::string dhId = "dhId";
    const std::string reqId = "reqIdUnreg";
    int32_t status = 0;
    const std::string data = "data";
    std::shared_ptr<UnregisterCallback> callback = std::make_shared<UnregisterCallbackTest>();
    dAudioIpcCallback_->PushUnregisterCallback(reqId, callback);
    int32_t ret = dAudioIpcCallback_->OnNotifyUnregResult(devId, dhId, reqId, status, data);
    dAudioIpcCallback_->PopUnregisterCallback(reqId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: OnNotifyUnregResult_002
 * @tc.desc: Verify the OnNotifyUnregResult function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DAudioIpcCallbackTest, OnNotifyUnregResult_002, TestSize.Level1)
{
    const std::string devId = "devId";
    const std::string dhId = "dhId";
    const std::string reqId = "reqId";
    int32_t status = 0;
    const std::string data = "data";
    int32_t ret = dAudioIpcCallback_->OnNotifyUnregResult(devId, dhId, reqId, status, data);
    EXPECT_EQ(ERR_DH_AUDIO_SA_UNREGISTERCALLBACK_NOT_FOUND, ret);
}

/**
 * @tc.name: OnNotifyUnregResult_003
 * @tc.desc: Verify the OnNotifyUnregResult function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DAudioIpcCallbackTest, OnNotifyUnregResult_003, TestSize.Level1)
{
    size_t  DAUDIO_MAX_DEVICE_ID_LEN = 101;
    std::string devId ;
    devId.resize(DAUDIO_MAX_DEVICE_ID_LEN);
    const std::string dhId = "dhId";
    const std::string reqId = "reqId";
    int32_t status = 0;
    const std::string data = "data";
    int32_t ret = dAudioIpcCallback_->OnNotifyUnregResult(devId, dhId, reqId, status, data);
    EXPECT_EQ(ERR_DH_AUDIO_SA_DEVID_ILLEGAL, ret);
}

/**
 * @tc.name: PushRegRegisterCallback_001
 * @tc.desc: Verify the PushRegRegisterCallback function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DAudioIpcCallbackTest, PushRegRegisterCallback_001, TestSize.Level1)
{
    const std::string reqId = "reqIdReg";
    std::shared_ptr<RegisterCallback> callback = std::make_shared<RegisterCallbackTest>();
    int32_t sizeFront = dAudioIpcCallback_->registerCallbackMap_.size();
    dAudioIpcCallback_->PushRegisterCallback(reqId, callback);
    int32_t sizeEnd = dAudioIpcCallback_->registerCallbackMap_.size();
    dAudioIpcCallback_->PopRegisterCallback(reqId);
    EXPECT_GT(sizeEnd, sizeFront);
}

/**
 * @tc.name: PopRegRegisterCallback_001
 * @tc.desc: Verify the PopRegRegisterCallback function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DAudioIpcCallbackTest, PopRegRegisterCallback_001, TestSize.Level1)
{
    const std::string reqId = "reqId";
    std::shared_ptr<RegisterCallback> callback = std::make_shared<RegisterCallbackTest>();
    dAudioIpcCallback_->PushRegisterCallback(reqId, callback);
    int32_t sizeFront = dAudioIpcCallback_->registerCallbackMap_.size();
    dAudioIpcCallback_->PopRegisterCallback(reqId);
    int32_t sizeEnd = dAudioIpcCallback_->registerCallbackMap_.size();
    EXPECT_GT(sizeFront, sizeEnd);
}

/**
 * @tc.name: PushRegRegisterCallback_001
 * @tc.desc: Verify the PushRegRegisterCallback function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DAudioIpcCallbackTest, PushUnregRegisterCallback_001, TestSize.Level1)
{
    const std::string reqId = "reqIdUnreg";
    std::shared_ptr<UnregisterCallback> callback = std::make_shared<UnregisterCallbackTest>();
    int32_t sizeFront = dAudioIpcCallback_->unregisterCallbackMap_.size();
    dAudioIpcCallback_->PushUnregisterCallback(reqId, callback);
    int32_t sizeEnd = dAudioIpcCallback_->unregisterCallbackMap_.size();
    dAudioIpcCallback_->PopUnregisterCallback(reqId);
    EXPECT_GT(sizeEnd, sizeFront);
}

/**
 * @tc.name: PopRegRegisterCallback_001
 * @tc.desc: Verify the PopRegRegisterCallback function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DAudioIpcCallbackTest, PopUnregRegisterCallback_001, TestSize.Level1)
{
    const std::string reqId = "reqId";
    std::shared_ptr<UnregisterCallback> callback = std::make_shared<UnregisterCallbackTest>();
    dAudioIpcCallback_->PushUnregisterCallback(reqId, callback);
    int32_t sizeFront = dAudioIpcCallback_->unregisterCallbackMap_.size();
    dAudioIpcCallback_->PopUnregisterCallback(reqId);
    int32_t sizeEnd = dAudioIpcCallback_->unregisterCallbackMap_.size();
    EXPECT_GT(sizeFront, sizeEnd);
}
} // namespace DistributedHardware
} // namespace OHOS
