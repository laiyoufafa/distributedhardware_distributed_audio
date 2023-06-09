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

#include "daudio_utils_test.h"

#include <thread>

#include "securec.h"

#include "daudio_constants.h"
#include "daudio_errorcode.h"
#include "daudio_latency_test.h"
#include "daudio_log.h"
#include "daudio_util.h"

#undef DH_LOG_TAG
#define DH_LOG_TAG "DAudioUtilsTest"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
constexpr static int64_t TEMP_BEEP_TIME_INTERVAL_US = 10000; // 10ms
constexpr static int64_t MIN_BEEP_TIME_INTERVAL_US = 900000; // 900ms

void DAudioUtilsTest::SetUpTestCase(void) {}

void DAudioUtilsTest::TearDownTestCase(void) {}

void DAudioUtilsTest::SetUp(void) {}

void DAudioUtilsTest::TearDown(void) {}

/**
 * @tc.name: DAudioLatencyTest_001
 * @tc.desc: Verify the DAudioLatencyTest AddPlayTime, AddRecordTime and ComputeLatency function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DAudioUtilsTest, DAudioLatencyTest_001, TestSize.Level1)
{
    int32_t latency = DAudioLatencyTest::GetInstance().ComputeLatency();
    EXPECT_EQ(-1, latency);

    int64_t t = GetNowTimeUs();
    EXPECT_EQ(ERR_DH_AUDIO_BAD_VALUE, DAudioLatencyTest::GetInstance().AddRecordTime(t));

    t = GetNowTimeUs();
    EXPECT_EQ(DH_SUCCESS, DAudioLatencyTest::GetInstance().AddPlayTime(t));
    EXPECT_EQ(ERR_DH_AUDIO_FAILED, DAudioLatencyTest::GetInstance().AddPlayTime(t + TEMP_BEEP_TIME_INTERVAL_US));

    std::this_thread::sleep_for(std::chrono::microseconds(MIN_BEEP_TIME_INTERVAL_US));
    t = GetNowTimeUs();
    EXPECT_EQ(DH_SUCCESS, DAudioLatencyTest::GetInstance().AddPlayTime(t));

    t = GetNowTimeUs();
    EXPECT_EQ(DH_SUCCESS, DAudioLatencyTest::GetInstance().AddRecordTime(t));
    EXPECT_EQ(ERR_DH_AUDIO_FAILED, DAudioLatencyTest::GetInstance().AddRecordTime(t + TEMP_BEEP_TIME_INTERVAL_US));

    latency = DAudioLatencyTest::GetInstance().ComputeLatency();
    EXPECT_EQ(-1, latency);

    std::this_thread::sleep_for(std::chrono::microseconds(MIN_BEEP_TIME_INTERVAL_US));
    t = GetNowTimeUs();
    EXPECT_EQ(DH_SUCCESS, DAudioLatencyTest::GetInstance().AddRecordTime(t));

    latency = DAudioLatencyTest::GetInstance().ComputeLatency();
    EXPECT_LE(0, latency);
}

/**
 * @tc.name: DAudioLatencyTest_002
 * @tc.desc: Verify the DAudioLatencyTest IsFrameHigh function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DAudioUtilsTest, DAudioLatencyTest_002, TestSize.Level1)
{
    int32_t threshhold = 5000;
    int32_t spanSizeInByte = 960;
    std::unique_ptr<uint8_t[]> buf = std::make_unique<uint8_t[]>(spanSizeInByte);
    memset_s(buf.get(), spanSizeInByte, 0, spanSizeInByte);
    bool isHigh = DAudioLatencyTest::GetInstance().IsFrameHigh(reinterpret_cast<int16_t *>(buf.get()),
        spanSizeInByte / sizeof(int16_t), threshhold);
    EXPECT_EQ(false, isHigh);

    memset_s(buf.get(), spanSizeInByte, threshhold, spanSizeInByte);
    isHigh = DAudioLatencyTest::GetInstance().IsFrameHigh(reinterpret_cast<int16_t *>(buf.get()),
        spanSizeInByte / sizeof(int16_t), threshhold);
    EXPECT_EQ(true, isHigh);
}

/**
 * @tc.name: DAudioLatencyTest_003
 * @tc.desc: Verify the DAudioLatencyTest IsFrameHigh function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DAudioUtilsTest, DAudioLatencyTest_003, TestSize.Level1)
{
    bool status = true;
    int32_t threshhold = 8000;
    int32_t spanSizeInByte = 960;
    std::unique_ptr<uint8_t[]> buf = std::make_unique<uint8_t[]>(spanSizeInByte);
    memset_s(buf.get(), spanSizeInByte, threshhold, spanSizeInByte);
    int64_t beepTime = DAudioLatencyTest::GetInstance().RecordBeepTime(static_cast<uint8_t *>(buf.get()),
        spanSizeInByte, status);
    EXPECT_NE(0, beepTime);
    EXPECT_EQ(false, status);

    memset_s(buf.get(), spanSizeInByte, 0, spanSizeInByte);
    beepTime = DAudioLatencyTest::GetInstance().RecordBeepTime(static_cast<uint8_t *>(buf.get()),
        spanSizeInByte, status);
    EXPECT_EQ(0, beepTime);
    EXPECT_EQ(true, status);
}

/**
 * @tc.name: DAudioLogTest_001
 * @tc.desc: Verify the DHLOG definition and DHLog function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DAudioUtilsTest, DAudioLogTest_001, TestSize.Level1)
{
    DHLOGD("DAudio TDD test DHLOGD print.");
    DHLOGI("DAudio TDD test DHLOGI print.");
    DHLOGW("DAudio TDD test DHLOGW print.");
    DHLOGE("DAudio TDD test DHLOGE print.");
    DHLog(DHLogLevel::DH_LOG_ERROR, "");
}

/**
 * @tc.name: DAudioLogTest_001
 * @tc.desc: Verify the GetCurrentTime function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DAudioUtilsTest, DAudioUtilTest_001, TestSize.Level1)
{
    int64_t tvSec;
    int64_t tvNSec;
    GetCurrentTime(tvSec, tvNSec);
}

/**
 * @tc.name: DAudioLogTest_002
 * @tc.desc: Verify the GetCurrentTime, GetCurNano and AbsoluteSleep function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DAudioUtilsTest, DAudioUtilTest_002, TestSize.Level1)
{
    int64_t tvSec;
    int64_t tvNSec;
    GetCurrentTime(tvSec, tvNSec);

    int64_t curNano = GetCurNano();
    EXPECT_NE(0, curNano);

    int32_t ret = AbsoluteSleep(curNano);
    EXPECT_EQ(0, ret);
}

/**
 * @tc.name: DAudioLogTest_003
 * @tc.desc: Verify the CalculateSampleNum and UpdateTimeOffset function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DAudioUtilsTest, DAudioUtilTest_003, TestSize.Level1)
{
    uint32_t sampleRate = 48000;
    uint32_t timeInterval = 5;
    int32_t desiredSpanSizeInFrame = 240;
    int32_t spanSizeInFrame = CalculateSampleNum(sampleRate, timeInterval);
    EXPECT_EQ(desiredSpanSizeInFrame, spanSizeInFrame);

    int64_t frameIndex = 0;
    int64_t framePeriodNs = 5000000;
    int64_t startTime = 0;
    int64_t timeOffset = UpdateTimeOffset(frameIndex, framePeriodNs, startTime);
    EXPECT_NE(0, startTime);
    EXPECT_EQ(0, timeOffset);

    frameIndex = AUDIO_OFFSET_FRAME_NUM / 2;
    timeOffset = UpdateTimeOffset(frameIndex, framePeriodNs, startTime);
    EXPECT_EQ(0, timeOffset);

    frameIndex = AUDIO_OFFSET_FRAME_NUM;
    timeOffset = UpdateTimeOffset(frameIndex, framePeriodNs, startTime);
    EXPECT_NE(0, timeOffset);
}

/**
 * @tc.name: DAudioLogTest_004
 * @tc.desc: Verify the GetAudioParamBool function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DAudioUtilsTest, DAudioUtilTest_004, TestSize.Level1)
{
    std::string params = "";
    std::string key = "";
    bool value = false;
    EXPECT_EQ(ERR_DH_AUDIO_FAILED, GetAudioParamBool(params, key, value));

    params = "params";
    key = "key";
    EXPECT_EQ(ERR_DH_AUDIO_NOT_FOUND_KEY, GetAudioParamBool(params, key, value));

    params = "key=0";
    EXPECT_EQ(DH_SUCCESS, GetAudioParamBool(params, key, value));
    EXPECT_EQ(false, value);

    params = "param1=true;key=1;param2=false;";
    EXPECT_EQ(DH_SUCCESS, GetAudioParamBool(params, key, value));
    EXPECT_EQ(true, value);
}

/**
 * @tc.name: DAudioLogTest_005
 * @tc.desc: Verify the GetAudioParamInt function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DAudioUtilsTest, DAudioUtilTest_005, TestSize.Level1)
{
    std::string params = "";
    std::string key = "";
    int32_t value = 5;
    EXPECT_EQ(ERR_DH_AUDIO_FAILED, GetAudioParamInt(params, key, value));

    params = "params";
    key = "key";
    EXPECT_EQ(ERR_DH_AUDIO_NOT_FOUND_KEY, GetAudioParamInt(params, key, value));

    params = "key=0";
    EXPECT_EQ(DH_SUCCESS, GetAudioParamInt(params, key, value));
    EXPECT_EQ(0, value);

    params = "param1=true;key=1;param2=false;";
    EXPECT_EQ(DH_SUCCESS, GetAudioParamInt(params, key, value));
    EXPECT_EQ(1, value);
}

/**
 * @tc.name: DAudioLogTest_006
 * @tc.desc: Verify the JsonParamCheck function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DAudioUtilsTest, DAudioUtilTest_006, TestSize.Level1)
{
    std::string tempKey = "TestParam";
    json jParam = { { KEY_DEV_ID, "TEST_DEV_ID" }, {KEY_AUDIO_PARAM, "TEST_PARAM" }, {KEY_FORMAT, "TEST_8000" },
        { tempKey, "TEST_TEMP_KEY" } };
    EXPECT_EQ(false, JsonParamCheck(jParam, { KEY_ATTRS }));
    EXPECT_EQ(false, JsonParamCheck(jParam, { KEY_AUDIO_PARAM }));
    EXPECT_EQ(false, JsonParamCheck(jParam, { KEY_FORMAT }));
    EXPECT_EQ(false, JsonParamCheck(jParam, { tempKey }));
    EXPECT_EQ(true, JsonParamCheck(jParam, { KEY_DEV_ID }));
}

/**
 * @tc.name: DAudioLogTest_007
 * @tc.desc: Verify the CheckIsNum and CheckDevIdIsLegal function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DAudioUtilsTest, DAudioUtilTest_007, TestSize.Level1)
{
    uint8_t maxDhIdLen = 20;
    std::string tempDhIdStr(maxDhIdLen + 1, 'a');
    EXPECT_EQ(false, CheckIsNum(tempDhIdStr));

    tempDhIdStr = "";
    EXPECT_EQ(false, CheckIsNum(tempDhIdStr));

    tempDhIdStr = "TestParams";
    EXPECT_EQ(false, CheckIsNum(tempDhIdStr));

    tempDhIdStr = "1";
    EXPECT_EQ(true, CheckIsNum(tempDhIdStr));

    std::string tempDevIdStr(DAUDIO_MAX_DEVICE_ID_LEN + 1, 'a');
    EXPECT_EQ(false, CheckDevIdIsLegal(tempDevIdStr));

    tempDevIdStr = "";
    EXPECT_EQ(false, CheckDevIdIsLegal(tempDevIdStr));

    tempDevIdStr = "Test*Params#";
    EXPECT_EQ(false, CheckDevIdIsLegal(tempDevIdStr));

    tempDevIdStr = "Test1";
    EXPECT_EQ(true, CheckDevIdIsLegal(tempDevIdStr));
}
} // namespace DistributedHardware
} // namespace OHOS
