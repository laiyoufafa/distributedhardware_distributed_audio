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

#ifndef OHOS_DAUDIO_MANAGER_INTERDFACE_IMPL_H
#define OHOS_DAUDIO_MANAGER_INTERDFACE_IMPL_H

#include <mutex>

#include <v1_0/id_audio_manager.h>

#include "audio_manager_interface_impl.h"

namespace OHOS {
namespace HDI {
namespace DistributedAudio {
namespace Audioext {
namespace V1_0 {
using OHOS::HDI::DistributedAudio::Audioext::V1_0::DAudioEvent;

class DAudioManagerInterfaceImpl : public IDAudioManager {
public:
    static DAudioManagerInterfaceImpl *GetDAudioManager()
    {
        if (dmgr == nullptr) {
            std::unique_lock<std::mutex> mgr_mutex(mutex_dmgr);
            if (dmgr == nullptr) {
                dmgr = new DAudioManagerInterfaceImpl();
            }
        }
        return dmgr;
    }

    ~DAudioManagerInterfaceImpl() override;

    int32_t RegisterAudioDevice(const std::string &adpName, int32_t devId, const std::string &capability,
        const sptr<IDAudioCallback> &callbackObj) override;

    int32_t UnRegisterAudioDevice(const std::string &adpName, int32_t devId) override;

    int32_t NotifyEvent(const std::string &adpName, int32_t devId, const DAudioEvent &event) override;

private:
    DAudioManagerInterfaceImpl();

    DAudioManagerInterfaceImpl(const DAudioManagerInterfaceImpl &);

    DAudioManagerInterfaceImpl &operator = (const DAudioManagerInterfaceImpl &);

private:
    class Deletor {
    public:
        ~Deletor()
        {
            if (DAudioManagerInterfaceImpl::dmgr != nullptr) {
                delete DAudioManagerInterfaceImpl::dmgr;
            }
        };
    };
    static Deletor deletor;

private:
    OHOS::HDI::DistributedAudio::Audio::V1_0::AudioManagerInterfaceImpl *audiomgr_;
    static DAudioManagerInterfaceImpl *dmgr;
    static std::mutex mutex_dmgr;
};
} // V1_0
} // AudioExt
} // Distributedaudio
} // HDI
} // OHOS
#endif // OHOS_DAUDIO_MANAGER_INTERDFACE_IMPL_H
