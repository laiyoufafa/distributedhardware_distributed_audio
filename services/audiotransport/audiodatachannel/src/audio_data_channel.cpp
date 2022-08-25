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

#include "audio_data_channel.h"

#include <securec.h>

#include "daudio_hitrace.h"

#undef DH_LOG_TAG
#define DH_LOG_TAG "AudioDataChannel"

namespace OHOS {
namespace DistributedHardware {
int32_t AudioDataChannel::CreateSession(const std::shared_ptr<IAudioChannelListener> &listener,
    const std::string &sessionName)
{
    DHLOGI("CreateSession, peerDevId: %s.", GetAnonyString(peerDevId_).c_str());
    if (!listener) {
        DHLOGE("Channel listener is null.");
        return ERR_DH_AUDIO_TRANS_NULL_VALUE;
    }

    DAUDIO_SYNC_TRACE(DAUDIO_CREATE_DATA_SESSION);
    int32_t ret =
        SoftbusAdapter::GetInstance().CreateSoftbusSessionServer(PKG_NAME, sessionName, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("Create softbus session failed ret.");
        return ret;
    }

    std::shared_ptr<ISoftbusListener> softbusListener = shared_from_this();
    ret = SoftbusAdapter::GetInstance().RegisterSoftbusListener(softbusListener, sessionName, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("Register softbus adapter listener failed ret: %d.", ret);
        return ret;
    }

    channelListener_ = listener;
    sessionName_ = sessionName;
    DHLOGI("Create softbus session success.");
    return DH_SUCCESS;
}

int32_t AudioDataChannel::ReleaseSession()
{
    DHLOGI("ReleaseSession, peerDevId: %s.", GetAnonyString(peerDevId_).c_str());
    DAUDIO_SYNC_TRACE(DAUDIO_RELEASE_DATA_SESSION);
    int32_t ret = SoftbusAdapter::GetInstance().RemoveSoftbusSessionServer(PKG_NAME, sessionName_, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("Release softbus session failed ret: %d.", ret);
        return ret;
    }

    ret = SoftbusAdapter::GetInstance().UnRegisterSoftbusListener(sessionName_, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("UnRegister softbus adapter listener failed ret: %d.", ret);
        return ret;
    }
    channelListener_.reset();

    DHLOGI("Release softbus session success.");
    return DH_SUCCESS;
}

int32_t AudioDataChannel::OpenSession()
{
    DHLOGI("OpenSession, peerDevId: %s.", GetAnonyString(peerDevId_).c_str());
    DaudioStartAsyncTrace(DAUDIO_OPEN_DATA_SESSION, DAUDIO_OPEN_DATA_SESSION_TASKID);
    int32_t sessionId =
        SoftbusAdapter::GetInstance().OpenSoftbusSession(sessionName_, sessionName_, peerDevId_);
    if (sessionId < 0) {
        DHLOGE("Open audio session failed, ret: %d.", sessionId);
        return ERR_DH_AUDIO_TRANS_ERROR;
    }
    sessionId_ = sessionId;

    DHLOGI("Open audio session success, sessionId: %d.", sessionId_);
    return DH_SUCCESS;
}

int32_t AudioDataChannel::CloseSession()
{
    DHLOGI("CloseSession, sessionId: %d.", sessionId_);
    if (sessionId_ == 0) {
        DHLOGI("Session is already close.");
        return DH_SUCCESS;
    }

    DAUDIO_SYNC_TRACE(DAUDIO_CLOSE_DATA_SESSION);
    int32_t ret = SoftbusAdapter::GetInstance().CloseSoftbusSession(sessionId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("Close audio session failed ret: %d.", ret);
        return ret;
    }
    sessionId_ = 0;

    DHLOGI("Close audio session success.");
    return DH_SUCCESS;
}

int32_t AudioDataChannel::SendEvent(const std::shared_ptr<AudioEvent> &audioEvent)
{
    (void) audioEvent;
    return DH_SUCCESS;
}

int32_t AudioDataChannel::SendData(const std::shared_ptr<AudioData> &audioData)
{
    DHLOGI("SendData, sessionId: %d.", sessionId_);
    if (!audioData) {
        DHLOGE("Audio data is null.");
        return ERR_DH_AUDIO_TRANS_NULL_VALUE;
    }

    int32_t ret = SoftbusAdapter::GetInstance().SendSoftbusStream(sessionId_, audioData);
    if (ret != DH_SUCCESS) {
        DHLOGE("Send audio data failed ret: %d.", ret);
        return ret;
    }

    return DH_SUCCESS;
}

void AudioDataChannel::OnSessionOpened(int32_t sessionId, int32_t result)
{
    DHLOGI("OnAudioSessionOpened, sessionId: %d, result: %d.", sessionId, result);
    if (result != 0) {
        DHLOGE("Session open failed.");
        return;
    }

    std::shared_ptr<IAudioChannelListener> listener = channelListener_.lock();
    if (!listener) {
        DHLOGE("Channel listener is null.");
        return;
    }

    listener->OnSessionOpened();
    sessionId_ = sessionId;
    DaudioFinishAsyncTrace(DAUDIO_OPEN_DATA_SESSION, DAUDIO_OPEN_DATA_SESSION_TASKID);
}

void AudioDataChannel::OnSessionClosed(int32_t sessionId)
{
    DHLOGI("OnAudioSessionClosed, sessionId: %d.", sessionId);
    if (sessionId_ == 0) {
        DHLOGI("Session already closed.");
        return;
    }
    std::shared_ptr<IAudioChannelListener> listener = channelListener_.lock();
    if (!listener) {
        DHLOGE("Channel listener is null.");
        return;
    }
    listener->OnSessionClosed();
    sessionId_ = 0;
}

void AudioDataChannel::OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen)
{
    (void) sessionId;
    (void) data;
    (void) dataLen;

    DHLOGI("OnAudioBytesReceived data channel not support yet.");
}

void AudioDataChannel::OnStreamReceived(int32_t sessionId, const StreamData *data, const StreamData *ext,
    const StreamFrameInfo *streamFrameInfo)
{
    (void) ext;
    (void) streamFrameInfo;

    if (data == nullptr) {
        DHLOGE("Stream data is null.");
        return;
    }

    std::shared_ptr<IAudioChannelListener> listener = channelListener_.lock();
    if (!listener) {
        DHLOGE("Channel listener is null.");
        return;
    }

    DHLOGI("OnAudioStreamReceived, sessionId: %d dataSize: %zu.", sessionId, data->bufLen);
    auto audioData = std::make_shared<AudioData>(data->bufLen);
    if (!audioData) {
        DHLOGE("audioData is null.");
        return;
    }

    int32_t ret = memcpy_s(audioData->Data(), audioData->Capacity(), (uint8_t *)data->buf, data->bufLen);
    if (ret != EOK) {
        DHLOGE("Data memcpy_s failed.");
        return;
    }
    listener->OnDataReceived(audioData);
}
} // namespace DistributedHardware
} // namespace OHOS
