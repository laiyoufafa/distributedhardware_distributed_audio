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

#ifndef OHOS_DAUDIO_DECODE_TRANS_TEST_H
#define OHOS_DAUDIO_DECODE_TRANS_TEST_H

#include <gtest/gtest.h>

#define private public
#include "audio_decode_transport.h"
#undef private

#include "iaudio_channel.h"
#include "iaudio_datatrans_callback.h"
#include "daudio_errorcode.h"

namespace OHOS {
namespace DistributedHardware {
class DecodeTransportTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<IAudioDataTransCallback> transCallback_ = nullptr;
    std::shared_ptr<AudioDecodeTransport> decodeTrans_ = nullptr;
};

class MockIAudioChannel : public IAudioChannel {
public:
    int32_t CreateSession(const std::shared_ptr<IAudioChannelListener> &listener,
        const std::string &sessionName) override
    {
        return DH_SUCCESS;
    }
    int32_t ReleaseSession() override
    {
        return ERR_DH_AUDIO_FAILED;
    }
    virtual int32_t OpenSession() override
    {
        return ERR_DH_AUDIO_FAILED;
    }
    virtual int32_t CloseSession() override
    {
        return DH_SUCCESS;
    }
    int32_t SendData(const std::shared_ptr<AudioData> &data) override
    {
        return DH_SUCCESS;
    }
    int32_t SendEvent(const AudioEvent &event) override
    {
        return DH_SUCCESS;
    }
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DAUDIO_DECODE_TRANS_TEST_H
