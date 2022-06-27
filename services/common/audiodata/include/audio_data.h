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

#ifndef OHOS_DAUDIO_DATA_H
#define OHOS_DAUDIO_DATA_H

#include <map>
#include <string>

using std::map;
using std::string;

namespace OHOS {
namespace DistributedHardware {
class AudioData {
public:
    explicit AudioData(const size_t capacity);
    ~AudioData();

    size_t Size() const;
    size_t Offset() const;
    size_t Capacity() const;
    uint8_t *Data() const;
    int32_t SetRange(size_t offset, size_t size);

private:
    size_t capacity_ = 0;
    size_t rangeOffset_ = 0;
    size_t rangeLength_ = 0;
    uint8_t *data_ = nullptr;

    AudioData(const AudioData &) = delete;
    AudioData &operator = (const AudioData &) = delete;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DAUDIO_DATA_H
