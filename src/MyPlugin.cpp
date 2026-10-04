#include "MyPlugin.h"
#include <cstring>
#include <cmath>

// Plugin descriptor - define plugin properties
static const clap_plugin_descriptor_t plugin_descriptor = {
    .clap_version = CLAP_VERSION,
    .id = "com.yourcompany.myclapplugin",
    .name = "My CLAP Plugin",
    .vendor = "Your Company",
    .url = "https://yourcompany.com/",
    .manual_url = "https://yourcompany.com/manual",
    .support_url = "https://yourcompany.com/support",
    .version = "0.1.0",
    .description = "A simple CLAP plugin implementation",
    .features = NULL
};

MyPlugin::MyPlugin(const clap_host_t *host) : host_(host) {
    // Initialize plugin data
    data_.sample_rate = 44100.0;
    data_.parameters.fill(0.0f);
    std::memset(data_.delay_line, 0, sizeof(data_.delay_line));
    data_.delay_ptr = 0;
}

MyPlugin::~MyPlugin() {
    // Cleanup if needed
}

bool MyPlugin::init(const struct clap_plugin *plugin) {
    // Plugin initialization
    return true;
}

void MyPlugin::destroy(const struct clap_plugin *plugin) {
    // Cleanup plugin instance
    delete static_cast<MyPlugin*>(plugin->plugin_data);
}

bool MyPlugin::activate(const struct clap_plugin *plugin, 
                        double sample_rate, 
                        uint32_t min_frames_count, 
                        uint32_t max_frames_count) {
    MyPlugin* my_plugin = static_cast<MyPlugin*>(plugin->plugin_data);
    my_plugin->data_.sample_rate = sample_rate;
    return true;
}

void MyPlugin::deactivate(const struct clap_plugin *plugin) {
    // Plugin deactivation
}

bool MyPlugin::start_processing(const struct clap_plugin *plugin) {
    return true;
}

void MyPlugin::stop_processing(const struct clap_plugin *plugin) {
    // Stop processing 
}

void MyPlugin::reset(const struct clap_plugin *plugin) {
    MyPlugin* my_plugin = static_cast<MyPlugin*>(plugin->plugin_data);
    std::memset(my_plugin->data_.delay_line, 0, sizeof(my_plugin->data_.delay_line));
    my_plugin->data_.delay_ptr = 0;
}

clap_process_status MyPlugin::process(const struct clap_plugin *plugin,
                                     const clap_process_t *process) {
    MyPlugin* my_plugin = static_cast<MyPlugin*>(plugin->plugin_data);
    
    // Process audio
    my_plugin->process_audio(process);
    
    return CLAP_PROCESS_CONTINUE;
}

const clap_plugin_t* MyPlugin::get_clap_interface() const {
    static const clap_plugin_t plugin_interface = {
        .desc = &plugin_descriptor,
        .plugin_data = const_cast<MyPlugin*>(this),
        .init = init,
        .destroy = destroy,
        .activate = activate,
        .deactivate = deactivate,
        .start_processing = start_processing,
        .stop_processing = stop_processing,
        .reset = reset,
        .process = process,
        .get_extension = nullptr,  // No extensions for simple plugin
        .on_main_thread = nullptr
    };
    
    return &plugin_interface;
}

void MyPlugin::process_audio(const clap_process_t *process) {
    // Simple audio processing example - just pass through with gain
    uint32_t nframes = process->out_frames;
    
    if (nframes > 0 && process->audio_outputs_count > 0 && 
        process->audio_outputs[0].data32) {
        
        float* output_buffer = process->audio_outputs[0].data32;
        float gain = data_.parameters[0]; // First parameter for gain
        
        for (uint32_t frame = 0; frame < nframes; ++frame) {
            output_buffer[frame] = output_buffer[frame] * gain;
        }
    }
}

float MyPlugin::process_sample(float input, uint32_t param_index) {
    // Example processing - could be replaced with more complex logic
    if (param_index == 0) {
        // Gain processing as example  
        return input * data_.parameters[0];
    }
    
    return input;
}