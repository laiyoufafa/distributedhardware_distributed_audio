/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_AUDIO_CAPTURE_LOWLATENCY_IMPL_H
#define OHOS_AUDIO_CAPTURE_LOWLATENCY_IMPL_H

#include <cmath>
#include <mutex>
#include <string>

#include <v1_0/audio_types.h>
#include <v1_0/iaudio_capture.h>
#include <v1_0/id_audio_manager.h>

#include "ashmem.h"
#include "audio_capture_interface_impl.h"
#include "audio_capture_interface_impl_base.h"

namespace OHOS {
namespace HDI {
namespace DistributedAudio {
namespace Audio {
namespace V1_0 {
using OHOS::HDI::DistributedAudio::Audioext::V1_0::AudioData;
using OHOS::HDI::DistributedAudio::Audioext::V1_0::AudioParameter;
using OHOS::HDI::DistributedAudio::Audioext::V1_0::DAudioEvent;
using OHOS::HDI::DistributedAudio::Audioext::V1_0::IDAudioCallback;
class AudioCaptureLowLatencyImpl : public AudioCaptureInterfaceImplBase {
public:
    AudioCaptureLowLatencyImpl(const std::string &adpName, const AudioDeviceDescriptor &desc,
        const AudioSampleAttributes &attrs, const sptr<IDAudioCallback> &callback);
    ~AudioCaptureLowLatencyImpl() override;

    int32_t CaptureFrame(std::vector<int8_t> &frame, uint64_t requestBytes) override;
    int32_t GetCapturePosition(uint64_t &frames, AudioTimeStamp &time) override;
    int32_t CheckSceneCapability(const AudioSceneDescriptor &scene, bool &supported) override;
    int32_t SelectScene(const AudioSceneDescriptor &scene) override;
    int32_t SetMute(bool mute) override;
    int32_t GetMute(bool &mute) override;
    int32_t SetVolume(float volume) override;
    int32_t GetVolume(float &volume) override;
    int32_t GetGainThreshold(float &min, float &max) override;
    int32_t GetGain(float &gain) override;
    int32_t SetGain(float gain) override;
    int32_t GetFrameSize(uint64_t &size) override;
    int32_t GetFrameCount(uint64_t &count) override;
    int32_t SetSampleAttributes(const AudioSampleAttributes &attrs) override;
    int32_t GetSampleAttributes(AudioSampleAttributes &attrs) override;
    int32_t GetCurrentChannelId(uint32_t &channelId) override;
    int32_t SetExtraParams(const std::string &keyValueList) override;
    int32_t GetExtraParams(std::string &keyValueList) override;
    int32_t ReqMmapBuffer(int32_t reqSize, AudioMmapBufferDescriptor &desc) override;
    int32_t GetMmapPosition(uint64_t &frames, AudioTimeStamp &time) override;
    int32_t AddAudioEffect(uint64_t effectid) override;
    int32_t RemoveAudioEffect(uint64_t effectid) override;
    int32_t GetFrameBufferSize(uint64_t &bufferSize) override;
    int32_t Start() override;
    int32_t Stop() override;
    int32_t Pause() override;
    int32_t Resume() override;
    int32_t Flush() override;
    int32_t TurnStandbyMode() override;
    int32_t AudioDevDump(int32_t range, int32_t fd) override;
    int32_t IsSupportsPauseAndResume(bool &supportPause, bool &supportResume) override;

private:
    int32_t InitAshmem(int32_t ashmemLength);
    void UnInitAshmem();
private:
    std::string adapterName_;
    AudioDeviceDescriptor devDesc_;
    AudioSampleAttributes devAttrs_;
    uint32_t timeInterval_ = 5;
    uint32_t minTimeInterval_ = 30;
    uint32_t maxTimeInterval_ = 80;

    std::mutex captureMtx_;
    AudioCaptureStatus captureStatus_ = CAPTURE_STATUS_CLOSE;
    sptr<IDAudioCallback> audioExtCallback_ = nullptr;
    OHOS::sptr<OHOS::Ashmem> ashmem_ = nullptr;
    int32_t ashmemLength_;
    int32_t lengthPerTrans_;
    int32_t fd_;
};
} // V1_0
} // Audio
} // Distributedaudio
} // HDI
} // OHOS
#endif // OHOS_AUDIO_CAPTURE_LOWLATENCY_IMPL_H