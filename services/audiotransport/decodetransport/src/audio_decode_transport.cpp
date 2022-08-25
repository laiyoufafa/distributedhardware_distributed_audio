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

#include "audio_decode_transport.h"

#include "audio_data_channel.h"
#include "audio_decoder_processor.h"
#include "audio_param.h"
#include "daudio_errorcode.h"
#include "daudio_log.h"

#undef DH_LOG_TAG
#define DH_LOG_TAG "AudioDecodeTransport"

namespace OHOS {
namespace DistributedHardware {
int32_t AudioDecodeTransport::SetUp(const AudioParam &localParam, const AudioParam &remoteParam,
    const std::shared_ptr<IAudioDataTransCallback> &callback, const std::string &role)
{
    if (callback == nullptr) {
        DHLOGE("The parameter is empty.");
        return ERR_DH_AUDIO_TRANS_ERROR;
    }
    dataTransCallback_ = callback;
    auto ret = InitAudioDecodeTransport(localParam, remoteParam, role);
    if (ret != DH_SUCCESS) {
        DHLOGE("Init audio encode transport, ret: %d.", ret);
        return ret;
    }
    DHLOGI("SetUp success.");
    return DH_SUCCESS;
}

int32_t AudioDecodeTransport::Start()
{
    DHLOGI("Start.");
    if (processor_ == nullptr) {
        DHLOGE("Processor  is null, Setup first.");
        return ERR_DH_AUDIO_TRANS_NULL_VALUE;
    }

    int32_t ret = processor_->StartAudioProcessor();
    if (ret != DH_SUCCESS) {
        DHLOGE("Open audio processor failed, ret: %d.", ret);
        processor_ = nullptr;
        return ERR_DH_AUDIO_TRANS_PROCESSOR_FAILED;
    }
    DHLOGI("Start success.");
    return DH_SUCCESS;
}

int32_t AudioDecodeTransport::Stop()
{
    DHLOGI("Stop.");
    bool stopStatus = true;
    int32_t ret;
    if (processor_ != nullptr) {
        ret = processor_->StopAudioProcessor();
        if (ret != DH_SUCCESS) {
            DHLOGE("Stop audio processor failed, ret: %d.", ret);
            stopStatus = false;
        }
    }
    if (audioChannel_ != nullptr) {
        ret = audioChannel_->CloseSession();
        if (ret != DH_SUCCESS) {
            DHLOGE("Close session failed, ret: %d.", ret);
            stopStatus = false;
        }
    }
    if (!stopStatus) {
        return ERR_DH_AUDIO_TRANS_ERROR;
    }
    DHLOGI("Stop success.");
    return DH_SUCCESS;
}

int32_t AudioDecodeTransport::Release()
{
    DHLOGI("Release.");
    bool releaseStatus = true;
    int32_t ret;
    if (processor_ != nullptr) {
        ret = processor_->ReleaseAudioProcessor();
        if (ret != DH_SUCCESS) {
            DHLOGE("Release audio processor failed, ret: %d.", ret);
            releaseStatus = false;
        }
    }
    if (audioChannel_ != nullptr) {
        ret = audioChannel_->ReleaseSession();
        if (ret != DH_SUCCESS) {
            DHLOGE("Release session failed, ret: %d.", ret);
            releaseStatus = false;
        }
    }
    if (!releaseStatus) {
        DHLOGE("The releaseStatus is false, ret: %d.", ret);
        return ERR_DH_AUDIO_TRANS_ERROR;
    }
    DHLOGI("Release success.");
    return DH_SUCCESS;
}

int32_t AudioDecodeTransport::FeedAudioData(std::shared_ptr<AudioData> &audioData)
{
    (void)audioData;
    return DH_SUCCESS;
}

void AudioDecodeTransport::OnSessionOpened()
{
    DHLOGI("On channel session opened.");
    if (dataTransCallback_ == nullptr) {
        DHLOGE("On channel session opened. callback is nullptr.");
        return;
    }
    dataTransCallback_->OnStateChange(AudioEventType::DATA_OPENED);
}

void AudioDecodeTransport::OnSessionClosed()
{
    DHLOGI("On channel session close.");
    if (dataTransCallback_ == nullptr) {
        DHLOGE("On channel session close, callback is nullptr.");
        return;
    }
    dataTransCallback_->OnStateChange(AudioEventType::DATA_CLOSED);
}

void AudioDecodeTransport::OnDataReceived(const std::shared_ptr<AudioData> &data)
{
    DHLOGI("On audio data received.");
    if (processor_ == nullptr) {
        DHLOGE("Processor null, setup first.");
        return;
    }

    int32_t ret = processor_->FeedAudioProcessor(data);
    if (ret != DH_SUCCESS) {
        DHLOGE("Feed audio processor failed ret: %d.", ret);
    }
}

void AudioDecodeTransport::OnEventReceived(const std::shared_ptr<AudioEvent> &event)
{
    (void)event;
}

void AudioDecodeTransport::OnAudioDataDone(const std::shared_ptr<AudioData> &outputData)
{
    DHLOGI("On audio data done.");
    std::lock_guard<std::mutex> lock(dataQueueMtx_);
    dataTransCallback_->OnDecodeTransDataDone(outputData);
}

void AudioDecodeTransport::OnStateNotify(const AudioEvent &event)
{
    (void)event;
}

int32_t AudioDecodeTransport::InitAudioDecodeTransport(const AudioParam &localParam,
    const AudioParam &remoteParam, const std::string &role)
{
    int32_t ret = RegisterChannelListener(role);
    if (ret != DH_SUCCESS) {
        DHLOGE("Register channel listener failed, ret: %d.", ret);
        audioChannel_ = nullptr;
        return ERR_DH_AUDIO_TRANS_ERROR;
    }

    ret = RegisterProcessorListener(localParam, remoteParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("Register processor listener failed, ret: %d.", ret);
        processor_ = nullptr;
        return ERR_DH_AUDIO_TRANS_ERROR;
    }
    audioParam_ = remoteParam;
    return DH_SUCCESS;
}

int32_t AudioDecodeTransport::RegisterChannelListener(const std::string &role)
{
    DHLOGI("Register Channel Listener.");
    audioChannel_ = std::make_shared<AudioDataChannel>(peerDevId_);
    if (audioChannel_ == nullptr) {
        DHLOGE("Create audio channel failed.");
        return ERR_DH_AUDIO_TRANS_ERROR;
    }

    int32_t result;
    if (role == "speaker") {
        result = audioChannel_->CreateSession(shared_from_this(), DATA_SPEAKER_SESSION_NAME);
    } else {
        result = audioChannel_->CreateSession(shared_from_this(), DATA_MIC_SESSION_NAME);
    }
    if (result != DH_SUCCESS) {
        DHLOGE("CreateSession failed.");
        return ERR_DH_AUDIO_TRANS_ERROR;
    }
    return DH_SUCCESS;
}

int32_t AudioDecodeTransport::RegisterProcessorListener(const AudioParam &localParam, const AudioParam &remoteParam)
{
    DHLOGI("Register processor listener.");
    processor_ = std::make_shared<AudioDecoderProcessor>();
    if (audioChannel_ == nullptr) {
        DHLOGE("Create audio processor failed.");
        return ERR_DH_AUDIO_TRANS_ERROR;
    }
    auto ret = processor_->ConfigureAudioProcessor(localParam.comParam, remoteParam.comParam, shared_from_this());
    if (ret != DH_SUCCESS) {
        DHLOGE("Configure audio processor failed.");
        return ret;
    }
    return DH_SUCCESS;
}
} // namespace DistributedHardware
} // namespace OHOS
