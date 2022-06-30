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

#ifndef OHOS_AUDIO_CTRL_CHANNEL_H
#define OHOS_AUDIO_CTRL_CHANNEL_H

#include "iaudio_channel_listener.h"
#include "iaudio_channel.h"
#include "softbus_adapter.h"

namespace OHOS {
namespace DistributedHardware {
class AudioCtrlChannel : public IAudioChannel,
    public ISoftbusListener,
    public std::enable_shared_from_this<AudioCtrlChannel> {
public:
    explicit AudioCtrlChannel(const std::string &peerDevId) : peerDevId_(peerDevId) {};
    ~AudioCtrlChannel() = default;

    int32_t CreateSession(const std::shared_ptr<IAudioChannelListener> &listener,
        const std::string &sessionName) override;
    int32_t ReleaseSession() override;
    int32_t OpenSession() override;
    int32_t CloseSession() override;
    int32_t SendData(const std::shared_ptr<AudioData> &audioData) override;
    int32_t SendEvent(const std::shared_ptr<AudioEvent> &audioEvent) override;

    void OnSessionOpened(int32_t sessionId, int32_t result) override;
    void OnSessionClosed(int32_t sessionId) override;
    void OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen) override;
    void OnStreamReceived(int32_t sessionId, const StreamData *data, const StreamData *ext,
        const StreamFrameInfo *streamFrameInfo) override;

private:
    static const constexpr char *LOG_TAG = "AudioCtrlChannel";

    const std::string peerDevId_;
    int32_t sessionId_ = 0;
    std::string sessionName_;
    std::weak_ptr<IAudioChannelListener> channelListener_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_AUDIO_CTRL_CHANNEL_H
