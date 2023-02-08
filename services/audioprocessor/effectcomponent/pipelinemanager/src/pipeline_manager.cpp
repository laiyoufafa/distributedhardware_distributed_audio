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

#include "pipeline_manager.h"

#include "daudio_errorcode.h"
#include "daudio_log.h"

#include "ipipeline.h"
#include "pipeline_impl.h"

#undef DH_LOG_TAG
#define DH_LOG_TAG "PipelineManager"
namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(PipelineManager);

int32_t PipelineManager::CreatePipeline(const PipeType &pipeType,
    const std::list<ElementType> &config, std::shared_ptr<IPipeline> &pipe)
{
    DHLOGI("Pipeline is creating, type: %d", static_cast<int32_t>(pipeType));
    if (CheckPipeConfig(config) != true) {
        DHLOGE("Pipeline config error.");
        return ERR_PIPE_ID;
    }
    auto pipeImpl = std::make_shared<PipelineImpl>(pipeType);
    if (pipeImpl == nullptr) {
        DHLOGE("Creating pipeline failed.");
        return ERR_PIPE_ID;
    }
    int32_t ret = pipeImpl->BuildPipeStream(config);
    if (ret != DH_SUCCESS) {
        DHLOGE("Build pipe stream failed.");
        return ERR_PIPE_ID;
    }

    pipe = pipeImpl;
    int32_t pipeId = GeneratePipeId();
    if (pipeId == ERR_PIPE_ID) {
        DHLOGE("Pipeline ID generated error.");
        return ERR_PIPE_ID;
    }
    mapPipelines_[pipeId] = pipe;
    return pipeId;
}

int32_t PipelineManager::ClearPipeline(int32_t pipeId)
{
    DHLOGI("Clearing pipeline.");
    auto ret = mapPipelines_.find(pipeId);
    if (ret == mapPipelines_.end()) {
        DHLOGE("pipe ID is not exist.");
        return ERR_DH_AUDIO_FAILED;
    }
    mapPipelines_.erase(pipeId);
    return DH_SUCCESS;
}

int32_t PipelineManager::GeneratePipeId()
{
    static int32_t pipeId = INITIAL_PIPE_ID;
    ++pipeId;
    if (pipeId > MAX_PIPELINE_SIZE) {
        DHLOGE("Pipe number is out the max size.");
        return ERR_PIPE_ID;
    }
    return pipeId;
}

bool PipelineManager::CheckPipeConfig(const std::list<ElementType> &config)
{
    DHLOGI("Checking pipeline's configure.");
    if (config.front() != ElementType::SOURCE) {
        return false;
    }
    if (config.back() != ElementType::REF_SINK && config.back() != ElementType::MIC_SINK) {
        return false;
    }
    return true;
}
} // DistributedHardware
} // OHOS