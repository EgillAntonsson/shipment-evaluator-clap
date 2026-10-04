#pragma once

#include "MyPluginData.h"
#include <clap/clap.h>

class MyPlugin {
private:
    MyPluginData data_;
    const clap_host_t *host_;

public:
    MyPlugin(const clap_host_t *host);
    ~MyPlugin();
    
    // C ABI compliant interface functions
    static bool init(const struct clap_plugin *plugin);
    static void destroy(const struct clap_plugin *plugin);
    static bool activate(const struct clap_plugin *plugin, 
                        double sample_rate, 
                        uint32_t min_frames_count, 
                        uint32_t max_frames_count);
    static void deactivate(const struct clap_plugin *plugin);
    static bool start_processing(const struct clap_plugin *plugin);
    static void stop_processing(const struct clap_plugin *plugin);
    static void reset(const struct clap_plugin *plugin);
    static clap_process_status process(const struct clap_plugin *plugin,
                                       const clap_process_t *process);
    
    // Access the CLAP interface
    const clap_plugin_t* get_clap_interface() const;
    
private:
    void process_audio(const clap_process_t *process);
    float process_sample(float input, uint32_t param_index);
};