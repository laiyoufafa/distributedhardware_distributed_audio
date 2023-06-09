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

#ifndef OHOS_AUDIO_DECODER_CALLBACK_H
#define OHOS_AUDIO_DECODER_CALLBACK_H

#include <memory>

#include "avcodec_common.h"
#include "media_errors.h"
#include "format.h"

#include "audio_decoder.h"

namespace OHOS {
namespace DistributedHardware {
class AudioDecoder;

class AudioDecoderCallback : public Media::AVCodecCallback {
public:
    explicit AudioDecoderCallback(const std::weak_ptr<AudioDecoder> &decoder)
        : audioDecoder_(decoder) {};
    ~AudioDecoderCallback() override = default;

    void OnError(Media::AVCodecErrorType errorType, int32_t errorCode) override;
    void OnOutputBufferAvailable(uint32_t index, Media::AVCodecBufferInfo info, Media::AVCodecBufferFlag flag) override;
    void OnInputBufferAvailable(uint32_t index) override;
    void OnOutputFormatChanged(const Media::Format &format) override;

private:
    std::weak_ptr<AudioDecoder> audioDecoder_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_AUDIO_DECODER_CALLBACK_H
