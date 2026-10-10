#pragma once

#include "ShipmentEvaluatorGui.h"
#include <atomic>
#include <clap/clap.h>

class ShipmentEvaluatorPlugin {
public:
    explicit ShipmentEvaluatorPlugin(const clap_host_t *host);
    ~ShipmentEvaluatorPlugin();

    const clap_plugin_t *get_clap_interface() const;

private:
    std::atomic<double> evaluation_meter_{0.0};
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
    static void reset(const clap_plugin_t *plugin);
    static clap_process_status process(const clap_plugin_t *plugin,
                                      const clap_process_t *process);
    static const void *get_extension(const clap_plugin_t *plugin, const char *id);

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
};
