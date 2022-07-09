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

#ifndef OHOS_DAUDIO_TEST_UTILS_H
#define OHOS_DAUDIO_TEST_UTILS_H

#include "daudio_errcode.h"
#include "v1_0/iaudio_capture.h"
#include "v1_0/iaudio_param_callback.h"
#include "v1_0/iaudio_render.h"
#include "v1_0/id_audio_callback.h"

namespace OHOS {
namespace HDI {
namespace DistributedAudio {
namespace Audio {
namespace V1_0 {
using OHOS::HDI::DistributedAudio::Audioext::V1_0::AudioEvent;
using OHOS::HDI::DistributedAudio::Audioext::V1_0::AudioData;
using OHOS::HDI::DistributedAudio::Audioext::V1_0::AudioParameter;
using OHOS::HDI::DistributedAudio::Audioext::V1_0::IDAudioCallback;
class MockIDAudioCallback : public IDAudioCallback {
public:
    MockIDAudioCallback() {}
    ~MockIDAudioCallback() {}

    int32_t OpenDevice(const std::string &adpName, int32_t devId) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t CloseDevice(const std::string &adpName, int32_t devId) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t SetParameters(const std::string &adpNam, int32_t devId, const AudioParameter &param) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t NotifyEvent(const std::string &adpNam, int32_t devId, const AudioEvent &event) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t WriteStreamData(const std::string &adpNam, int32_t devId, const AudioData &data) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t ReadStreamData(const std::string &adpNam, int32_t devId, AudioData &data) override
    {
        return DistributedHardware::DH_SUCCESS;
    }
};

class MockIAudioRender : public IAudioRender {
public:
    MockIAudioRender() {}
    ~MockIAudioRender() {}

    int32_t GetLatency(uint32_t& ms) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t RenderFrame(const std::vector<uint8_t>& frame, uint64_t requestBytes, uint64_t& replyBytes) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetRenderPosition(uint64_t& frames, AudioTimeStampHAL& time) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t SetRenderSpeed(float speed) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetRenderSpeed(float& speed) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t SetChannelMode(AudioChannelModeHAL mode) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetChannelMode(AudioChannelModeHAL& mode) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t RegCallback(const sptr<IAudioRenderCallback>& cbObj) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t DrainBuffer(AudioDrainNotifyTypeHAL type) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t Start() override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t Stop() override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t Pause() override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t Resume() override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t Flush() override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t TurnStandbyMode() override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t AudioDevDump(int32_t range, int32_t fd) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t CheckSceneCapability(const AudioSceneDescriptorHAL& scene, bool& support) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t SelectScene(const AudioSceneDescriptorHAL& scene) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t SetMute(bool mute) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetMute(bool& mute) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t SetVolume(float volume) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetVolume(float& volume) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetGainThreshold(float& min, float& max) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t SetGain(float gain) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetGain(float& gain) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetFrameSize(uint64_t& size) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetFrameCount(uint64_t& count) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t SetSampleAttributes(const AudioSampleAttributesHAL& attrs) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetSampleAttributes(AudioSampleAttributesHAL& attrs) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetCurrentChannelId(uint32_t& channelId) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t SetExtraParams(const std::string& keyValueList) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetExtraParams(std::string& keyValueList) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t ReqMmapBuffer(int32_t reqSize, AudioMmapBufferDescripterHAL& desc) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetMmapPosition(uint64_t& frames, AudioTimeStampHAL& time) override
    {
        return DistributedHardware::DH_SUCCESS;
    }
};

class MockIAudioCapture : public IAudioCapture {
public:
    MockIAudioCapture() {}
    ~MockIAudioCapture() {}

    int32_t GetCapturePosition(uint64_t& frames, AudioTimeStampHAL& time) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t CaptureFrame(std::vector<uint8_t>& frame, uint64_t requestBytes, uint64_t& replyBytes) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t Start() override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t Stop() override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t Pause() override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t Resume() override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t Flush() override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t TurnStandbyMode() override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t AudioDevDump(int32_t range, int32_t fd) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t CheckSceneCapability(const AudioSceneDescriptorHAL& scene, bool& support) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t SelectScene(const AudioSceneDescriptorHAL& scene) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t SetMute(bool mute) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetMute(bool& mute) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t SetVolume(float volume) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetVolume(float& volume) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetGainThreshold(float& min, float& max) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t SetGain(float gain) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetGain(float& gain) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetFrameSize(uint64_t& size) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetFrameCount(uint64_t& count) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t SetSampleAttributes(const AudioSampleAttributesHAL& attrs) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetSampleAttributes(AudioSampleAttributesHAL& attrs) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetCurrentChannelId(uint32_t& channelId) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t SetExtraParams(const std::string& keyValueList) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetExtraParams(std::string& keyValueList) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t ReqMmapBuffer(int32_t reqSize, AudioMmapBufferDescripterHAL& desc) override
    {
        return DistributedHardware::DH_SUCCESS;
    }

    int32_t GetMmapPosition(uint64_t& frames, AudioTimeStampHAL& time) override
    {
        return DistributedHardware::DH_SUCCESS;
    }
};

class MockIAudioParamCallback : public IAudioParamCallback {
public:
    MockIAudioParamCallback() {}
    ~MockIAudioParamCallback() {}

    int32_t OnAudioParamNotify(AudioExtParamKey key, const std::string& condition,
         const std::string& value) override
    {
        return DistributedHardware::DH_SUCCESS;
    }
};
} // V1_0
} // AudioExt
} // Distributedaudio
} // HDI
} // OHOS
#endif // OHOS_DAUDIO_TEST_UTILS_H