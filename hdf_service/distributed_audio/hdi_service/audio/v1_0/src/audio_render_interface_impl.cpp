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

#include "audio_render_interface_impl.h"

#include <hdf_base.h>
#include <unistd.h>
#include "sys/time.h"

#include "daudio_constants.h"
#include "daudio_log.h"

#undef DH_LOG_TAG
#define DH_LOG_TAG "AudioRenderInterfaceImpl"

using namespace OHOS::DistributedHardware;
namespace OHOS {
namespace HDI {
namespace DistributedAudio {
namespace Audio {
namespace V1_0 {
AudioRenderInterfaceImpl::AudioRenderInterfaceImpl(const std::string adpName, const AudioDeviceDescriptorHAL &desc,
    const AudioSampleAttributesHAL &attrs, const sptr<IDAudioCallback> &callback)
    : adapterName_(adpName), devDesc_(desc), devAttrs_(attrs), audioExtCallback_(callback)
{
    DHLOGD("Distributed Audio Render constructed, id(%d).", desc.pins);
}

AudioRenderInterfaceImpl::~AudioRenderInterfaceImpl()
{
    DHLOGD("Distributed Audio Render destructed, id(%d).", devDesc_.pins);
}

int32_t AudioRenderInterfaceImpl::GetLatency(uint32_t &ms)
{
    DHLOGI("Get render device latency, not support yet.");
    ms = 0;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::RenderFrame(const std::vector<uint8_t> &frame, uint64_t requestBytes,
    uint64_t &replyBytes)
{
    DHLOGI("Render frame[samplerate: %d, channelmask: %d, bitformat: %d].", devAttrs_.sampleRate,
        devAttrs_.channelCount, devAttrs_.format);

    std::lock_guard<std::mutex> renderLck(renderMtx_);
    if (renderStatus_ != RENDER_STATUS_START) {
        DHLOGE("Render status wrong, return false.");
        return HDF_FAILURE;
    }

    AudioParameter param = { devAttrs_.format, devAttrs_.channelCount, devAttrs_.sampleRate };
    AudioData data = { param, frame };
    int32_t ret = audioExtCallback_->WriteStreamData(adapterName_, devDesc_.pins, data);
    if (ret != HDF_SUCCESS) {
        DHLOGE("Write stream data failed.");
        return HDF_FAILURE;
    }

    DHLOGI("Render audio frame success.");
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::GetRenderPosition(uint64_t &frames, AudioTimeStampHAL &time)
{
    DHLOGI("Get render position, not support yet.");
    (void)frames;
    (void)time;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::SetRenderSpeed(float speed)
{
    DHLOGI("Set render speed, control render speed is not support yet.");
    renderSpeed_ = speed;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::GetRenderSpeed(float &speed)
{
    DHLOGI("Get render speed, control render speed is not support yet.");
    speed = renderSpeed_;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::SetChannelMode(AudioChannelModeHAL mode)
{
    DHLOGI("Set channel mode, control channel mode is not support yet.");
    channelMode_ = mode;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::GetChannelMode(AudioChannelModeHAL &mode)
{
    DHLOGI("Get channel mode, control channel mode is not support yet.");
    mode = channelMode_;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::RegCallback(const sptr<IAudioRenderCallback> &cbObj)
{
    DHLOGI("Register render callback.");
    renderCallback_ = cbObj;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::DrainBuffer(AudioDrainNotifyTypeHAL type)
{
    DHLOGI("Drain audio buffer, not support yet.");
    (void)type;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::Start()
{
    DHLOGI("Start render.");
    std::lock_guard<std::mutex> renderLck(renderMtx_);
    renderStatus_ = RENDER_STATUS_START;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::Stop()
{
    DHLOGI("Stop render.");
    std::lock_guard<std::mutex> renderLck(renderMtx_);
    renderStatus_ = RENDER_STATUS_STOP;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::Pause()
{
    DHLOGI("Pause render.");
    std::lock_guard<std::mutex> renderLck(renderMtx_);
    renderStatus_ = RENDER_STATUS_PAUSE;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::Resume()
{
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::Flush()
{
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::TurnStandbyMode()
{
    DHLOGI("Turn stand by mode, not support yet.");
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::AudioDevDump(int32_t range, int32_t fd)
{
    DHLOGI("Dump audio info, not support yet.");
    (void)range;
    (void)fd;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::CheckSceneCapability(const AudioSceneDescriptorHAL &scene, bool &support)
{
    DHLOGI("Check scene capability.");
    (void)scene;
    (void)support;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::SelectScene(const AudioSceneDescriptorHAL &scene)
{
    DHLOGI("Select audio scene, not support yet.");
    (void)scene;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::SetMute(bool mute)
{
    DHLOGI("Set mute, not support yet.");
    (void)mute;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::GetMute(bool &mute)
{
    DHLOGI("Get mute, not support yet.");
    (void)mute;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::SetVolume(float volume)
{
    DHLOGI("Can not set vol not by this interface.");
    (void)volume;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::GetVolume(float &volume)
{
    DHLOGI("Can not get vol not by this interface.");
    (void)volume;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::GetGainThreshold(float &min, float &max)
{
    DHLOGI("Get gain threshold, not support yet.");
    min = 0;
    max = 0;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::SetGain(float gain)
{
    DHLOGI("Set gain, not support yet.");
    (void)gain;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::GetGain(float &gain)
{
    DHLOGI("Get gain, not support yet.");
    gain = 1.0;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::GetFrameSize(uint64_t &size)
{
    (void)size;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::GetFrameCount(uint64_t &count)
{
    (void)count;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::SetSampleAttributes(const AudioSampleAttributesHAL &attrs)
{
    DHLOGI("Set sample attributes.");
    devAttrs_ = attrs;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::GetSampleAttributes(AudioSampleAttributesHAL &attrs)
{
    DHLOGI("Get sample attributes.");
    attrs = devAttrs_;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::GetCurrentChannelId(uint32_t &channelId)
{
    DHLOGI("Get current channel id, not support yet.");
    (void)channelId;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::SetExtraParams(const std::string &keyValueList)
{
    DHLOGI("Set extra parameters, not support yet.");
    (void)keyValueList;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::GetExtraParams(std::string &keyValueList)
{
    DHLOGI("Get extra parameters, not support yet.");
    (void)keyValueList;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::ReqMmapBuffer(int32_t reqSize, AudioMmapBufferDescripterHAL &desc)
{
    DHLOGI("Request mmap buffer, not support yet.");
    (void)reqSize;
    (void)desc;
    return HDF_SUCCESS;
}

int32_t AudioRenderInterfaceImpl::GetMmapPosition(uint64_t &frames, AudioTimeStampHAL &time)
{
    DHLOGI("Get mmap position, not support yet.");
    (void)frames;
    (void)time;
    return HDF_SUCCESS;
}

const AudioDeviceDescriptorHAL &AudioRenderInterfaceImpl::GetRenderDesc()
{
    return devDesc_;
}

void AudioRenderInterfaceImpl::SetVolumeInner(const uint32_t vol)
{
    std::lock_guard<std::mutex> volLck(volMtx_);
    vol_ = vol;
}


void AudioRenderInterfaceImpl::SetVolumeRangeInner(const uint32_t volMax, const uint32_t volMin)
{
    std::lock_guard<std::mutex> volLck(volMtx_);
    volMin_ = volMin;
    volMax_ = volMax;
}

uint32_t AudioRenderInterfaceImpl::GetVolumeInner()
{
    std::lock_guard<std::mutex> volLck(volMtx_);
    return vol_;
}

uint32_t AudioRenderInterfaceImpl::GetMaxVolumeInner()
{
    std::lock_guard<std::mutex> volLck(volMtx_);
    return volMax_;
}

uint32_t AudioRenderInterfaceImpl::GetMinVolumeInner()
{
    std::lock_guard<std::mutex> volLck(volMtx_);
    return volMin_;
}
} // V1_0
} // Audio
} // Distributedaudio
} // HDI
} // OHOS
