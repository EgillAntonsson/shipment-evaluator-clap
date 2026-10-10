#include "ShipmentEvaluatorPlugin.h"
#include <algorithm>
#include <cstring>

static const char *plugin_features[] = {
    CLAP_PLUGIN_FEATURE_UTILITY,
    nullptr
};

static const clap_plugin_descriptor_t plugin_descriptor = {
    .clap_version = CLAP_VERSION,
    .id = "rocks.egill.shipment-evaluator",
    .name = "The Shipment Evaluator",
    .vendor = "Egill Antonsson",
    .url = "https://www.egill.rocks/blog/rea-scripts/shipment-evaluator",
    .manual_url = "https://www.egill.rocks/blog/rea-scripts/shipment-evaluator",
    .support_url = "https://www.egill.rocks/",
    .version = "0.1.0",
    .description = "A fixed-result evaluation utility. No audio is processed.",
    .features = plugin_features
};

const clap_plugin_descriptor_t *shipment_evaluator_descriptor() {
    return &plugin_descriptor;
}

ShipmentEvaluatorPlugin::ShipmentEvaluatorPlugin(const clap_host_t *host)
    : gui_(&evaluation_meter_) {
    (void)host;
    plugin_interface_ = {
        .desc = &plugin_descriptor,
        .plugin_data = this,
        .init = init,
        .destroy = destroy,
        .activate = activate,
        .deactivate = deactivate,
        .start_processing = start_processing,
        .stop_processing = stop_processing,
        .reset = reset,
        .process = process,
        .get_extension = get_extension,
        .on_main_thread = nullptr
    };
}

ShipmentEvaluatorPlugin::~ShipmentEvaluatorPlugin() {
    gui_.destroy();
}

const clap_plugin_t *ShipmentEvaluatorPlugin::get_clap_interface() const {
    return &plugin_interface_;
}

ShipmentEvaluatorPlugin *ShipmentEvaluatorPlugin::from_plugin(const clap_plugin_t *plugin) {
    return plugin ? static_cast<ShipmentEvaluatorPlugin *>(plugin->plugin_data) : nullptr;
}

bool CLAP_ABI ShipmentEvaluatorPlugin::init(const clap_plugin_t *) {
    return true;
}

void CLAP_ABI ShipmentEvaluatorPlugin::destroy(const clap_plugin_t *plugin) {
    delete from_plugin(plugin);
}

bool CLAP_ABI ShipmentEvaluatorPlugin::activate(const clap_plugin_t *plugin,
                                                double,
                                                uint32_t,
                                                uint32_t) {
    ShipmentEvaluatorPlugin *instance = from_plugin(plugin);
    if (!instance)
        return false;

    instance->evaluation_meter_.store(0.0, std::memory_order_relaxed);
    return true;
}

void CLAP_ABI ShipmentEvaluatorPlugin::deactivate(const clap_plugin_t *plugin) {
    reset(plugin);
}

bool CLAP_ABI ShipmentEvaluatorPlugin::start_processing(const clap_plugin_t *) {
    return true;
}

void CLAP_ABI ShipmentEvaluatorPlugin::stop_processing(const clap_plugin_t *) {}

void CLAP_ABI ShipmentEvaluatorPlugin::reset(const clap_plugin_t *plugin) {
    ShipmentEvaluatorPlugin *instance = from_plugin(plugin);
    if (!instance)
        return;

    instance->evaluation_meter_.store(0.0, std::memory_order_relaxed);
}

clap_process_status CLAP_ABI ShipmentEvaluatorPlugin::process(
    const clap_plugin_t *plugin,
    const clap_process_t *process) {
    ShipmentEvaluatorPlugin *instance = from_plugin(plugin);
    if (!instance || !process)
        return CLAP_PROCESS_ERROR;

    const bool playing = process->transport &&
        (process->transport->flags & CLAP_TRANSPORT_IS_PLAYING);

    if (playing) {
        const double meter = instance->evaluation_meter_.load(std::memory_order_relaxed);
        instance->evaluation_meter_.store(
            std::min(11.0, meter + 0.025), std::memory_order_relaxed);
    } else {
        instance->evaluation_meter_.store(0.0, std::memory_order_relaxed);
    }

    return CLAP_PROCESS_CONTINUE;
}

const void *CLAP_ABI ShipmentEvaluatorPlugin::get_extension(const clap_plugin_t *,
                                                            const char *id) {
    static const clap_plugin_gui_t gui_extension = {
        .is_api_supported = gui_is_api_supported,
        .get_preferred_api = gui_get_preferred_api,
        .create = gui_create,
        .destroy = gui_destroy,
        .set_scale = gui_set_scale,
        .get_size = gui_get_size,
        .can_resize = gui_can_resize,
        .get_resize_hints = gui_get_resize_hints,
        .adjust_size = gui_adjust_size,
        .set_size = gui_set_size,
        .set_parent = gui_set_parent,
        .set_transient = gui_set_transient,
        .suggest_title = gui_suggest_title,
        .show = gui_show,
        .hide = gui_hide
    };

    return id && std::strcmp(id, CLAP_EXT_GUI) == 0 ? &gui_extension : nullptr;
}

bool CLAP_ABI ShipmentEvaluatorPlugin::gui_is_api_supported(const clap_plugin_t *,
                                                             const char *api,
                                                             bool is_floating) {
    return !is_floating && api && std::strcmp(api, CLAP_WINDOW_API_COCOA) == 0;
}

bool CLAP_ABI ShipmentEvaluatorPlugin::gui_get_preferred_api(const clap_plugin_t *,
                                                              const char **api,
                                                              bool *is_floating) {
    if (!api || !is_floating)
        return false;

    *api = CLAP_WINDOW_API_COCOA;
    *is_floating = false;
    return true;
}

bool CLAP_ABI ShipmentEvaluatorPlugin::gui_create(const clap_plugin_t *plugin,
                                                  const char *api,
                                                  bool is_floating) {
    if (!gui_is_api_supported(plugin, api, is_floating))
        return false;

    ShipmentEvaluatorPlugin *instance = from_plugin(plugin);
    return instance && instance->gui_.create();
}

void CLAP_ABI ShipmentEvaluatorPlugin::gui_destroy(const clap_plugin_t *plugin) {
    if (ShipmentEvaluatorPlugin *instance = from_plugin(plugin))
        instance->gui_.destroy();
}

bool CLAP_ABI ShipmentEvaluatorPlugin::gui_set_scale(const clap_plugin_t *, double) {
    return false;
}

bool CLAP_ABI ShipmentEvaluatorPlugin::gui_get_size(const clap_plugin_t *plugin,
                                                    uint32_t *width,
                                                    uint32_t *height) {
    ShipmentEvaluatorPlugin *instance = from_plugin(plugin);
    return instance && instance->gui_.get_size(width, height);
}

bool CLAP_ABI ShipmentEvaluatorPlugin::gui_can_resize(const clap_plugin_t *) {
    return false;
}

bool CLAP_ABI ShipmentEvaluatorPlugin::gui_get_resize_hints(const clap_plugin_t *,
                                                              clap_gui_resize_hints_t *) {
    return false;
}

bool CLAP_ABI ShipmentEvaluatorPlugin::gui_adjust_size(const clap_plugin_t *,
                                                       uint32_t *,
                                                       uint32_t *) {
    return false;
}

bool CLAP_ABI ShipmentEvaluatorPlugin::gui_set_size(const clap_plugin_t *plugin,
                                                    uint32_t width,
                                                    uint32_t height) {
    ShipmentEvaluatorPlugin *instance = from_plugin(plugin);
    return instance && instance->gui_.set_size(width, height);
}

bool CLAP_ABI ShipmentEvaluatorPlugin::gui_set_parent(const clap_plugin_t *plugin,
                                                      const clap_window_t *window) {
    ShipmentEvaluatorPlugin *instance = from_plugin(plugin);
    if (!instance || !window || !window->api ||
        std::strcmp(window->api, CLAP_WINDOW_API_COCOA) != 0)
        return false;

    return instance->gui_.set_parent(window->cocoa);
}

bool CLAP_ABI ShipmentEvaluatorPlugin::gui_set_transient(const clap_plugin_t *,
                                                          const clap_window_t *) {
    return false;
}

void CLAP_ABI ShipmentEvaluatorPlugin::gui_suggest_title(const clap_plugin_t *, const char *) {}

bool CLAP_ABI ShipmentEvaluatorPlugin::gui_show(const clap_plugin_t *plugin) {
    ShipmentEvaluatorPlugin *instance = from_plugin(plugin);
    return instance && instance->gui_.show();
}

bool CLAP_ABI ShipmentEvaluatorPlugin::gui_hide(const clap_plugin_t *plugin) {
    ShipmentEvaluatorPlugin *instance = from_plugin(plugin);
    return instance && instance->gui_.hide();
}
