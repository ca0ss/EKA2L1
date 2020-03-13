/*
 * Copyright (c) 2020 EKA2L1 Team.
 * 
 * This file is part of EKA2L1 project.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <drivers/audio/backend/cubeb/audio_cubeb.h>
#include <drivers/audio/backend/cubeb/stream_cubeb.h>
#include <common/log.h>
#include <common/platform.h>

#if EKA2L1_PLATFORM(WIN32)
#include <objbase.h>
#endif

namespace eka2l1::drivers {
    cubeb_audio_driver::cubeb_audio_driver()
        : context_(nullptr) {
#if EKA2L1_PLATFORM(WIN32)
        HRESULT hr = S_OK;
        hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        
        if (hr != S_OK) {
            LOG_CRITICAL("Failed to initialize COM");
            return;
        }
#endif

        if (cubeb_init(&context_, "EKA2L1 Audio Driver", nullptr) != CUBEB_OK) {
            LOG_CRITICAL("Can't initialize Cubeb audio driver!");
            return;
        }
    }

    cubeb_audio_driver::~cubeb_audio_driver() {
        if (context_) {
            cubeb_destroy(context_);
        }
    }

    std::uint32_t cubeb_audio_driver::native_sample_rate() {
        std::uint32_t preferred_rate = 0;
        const auto result = cubeb_get_preferred_sample_rate(context_, &preferred_rate);

        if (result != CUBEB_OK) {
            return 0;
        }

        return preferred_rate;
    }

    std::unique_ptr<audio_output_stream> cubeb_audio_driver::new_output_stream(const std::uint32_t sample_rate,
        data_callback callback) {
        return std::make_unique<cubeb_audio_output_stream>(context_, sample_rate, callback);
    }
};