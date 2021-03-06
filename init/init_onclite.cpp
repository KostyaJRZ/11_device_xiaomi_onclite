/*
  Copyright (C) 2020 Paranoid Android

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 */

#include <android-base/properties.h>

#include <string>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include <sys/sysinfo.h>
#include "property_service.h"
#include "vendor_init.h"

using android::init::property_set;

void property_override(const std::string &prop, const std::string &value) {
    auto pi = (prop_info*) __system_property_find(prop.c_str());

    if (pi != nullptr)
        __system_property_update(pi, value.c_str(), value.size());
    else
        __system_property_add(prop.c_str(), prop.size(), value.c_str(), value.size());
}

void load_props(const std::string &device, const std::string &model) {
    std::string RO_PROP_SOURCES[] = { "", "odm.", "system.", "vendor." };

    for (const std::string &source : RO_PROP_SOURCES) {
        property_override(std::string("ro.product.") + source + std::string("name"), device);
        property_override(std::string("ro.product.") + source + std::string("device"), device);
        property_override(std::string("ro.product.") + source + std::string("model"), model);
    }
}

void set_dalvik_properties() {
    struct sysinfo sys;
    sysinfo(&sys);

    if (sys.totalram > 3072ull * 1024 * 1024) {
        // Set for 4GB RAM
        property_set("dalvik.vm.heapstartsize", "8m");
        property_set("dalvik.vm.heapgrowthlimit", "192m");
        property_set("dalvik.vm.heapsize", "512m");
        property_set("dalvik.vm.heaptargetutilization", "0.6");
        property_set("dalvik.vm.heapmaxfree", "16m");
        property_set("dalvik.vm.heapminfree", "8m");
    } else {
        // Set for 2/3GB RAM
        property_set("dalvik.vm.heapstartsize", "8m");
        property_set("dalvik.vm.heapgrowthlimit", "192m");
        property_set("dalvik.vm.heapsize", "512m");
        property_set("dalvik.vm.heaptargetutilization", "0.75");
        property_set("dalvik.vm.heapmaxfree", "8m");
        property_set("dalvik.vm.heapminfree", "512k");
    }
}

void set_avoid_gfxaccel_config() {
    struct sysinfo sys;
    sysinfo(&sys);

    if (sys.totalram <= 2048ull * 1024 * 1024) {
        // Reduce memory footprint
        property_set("ro.config.avoid_gfx_accel", "true");
    }
}

void vendor_load_properties() {
    std::string boot_cert = android::base::GetProperty("ro.boot.product.cert", "");

    if (boot_cert == "M1810F6LG" || boot_cert == "M1810F6LH" || boot_cert == "M1810F6LI"
            || boot_cert == "M1810F6LE" || boot_cert == "M1810F6LT" || boot_cert == "M1810F6LC")
        load_props("onclite", "Redmi 7");
    else
        load_props("onc", "Redmi Y3");

    set_dalvik_properties();
    set_avoid_gfxaccel_config();
}
