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

#include "dspeaker_client.h"

#include "daudio_constants.h"
#include "daudio_hisysevent.h"
#include "dh_utils_tool.h"

namespace OHOS {
namespace DistributedHardware {
DSpeakerClient::~DSpeakerClient()
{
    DHLOGI("%s: ~DSpeakerClient. Release speaker client.", LOG_TAG);
}

int32_t DSpeakerClient::SetUp(const AudioParam &param)
{
    DHLOGI("%s: SetUp spk client.", LOG_TAG);
    DHLOGI("%s: Param: {sampleRate: %d, bitFormat: %d, channelMask: %d, contentType: %d, streamUsage: %d}.", LOG_TAG,
        param.comParam.sampleRate, param.comParam.bitFormat, param.comParam.channelMask, param.renderOpts.contentType,
        param.renderOpts.streamUsage);
    audioParam_ = param;
    audioParam_.comParam.bitFormat = SAMPLE_S16LE;
    AudioStandard::AudioRendererOptions rendererOptions = {
        {
            static_cast<AudioStandard::AudioSamplingRate>(audioParam_.comParam.sampleRate),
            AudioStandard::AudioEncodingType::ENCODING_PCM,
            static_cast<AudioStandard::AudioSampleFormat>(audioParam_.comParam.bitFormat),
            static_cast<AudioStandard::AudioChannel>(audioParam_.comParam.channelMask),
        },
        {
            static_cast<AudioStandard::ContentType>(audioParam_.renderOpts.contentType),
            static_cast<AudioStandard::StreamUsage>(audioParam_.renderOpts.streamUsage),
            NUMBER_ZERO,
        }
    };
    std::lock_guard<std::mutex> lck(devMtx_);
    audioRenderer_ = AudioStandard::AudioRenderer::Create(rendererOptions);
    if (audioRenderer_ == nullptr) {
        DHLOGE("%s: Audio renderer create failed.", LOG_TAG);
        return ERR_DH_AUDIO_CLIENT_CREATE_RENDER_FAILED;
    }
    speakerTrans_ = std::make_shared<AudioDecodeTransport>(devId_);
    int32_t ret = speakerTrans_->SetUp(audioParam_, audioParam_, shared_from_this(), "speaker");
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Speaker trans setup failed.", LOG_TAG);
        return ret;
    }
    ret = speakerTrans_->Start();
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Speaker trans start failed.", LOG_TAG);
        return ret;
    }
    auto pid = getpid();
    ret = AudioStandard::AudioSystemManager::GetInstance()->RegisterVolumeKeyEventCallback(pid, shared_from_this());
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Failed to register volume key event callback.", LOG_TAG);
        return ret;
    }
    clientStatus_ = CLIENT_STATUS_READY;
    return DH_SUCCESS;
}

int32_t DSpeakerClient::Release()
{
    DHLOGI("%s: Release spk client.", LOG_TAG);
    std::lock_guard<std::mutex> lck(devMtx_);
    if ((clientStatus_ != CLIENT_STATUS_READY && clientStatus_ != CLIENT_STATUS_STOP) || speakerTrans_ == nullptr) {
        DHLOGE("%s: Speaker status is wrong or spk is null, %d.", LOG_TAG, (int32_t)clientStatus_);
        return ERR_DH_AUDIO_SA_STATUS_ERR;
    }
    int32_t ret = speakerTrans_->Stop();
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Speaker trans stop failed.", LOG_TAG);
        return ret;
    }
    ret = speakerTrans_->Release();
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Speaker trans release failed.", LOG_TAG);
        return ret;
    }
    speakerTrans_ = nullptr;

    ret = AudioStandard::AudioSystemManager::GetInstance()->UnregisterVolumeKeyEventCallback(getpid());
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Failed to unregister volume key event callback.", LOG_TAG);
        return ret;
    }
    if (!audioRenderer_->Release()) {
        DHLOGE("%s: Audio renderer release failed.", LOG_TAG);
        return ERR_DH_AUDIO_CLIENT_RENDER_RELEASE_FAILED;
    }
    audioRenderer_ = nullptr;
    clientStatus_ = CLIENT_STATUS_IDLE;
    return DH_SUCCESS;
}

int32_t DSpeakerClient::StartRender()
{
    DHLOGI("%s: Start spk client.", LOG_TAG);
    std::lock_guard<std::mutex> lck(devMtx_);
    if (audioRenderer_ == nullptr || clientStatus_ != CLIENT_STATUS_READY) {
        DHLOGE("%s: Audio renderer init failed or spk status wrong, status: %d.", LOG_TAG, (int32_t)clientStatus_);
        DAudioHisysevent::GetInstance().SysEventWriteFault(DAUDIO_OPT_FAIL, ERR_DH_AUDIO_SA_STATUS_ERR,
            "daduio renderer init failed or spk status wrong.");
        return ERR_DH_AUDIO_SA_STATUS_ERR;
    }
    if (!audioRenderer_->Start()) {
        DHLOGE("%s: Audio renderer start failed.", LOG_TAG);
        DAudioHisysevent::GetInstance().SysEventWriteFault(DAUDIO_OPT_FAIL, ERR_DH_AUDIO_CLIENT_RENDER_STARTUP_FAILURE,
            "daduio renderer start failed.");
        return ERR_DH_AUDIO_CLIENT_RENDER_STARTUP_FAILURE;
    }
    isRenderReady_.store(true);
    renderDataThread_ = std::thread(&DSpeakerClient::PlayThreadRunning, this);
    clientStatus_ = CLIENT_STATUS_START;
    return DH_SUCCESS;
}

int32_t DSpeakerClient::StopRender()
{
    DHLOGI("%s: Stop spk client.", LOG_TAG);
    std::lock_guard<std::mutex> lck(devMtx_);
    if (clientStatus_ != CLIENT_STATUS_START || !isRenderReady_.load()) {
        DHLOGE("%s: Renderer is not start or spk status wrong, status: %d.", LOG_TAG, (int32_t)clientStatus_);
        DAudioHisysevent::GetInstance().SysEventWriteFault(DAUDIO_OPT_FAIL, ERR_DH_AUDIO_SA_STATUS_ERR,
            "daduio renderer is not start or spk status wrong.");
        return ERR_DH_AUDIO_SA_STATUS_ERR;
    }
    if (audioRenderer_ == nullptr || speakerTrans_ == nullptr) {
        DHLOGE("%s: Audio renderer or speaker trans is nullptr.", LOG_TAG);
        DAudioHisysevent::GetInstance().SysEventWriteFault(DAUDIO_OPT_FAIL, ERR_DH_AUDIO_CLIENT_RENDER_OR_TRANS_IS_NULL,
            "daduio renderer or speaker trans is nullptr.");
        return ERR_DH_AUDIO_CLIENT_RENDER_OR_TRANS_IS_NULL;
    }
    isRenderReady_.store(false);
    if (renderDataThread_.joinable()) {
        renderDataThread_.join();
    }

    if (!audioRenderer_->Stop()) {
        DHLOGE("%s: Audio renderer stop failed", LOG_TAG);
        DAudioHisysevent::GetInstance().SysEventWriteFault(DAUDIO_OPT_FAIL, ERR_DH_AUDIO_CLIENT_RENDER_STOP_FAILED,
            "daduio renderer stop failed.");
        return ERR_DH_AUDIO_CLIENT_RENDER_STOP_FAILED;
    }
    clientStatus_ = CLIENT_STATUS_STOP;
    return DH_SUCCESS;
}

void DSpeakerClient::PlayThreadRunning()
{
    DHLOGI("%s:Start the renderer thread.", LOG_TAG);
    while (isRenderReady_.load()) {
        std::unique_lock<std::mutex> spkLck(dataQueueMtx_);
        dataQueueCond_.wait_for(spkLck, std::chrono::milliseconds(REQUEST_DATA_WAIT),
            [this]() { return !dataQueue_.empty(); });
        if (dataQueue_.empty()) {
            continue;
        }

        std::shared_ptr<AudioData> audioData = nullptr;
        audioData = dataQueue_.front();
        dataQueue_.pop();
        int32_t writeLen = 0;
        int32_t writeOffSet = 0;
        while (writeOffSet < static_cast<int32_t>(audioData->Capacity())) {
            writeLen = audioRenderer_->Write(audioData->Data() + writeOffSet,
                static_cast<int32_t>(audioData->Capacity()) - writeOffSet);
            DHLOGD("write audio render, write len: %d, raw len: %d, offset: %d", writeLen, audioData->Capacity(),
                writeOffSet);
            if (writeLen < 0) {
                break;
            }
            writeOffSet += writeLen;
        }
    }
}

int32_t DSpeakerClient::WriteStreamBuffer(const std::shared_ptr<AudioData> &audioData)
{
    DHLOGI("%s: Write stream buffer.", LOG_TAG);
    std::lock_guard<std::mutex> lock(dataQueueMtx_);
    while (dataQueue_.size() > DATA_QUEUE_MAX_SIZE) {
        DHLOGE("%s: Data queue overflow.", LOG_TAG);
        dataQueue_.pop();
    }
    dataQueue_.push(audioData);
    dataQueueCond_.notify_all();
    DHLOGI("%s: Push new spk data, buf len: %d.", LOG_TAG, dataQueue_.size());
    return DH_SUCCESS;
}

int32_t DSpeakerClient::OnStateChange(int32_t type)
{
    DHLOGI("%s: On state change. type: %d", LOG_TAG, type);
    auto event = std::make_shared<AudioEvent>();
    switch (type) {
        case AudioEventType::DATA_OPENED: {
            event->type = AudioEventType::SPEAKER_OPENED;
            event->content = GetVolumeLevel();
            break;
        }
        case AudioEventType::DATA_CLOSED: {
            event->type = AudioEventType::SPEAKER_CLOSED;
            break;
        }
        default:
            DHLOGE("%s: Invalid parameter type: %d.", LOG_TAG, type);
            break;
    }
    if (eventCallback_ == nullptr) {
        DHLOGE("%s: Event callback is null.", LOG_TAG);
        return ERR_DH_AUDIO_NULLPTR;
    }
    eventCallback_->NotifyEvent(event);
    return DH_SUCCESS;
}

string DSpeakerClient::GetVolumeLevel()
{
    DHLOGI("%s: GetVolumeLevel begin.", LOG_TAG);
    std::stringstream ss;
    AudioStandard::AudioStreamType streamType = AudioStandard::AudioStreamType::STREAM_DEFAULT;
    auto volumeType = static_cast<AudioStandard::AudioVolumeType>(1);
    int32_t volumeLevel = AudioStandard::AudioSystemManager::GetInstance()->GetVolume(volumeType);
    int32_t maxVolumeLevel = AudioStandard::AudioSystemManager::GetInstance()->GetMaxVolume(volumeType);
    int32_t minVolumeLevel = AudioStandard::AudioSystemManager::GetInstance()->GetMinVolume(volumeType);
    bool isUpdateUi = false;
    ss << "FIRST_VOLUME_CHANAGE;"
       << "AUDIO_STREAM_TYPE=" << streamType << ";"
       << "VOLUME_LEVEL=" << volumeLevel << ";"
       << "IS_UPDATEUI=" << isUpdateUi << ";"
       << "MAX_VOLUME_LEVEL=" << maxVolumeLevel << ";"
       << "MIN_VOLUME_LEVEL=" << minVolumeLevel << ";";
    std::string str = ss.str();
    DHLOGI("%s: GetVolumeLevel result, event: %s.", LOG_TAG, str.c_str());
    return str;
}

void DSpeakerClient::OnVolumeKeyEvent(AudioStandard::VolumeEvent volumeEvent)
{
    DHLOGI("%s: OnVolumeKeyEvent.", LOG_TAG);
    if (eventCallback_ == nullptr) {
        DHLOGE("%s: EventCallback is nullptr.", LOG_TAG);
        return;
    }
    std::stringstream ss;
    ss << "VOLUME_CHANAGE;"
       << "AUDIO_STREAM_TYPE=" << volumeEvent.volumeType << ";"
       << "VOLUME_LEVEL=" << volumeEvent.volume << ";"
       << "IS_UPDATEUI=" << volumeEvent.updateUi << ";"
       << "VOLUME_GROUP_ID=" << volumeEvent.volumeGroupId << ";";
    std::string str = ss.str();
    DHLOGI("%s: Volume change notification result, event: %s.", LOG_TAG, str.c_str());

    std::shared_ptr<AudioEvent> audioEvent = std::make_shared<AudioEvent>();
    audioEvent->type = VOLUME_CHANGE;
    audioEvent->content = str;
    eventCallback_->NotifyEvent(audioEvent);
}

void DSpeakerClient::OnInterrupt(const AudioStandard::InterruptEvent &interruptEvent)
{
    DHLOGI("%s: OnInterrupt.", LOG_TAG);
    if (eventCallback_ == nullptr) {
        DHLOGE("%s: EventCallback is nullptr.", LOG_TAG);
        return;
    }
    std::stringstream ss;
    ss << "INTERRUPT_EVENT;"
       << "EVENT_TYPE=" << interruptEvent.eventType << ";"
       << "FORCE_TYPE=" << interruptEvent.forceType << ";"
       << "HINT_TYPE=" << interruptEvent.hintType << ";";
    std::string str = ss.str();
    DHLOGI("%s: Audio focus oninterrupt notification result, event: %s.", LOG_TAG, str.c_str());

    std::shared_ptr<AudioEvent> audioEvent = std::make_shared<AudioEvent>();
    audioEvent->type = AUDIO_FOCUS_CHANGE;
    audioEvent->content = str;
    eventCallback_->NotifyEvent(audioEvent);
}

void DSpeakerClient::OnStateChange(const AudioStandard::RendererState state)
{
    DHLOGI("%s: On render state change. state: %d", LOG_TAG, state);
    if (eventCallback_ == nullptr) {
        DHLOGE("%s: EventCallback is nullptr.", LOG_TAG);
        return;
    }
    std::stringstream ss;
    ss << "RENDER_STATE_CHANGE_EVENT;"
       << "STATE=" << state << ";";
    std::string str = ss.str();
    DHLOGI("%s: Audio render state changes notification result, event: %s.", LOG_TAG, str.c_str());

    std::shared_ptr<AudioEvent> audioEvent = std::make_shared<AudioEvent>();
    audioEvent->type = AUDIO_RENDER_STATE_CHANGE;
    audioEvent->content = str;
    eventCallback_->NotifyEvent(audioEvent);
}

int32_t DSpeakerClient::GetAudioParameters(const std::shared_ptr<AudioEvent> &event)
{
    DHLOGI("%s: Get the volume.", LOG_TAG);
    std::shared_ptr<AudioEvent> audioEvent = event;
    std::vector<std::string> volumeList;
    StringSplit(audioEvent->content, '=', volumeList);
    if (volumeList[NUMBER_ZERO] != "VOLUMETYPE") {
        DHLOGE("%s: Invalid event content parameter.", LOG_TAG);
        return ERR_DH_AUDIO_CLIENT_INVALID_EVENT_PARAM;
    }
    auto volumeType =
        static_cast<AudioStandard::AudioVolumeType>(std::stoi(volumeList[NUMBER_ONE]));
    switch (audioEvent->type) {
        case VOLUME_GET: {
            auto volume = AudioStandard::AudioSystemManager::GetInstance()->GetVolume(volumeType);
            DHLOGI("%s: Get the current volume, volume: %d.", LOG_TAG, volume);
            return volume;
        }
        case VOLUME_MIN_GET: {
            auto volume = AudioStandard::AudioSystemManager::GetInstance()->GetMinVolume(volumeType);
            DHLOGI("%s: Get the Minimum  volume, volume: %d.", LOG_TAG, volume);
            return volume;
        }
        case VOLUME_MAX_GET: {
            auto volume = AudioStandard::AudioSystemManager::GetInstance()->GetMaxVolume(volumeType);
            DHLOGI("%s: Get the Maximum  volume, volume: %d.", LOG_TAG, volume);
            return volume;
        }
        default:
            DHLOGE("%s: Invalid event type parameter.", LOG_TAG);
            return ERR_DH_AUDIO_CLIENT_INVALID_EVENT_PARAM;
    }
}

int32_t DSpeakerClient::SetAudioParameters(const std::shared_ptr<AudioEvent> &event)
{
    DHLOGI("%s: Set the volume, arg: %s.", LOG_TAG, event->content.c_str());
    std::shared_ptr<AudioEvent> audioEvent = event;
    std::vector<std::string> conditionList;
    std::vector<std::string> typeList;
    std::vector<std::string> levelList;
    StringSplit(audioEvent->content, ';', conditionList);
    StringSplit(conditionList[NUMBER_TWO], '=', typeList);
    StringSplit(conditionList[NUMBER_THREE], '=', levelList);

    if (typeList[NUMBER_ZERO] != "AUDIO_VOLUME_TYPE") {
        DHLOGE("%s: Invalid event content parameter.", LOG_TAG);
        return ERR_DH_AUDIO_CLIENT_INVALID_EVENT_PARAM;
    }
    auto volumeType = static_cast<AudioStandard::AudioVolumeType>(std::stoi(typeList[NUMBER_ONE]));
    DHLOGE("%s: AudioVolumeType volumeType = %d.", LOG_TAG, volumeType);
    if (audioEvent->type != VOLUME_SET && audioEvent->type != VOLUME_MUTE_SET) {
        DHLOGE("%s: Invalid parameter.", LOG_TAG);
        return ERR_DH_AUDIO_CLIENT_INVALID_VOLUME_PARAMETER;
    }
    DHLOGE("%s: volume level = %d.", LOG_TAG, std::stoi(levelList[NUMBER_ONE]));
    int32_t ret =
        AudioStandard::AudioSystemManager::GetInstance()->SetVolume(volumeType, std::stoi(levelList[NUMBER_ONE]));
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Voloume set failed.", LOG_TAG);
        return ERR_DH_AUDIO_CLIENT_SET_VOLUME_FAILED;
    }
    return DH_SUCCESS;
}

void DSpeakerClient::StringSplit(const std::string &str, const uint8_t &splits, std::vector<std::string> &res)
{
    if (splits != '=' && splits != ';') {
        DHLOGI("%s: Splits error.", LOG_TAG);
        return;
    }
    char ch = splits;

    if (str.empty()) {
        DHLOGI("%s: The string is empty.", LOG_TAG);
        return;
    }
    std::stringstream ss(str);
    std::string temp;
    while (getline(ss, temp, ch)) {
        res.push_back(temp);
    }
}
} // DistributedHardware
} // OHOS
