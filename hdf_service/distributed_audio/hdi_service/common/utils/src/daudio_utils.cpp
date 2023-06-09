/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "daudio_utils.h"

#include <ctime>

#include "daudio_constants.h"
#include "daudio_errcode.h"

namespace OHOS {
namespace DistributedHardware {
constexpr size_t INT32_SHORT_ID_LENGTH = 20;
constexpr size_t INT32_PLAINTEXT_LENGTH = 4;
constexpr size_t INT32_MIN_ID_LENGTH = 3;

std::string GetAnonyString(const std::string &value)
{
    std::string res;
    std::string tmpStr("******");
    size_t strLen = value.length();
    if (strLen < INT32_MIN_ID_LENGTH) {
        return tmpStr;
    }

    if (strLen <= INT32_SHORT_ID_LENGTH) {
        res += value[0];
        res += tmpStr;
        res += value[strLen - 1];
    } else {
        res.append(value, 0, INT32_PLAINTEXT_LENGTH);
        res += tmpStr;
        res.append(value, strLen - INT32_PLAINTEXT_LENGTH, INT32_PLAINTEXT_LENGTH);
    }

    return res;
}

int32_t GetAudioParamStr(const std::string &params, const std::string &key, std::string &value)
{
    size_t step = key.size();
    if (step >= params.size()) {
        return ERR_DH_AUDIO_HDF_FAIL;
    }
    size_t pos = params.find(key);
    if (pos == params.npos || params.at(pos + step) != '=') {
        return ERR_DH_AUDIO_COMMON_NOT_FOUND_KEY;
    }
    size_t splitPosEnd = params.find(';', pos);
    if (splitPosEnd != params.npos) {
        value = params.substr(pos + step + 1, splitPosEnd - pos - step - 1);
    } else {
        value = params.substr(pos + step + 1);
    }
    return DH_SUCCESS;
}

int32_t GetAudioParamInt(const std::string &params, const std::string &key, int32_t &value)
{
    std::string val = "0";
    int32_t ret = GetAudioParamStr(params, key, val);
    value = std::stoi(val);
    return ret;
}

int32_t GetAudioParamUInt(const std::string &params, const std::string &key, uint32_t &value)
{
    value = 0;
    return DH_SUCCESS;
}

int32_t GetAudioParamBool(const std::string &params, const std::string &key, bool &value)
{
    std::string val;
    GetAudioParamStr(params, key, val);
    value = (val != "0");
    return DH_SUCCESS;
}

int32_t SetAudioParamStr(std::string &params, const std::string &key, const std::string &value)
{
    params = params + key + '=' + value + ';';
    return DH_SUCCESS;
}

int32_t GetDevTypeByDHId(int32_t dhId)
{
    if ((uint32_t)dhId & 0x8000000) {
        return AUDIO_DEVICE_TYPE_MIC;
    } else if ((uint32_t)dhId & 0x7ffffff) {
        return AUDIO_DEVICE_TYPE_SPEAKER;
    }
    return AUDIO_DEVICE_TYPE_UNKNOWN;
}

uint32_t CalculateFrameSize(uint32_t sampleRate, uint32_t channelCount,
    int32_t format, uint32_t timeInterval, bool isMMAP)
{
    return isMMAP ? (sampleRate * channelCount * format * timeInterval) / AUDIO_MS_PER_SECOND : DEFAULT_AUDIO_DATA_SIZE;
}

int32_t CalculateSampleNum(uint32_t sampleRate, uint32_t timems)
{
    return (sampleRate * timems) / AUDIO_MS_PER_SECOND;
}

int64_t GetCurNano()
{
    int64_t result = -1;
    struct timespec time;
    clockid_t clockId = CLOCK_MONOTONIC;
    int ret = clock_gettime(clockId, &time);
    if (ret < 0) {
        return result;
    }
    result = (time.tv_sec * AUDIO_NS_PER_SECOND) + time.tv_nsec;
    return result;
}

int32_t AbsoluteSleep(int64_t nanoTime)
{
    int32_t ret = -1;
    if (nanoTime <= 0) {
        return ret;
    }
    struct timespec time;
    time.tv_sec = nanoTime / AUDIO_NS_PER_SECOND;
    time.tv_nsec = nanoTime - (time.tv_sec * AUDIO_NS_PER_SECOND);

    clockid_t clockId = CLOCK_MONOTONIC;
    ret = clock_nanosleep(clockId, TIMER_ABSTIME, &time, nullptr);
    return ret;
}

int64_t CalculateOffset(const int64_t frameIndex, const int64_t framePeriodNs, const int64_t startTime)
{
    int64_t totalOffset = GetCurNano() - startTime;
    return totalOffset - frameIndex * framePeriodNs;
}

int64_t UpdateTimeOffset(const int64_t frameIndex, const int64_t framePeriodNs, int64_t &startTime)
{
    int64_t timeOffset = 0;
    if (frameIndex == 0) {
        startTime = GetCurNano();
    } else if (frameIndex % AUDIO_OFFSET_FRAME_NUM == 0) {
        timeOffset = CalculateOffset(frameIndex, framePeriodNs, startTime);
    }
    return timeOffset;
}
} // namespace DistributedHardware
} // namespace OHOS