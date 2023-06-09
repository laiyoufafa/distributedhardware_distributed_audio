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

#include <gtest/gtest.h>
#include <memory>

#include "audio_data.h"
#include "audio_event.h"
#include "audio_decoder_processor.h"
#include "audio_param.h"
#include "audio_processor_callback_test.h"
#include "daudio_errorcode.h"
#include "daudio_log.h"
#include "daudio_util.h"
#include "iaudio_processor.h"
#include "iaudio_processor_callback.h"
#include "securec.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
class DecodeProcessTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<AudioProcessorCallbackTest> proCallback_;
    std::shared_ptr<AudioDecoderProcessor> decodeProc_;
};

const AudioCommonParam LOC_COMPARA_DEC_TEST = {SAMPLE_RATE_48000, STEREO, SAMPLE_S16LE, AUDIO_CODEC_AAC};
const AudioCommonParam RMT_COMPARA_DEC_TEST = {SAMPLE_RATE_48000, STEREO, SAMPLE_S16LE, AUDIO_CODEC_AAC};

void DecodeProcessTest::SetUpTestCase(void)
{
}

void DecodeProcessTest::TearDownTestCase(void)
{
}

void DecodeProcessTest::SetUp(void)
{
    proCallback_ = std::make_shared<AudioProcessorCallbackTest>();
    decodeProc_ = std::make_shared<AudioDecoderProcessor>();
}

void DecodeProcessTest::TearDown(void)
{
    proCallback_ = nullptr;
    decodeProc_ = nullptr;
}

/**
 * @tc.name: decode_process_test_001
 * @tc.desc: Verify the configure and release processor function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DecodeProcessTest, decode_process_test_001, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, decodeProc_->ConfigureAudioProcessor(LOC_COMPARA_DEC_TEST,
        RMT_COMPARA_DEC_TEST, proCallback_));

    EXPECT_EQ(DH_SUCCESS, decodeProc_->ReleaseAudioProcessor());
}

/**
 * @tc.name: decode_process_test_002
 * @tc.desc: Verify the configure processor function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DecodeProcessTest, decode_process_test_002, TestSize.Level1)
{
    const AudioCommonParam testLocalComPara = {SAMPLE_RATE_48000, STEREO, SAMPLE_S32LE, AUDIO_CODEC_FLAC};
    const AudioCommonParam testRemoteComPara = {SAMPLE_RATE_48000, STEREO, SAMPLE_S32LE, AUDIO_CODEC_FLAC};
    EXPECT_NE(DH_SUCCESS, decodeProc_->ConfigureAudioProcessor(testLocalComPara, testRemoteComPara, proCallback_));

    EXPECT_EQ(DH_SUCCESS, decodeProc_->ReleaseAudioProcessor());
}

/**
 * @tc.name: decode_process_test_003
 * @tc.desc: Verify the start and stop processor function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DecodeProcessTest, decode_process_test_003, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, decodeProc_->ConfigureAudioProcessor(LOC_COMPARA_DEC_TEST,
        RMT_COMPARA_DEC_TEST, proCallback_));
    EXPECT_EQ(DH_SUCCESS, decodeProc_->StartAudioProcessor());

    EXPECT_EQ(DH_SUCCESS, decodeProc_->StopAudioProcessor());
    EXPECT_EQ(DH_SUCCESS, decodeProc_->ReleaseAudioProcessor());
}

/**
 * @tc.name: decode_process_test_004
 * @tc.desc: Verify the start processor without configure processor function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DecodeProcessTest, decode_process_test_004, TestSize.Level1)
{
    EXPECT_NE(DH_SUCCESS, decodeProc_->StartAudioProcessor());

    EXPECT_NE(DH_SUCCESS, decodeProc_->StopAudioProcessor());
    EXPECT_EQ(DH_SUCCESS, decodeProc_->ReleaseAudioProcessor());
}

/**
 * @tc.name: decode_process_test_005
 * @tc.desc: Verify the stop processor without start processor function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DecodeProcessTest, decode_process_test_005, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, decodeProc_->ConfigureAudioProcessor(LOC_COMPARA_DEC_TEST,
        RMT_COMPARA_DEC_TEST, proCallback_));

    EXPECT_NE(DH_SUCCESS, decodeProc_->StopAudioProcessor());
    EXPECT_EQ(DH_SUCCESS, decodeProc_->ReleaseAudioProcessor());
}

/**
 * @tc.name: decode_process_test_006
 * @tc.desc: Verify the start and stop processor again function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DecodeProcessTest, decode_process_test_006, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, decodeProc_->ConfigureAudioProcessor(LOC_COMPARA_DEC_TEST,
        RMT_COMPARA_DEC_TEST, proCallback_));

    EXPECT_EQ(DH_SUCCESS, decodeProc_->StartAudioProcessor());
    EXPECT_EQ(DH_SUCCESS, decodeProc_->StopAudioProcessor());

    EXPECT_EQ(DH_SUCCESS, decodeProc_->StartAudioProcessor());
    EXPECT_EQ(DH_SUCCESS, decodeProc_->StopAudioProcessor());

    EXPECT_EQ(DH_SUCCESS, decodeProc_->ReleaseAudioProcessor());
}

/**
 * @tc.name: decode_process_test_007
 * @tc.desc: Verify the start processor again function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DecodeProcessTest, decode_process_test_007, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, decodeProc_->ConfigureAudioProcessor(LOC_COMPARA_DEC_TEST,
        RMT_COMPARA_DEC_TEST, proCallback_));

    EXPECT_EQ(DH_SUCCESS, decodeProc_->StartAudioProcessor());
    EXPECT_NE(DH_SUCCESS, decodeProc_->StartAudioProcessor());

    EXPECT_EQ(DH_SUCCESS, decodeProc_->StopAudioProcessor());
    EXPECT_EQ(DH_SUCCESS, decodeProc_->ReleaseAudioProcessor());
}

/**
 * @tc.name: decode_process_test_008
 * @tc.desc: Verify the stop processor again function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DecodeProcessTest, decode_process_test_008, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, decodeProc_->ConfigureAudioProcessor(LOC_COMPARA_DEC_TEST,
        RMT_COMPARA_DEC_TEST, proCallback_));
    EXPECT_EQ(DH_SUCCESS, decodeProc_->StartAudioProcessor());

    EXPECT_EQ(DH_SUCCESS, decodeProc_->StopAudioProcessor());
    EXPECT_NE(DH_SUCCESS, decodeProc_->StopAudioProcessor());

    EXPECT_EQ(DH_SUCCESS, decodeProc_->ReleaseAudioProcessor());
}

/**
 * @tc.name: decode_process_test_009
 * @tc.desc: Verify feed audio processor function.
 * @tc.type: FUNC
 * @tc.require: AR000H0J88 SR000H0J7O
 */
HWTEST_F(DecodeProcessTest, decode_process_test_009, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, decodeProc_->ConfigureAudioProcessor(LOC_COMPARA_DEC_TEST,
        RMT_COMPARA_DEC_TEST, proCallback_));
    EXPECT_EQ(DH_SUCCESS, decodeProc_->StartAudioProcessor());

    size_t bufLen = 4096;
    std::shared_ptr<AudioData> inputData = std::make_shared<AudioData>(bufLen);
    EXPECT_EQ(EOK, memset_s(inputData->Data(), inputData->Size(), 0, inputData->Size()));
    EXPECT_EQ(DH_SUCCESS, decodeProc_->FeedAudioProcessor(inputData));

    EXPECT_EQ(DH_SUCCESS, decodeProc_->StopAudioProcessor());
    EXPECT_EQ(DH_SUCCESS, decodeProc_->ReleaseAudioProcessor());
}

/**
 * @tc.name: decode_process_test_0010
 * @tc.desc: Verify feed audio processor function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DecodeProcessTest, decode_process_test_0010, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, decodeProc_->ConfigureAudioProcessor(LOC_COMPARA_DEC_TEST,
        RMT_COMPARA_DEC_TEST, proCallback_));
    EXPECT_EQ(DH_SUCCESS, decodeProc_->StartAudioProcessor());

    std::shared_ptr<AudioData> inputData = nullptr;
    EXPECT_NE(DH_SUCCESS, decodeProc_->FeedAudioProcessor(inputData));
    decodeProc_->OnCodecDataDone(inputData);
    AudioEvent event;
    decodeProc_->OnCodecStateNotify(event);

    EXPECT_EQ(DH_SUCCESS, decodeProc_->StopAudioProcessor());
    EXPECT_EQ(DH_SUCCESS, decodeProc_->ReleaseAudioProcessor());
}

/**
 * @tc.name: decode_process_test_0011
 * @tc.desc: Verify feed audio processor function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5U
 */
HWTEST_F(DecodeProcessTest, decode_process_test_0011, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, decodeProc_->ConfigureAudioProcessor(LOC_COMPARA_DEC_TEST,
        RMT_COMPARA_DEC_TEST, proCallback_));
    decodeProc_ = std::make_shared<AudioDecoderProcessor>();
    EXPECT_EQ(ERR_DH_AUDIO_BAD_VALUE, decodeProc_->ConfigureAudioProcessor(LOC_COMPARA_DEC_TEST,
        RMT_COMPARA_DEC_TEST, nullptr));

    decodeProc_->OnCodecDataDone(nullptr);
    size_t bufLen = 4096;
    std::shared_ptr<AudioData> inputData = std::make_shared<AudioData>(bufLen);
    EXPECT_EQ(EOK, memset_s(inputData->Data(), inputData->Size(), 0, inputData->Size()));
    decodeProc_->OnCodecDataDone(inputData);
    AudioEvent event;
    decodeProc_->OnCodecStateNotify(event);
    EXPECT_EQ(ERR_DH_AUDIO_BAD_VALUE, decodeProc_->FeedAudioProcessor(inputData));
}
} // namespace DistributedHardware
} // namespace OHOS