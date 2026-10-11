#pragma once

#include "ShipmentEvaluatorGui.h"
#include <atomic>
#include <clap/clap.h>
#include <clap/ext/log.h>

inline constexpr clap_version_t kShipmentEvaluatorClapVersion{1, 2, 7};

class ShipmentEvaluatorPlugin {
public:
    explicit ShipmentEvaluatorPlugin(const clap_host_t *host);
    ~ShipmentEvaluatorPlugin();

    const clap_plugin_t *get_clap_interface() const;

private:
    const clap_host_t *host_;
    const clap_host_log_t *host_log_ = nullptr;
    std::atomic<double> evaluation_meter_{0.0};
    std::atomic<bool> process_diagnostic_logged_{false};
    std::atomic<uint32_t> last_transport_flags_{UINT32_MAX};
    std::atomic<uint64_t> playing_diagnostic_frames_{0};
    std::atomic<uint32_t> gui_callback_count_{0};
    uint32_t gui_callback_frame_threshold_ = 1600;
    uint64_t gui_callback_frame_accumulator_ = 0;
    clap_plugin_t plugin_interface_{};
    ShipmentEvaluatorGui gui_;

    static bool init(const clap_plugin_t *plugin);
    static void destroy(const clap_plugin_t *plugin);
    static bool activate(const clap_plugin_t *plugin,
                         double sample_rate,
                         uint32_t min_frames_count,
                         uint32_t max_frames_count);
    static void deactivate(const clap_plugin_t *plugin);
    static bool start_processing(const clap_plugin_t *plugin);
    static void stop_processing(const clap_plugin_t *plugin);
    static void on_main_thread(const clap_plugin_t *plugin);
    static void reset(const clap_plugin_t *plugin);
    static clap_process_status process(const clap_plugin_t *plugin,
                                      const clap_process_t *process);
    static const void *get_extension(const clap_plugin_t *plugin, const char *id);
    static uint32_t audio_port_count(const clap_plugin_t *plugin, bool is_input);
    static bool audio_port_get(const clap_plugin_t *plugin,
                               uint32_t index,
                               bool is_input,
                               clap_audio_port_info_t *info);
    static uint32_t audio_ports_config_count(const clap_plugin_t *plugin);
    static bool audio_ports_config_get(const clap_plugin_t *plugin,
                                       uint32_t index,
                                       clap_audio_ports_config_t *config);
    static bool audio_ports_config_select(const clap_plugin_t *plugin, clap_id config_id);
    static clap_id audio_ports_config_current(const clap_plugin_t *plugin);
    static bool audio_ports_config_info_get(const clap_plugin_t *plugin,
                                            clap_id config_id,
                                            uint32_t port_index,
                                            bool is_input,
                                            clap_audio_port_info_t *info);

    static bool gui_is_api_supported(const clap_plugin_t *plugin,
                                     const char *api,
                                     bool is_floating);
    static bool gui_get_preferred_api(const clap_plugin_t *plugin,
                                      const char **api,
                                      bool *is_floating);
    static bool gui_create(const clap_plugin_t *plugin, const char *api, bool is_floating);
    static void gui_destroy(const clap_plugin_t *plugin);
    static bool gui_set_scale(const clap_plugin_t *plugin, double scale);
    static bool gui_get_size(const clap_plugin_t *plugin, uint32_t *width, uint32_t *height);
    static bool gui_can_resize(const clap_plugin_t *plugin);
    static bool gui_get_resize_hints(const clap_plugin_t *plugin,
                                     clap_gui_resize_hints_t *hints);
    static bool gui_adjust_size(const clap_plugin_t *plugin, uint32_t *width, uint32_t *height);
    static bool gui_set_size(const clap_plugin_t *plugin, uint32_t width, uint32_t height);
    static bool gui_set_parent(const clap_plugin_t *plugin, const clap_window_t *window);
    static bool gui_set_transient(const clap_plugin_t *plugin, const clap_window_t *window);
    static void gui_suggest_title(const clap_plugin_t *plugin, const char *title);
    static bool gui_show(const clap_plugin_t *plugin);
    static bool gui_hide(const clap_plugin_t *plugin);
    static ShipmentEvaluatorPlugin *from_plugin(const clap_plugin_t *plugin);
    void log_host(clap_log_severity severity, const char *message) const;
};
