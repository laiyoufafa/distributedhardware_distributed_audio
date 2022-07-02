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

#ifndef HDF_I_AUDIO_RENDER_H
#define HDF_I_AUDIO_RENDER_H

#include <string>
#include <vector>

#include "i_audio_attribute.h"
#include "i_audio_control.h"
#include "i_audio_scene.h"
#include "i_audio_render_callback.h"
#include "i_audio_volume.h"
#include "types.h"

namespace OHOS {
namespace DistributedHardware {
class IAudioRender : public IAudioControl, public IAudioAttribute, public IAudioScene, public IAudioVolume {
public:
    virtual int32_t GetLatency(uint32_t &ms) = 0;

    virtual int32_t RenderFrame(const std::vector<uint8_t> &frame, const uint64_t requestBytes,
        uint64_t &replyBytes) = 0;

    virtual int32_t GetRenderPosition(uint64_t &frames, AudioTimeStampHAL &time) = 0;

    virtual int32_t SetRenderSpeed(const float speed) = 0;

    virtual int32_t GetRenderSpeed(float &speed) = 0;

    virtual int32_t SetChannelMode(const AudioChannelModeHAL &mode) = 0;

    virtual int32_t GetChannelMode(AudioChannelModeHAL &mode) = 0;

    virtual int32_t RegCallback(IAudioRenderCallback &cbObj) = 0;

    virtual int32_t DrainBuffer(const AudioDrainNotifyTypeHAL &type) = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // HDF_I_AUDIO_RENDER_H