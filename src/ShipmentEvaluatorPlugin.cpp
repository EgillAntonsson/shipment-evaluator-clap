#include "ShipmentEvaluatorPlugin.h"
#include "EvaluationMeter.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <os/log.h>

static os_log_t shipment_evaluator_log() {
    static os_log_t logger = os_log_create("rocks.egill.shipment-evaluator", "clap");
    return logger;
}

static const char *plugin_features[] = {
    CLAP_PLUGIN_FEATURE_AUDIO_EFFECT,
    CLAP_PLUGIN_FEATURE_UTILITY,
    CLAP_PLUGIN_FEATURE_STEREO,
    nullptr
};

static const clap_plugin_descriptor_t plugin_descriptor = {
    .clap_version = kShipmentEvaluatorClapVersion,
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
    : host_(host), gui_(&evaluation_meter_) {
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
        .on_main_thread = on_main_thread
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

void ShipmentEvaluatorPlugin::log_host(clap_log_severity severity,
                                       const char *message) const {
    if (host_ && host_log_ && host_log_->log)
        host_log_->log(host_, severity, message);
}

bool CLAP_ABI ShipmentEvaluatorPlugin::init(const clap_plugin_t *plugin) {
    ShipmentEvaluatorPlugin *instance = from_plugin(plugin);
    if (!instance)
        return false;

    if (instance->host_ && instance->host_->get_extension) {
        instance->host_log_ = static_cast<const clap_host_log_t *>(
            instance->host_->get_extension(instance->host_, CLAP_EXT_LOG));
    }
    const clap_version_t host_version = instance->host_
        ? instance->host_->clap_version
        : clap_version_t{};
    os_log_info(shipment_evaluator_log(),
                "CLAP init succeeded: host=%{public}s host_version=%u.%u.%u "
                "plugin_version=%u.%u.%u",
                instance->host_ && instance->host_->name
                    ? instance->host_->name
                    : "(unknown)",
                host_version.major,
                host_version.minor,
                host_version.revision,
                kShipmentEvaluatorClapVersion.major,
                kShipmentEvaluatorClapVersion.minor,
                kShipmentEvaluatorClapVersion.revision);
    instance->log_host(CLAP_LOG_INFO, "Shipment Evaluator: init succeeded");
    return true;
}

void CLAP_ABI ShipmentEvaluatorPlugin::destroy(const clap_plugin_t *plugin) {
    os_log_info(shipment_evaluator_log(), "CLAP plugin destroy");
    delete from_plugin(plugin);
}

bool CLAP_ABI ShipmentEvaluatorPlugin::activate(const clap_plugin_t *plugin,
                                                double sample_rate,
                                                uint32_t min_frames,
                                                uint32_t max_frames) {
    os_log_info(shipment_evaluator_log(),
                "CLAP activate entered: sample_rate=%f min_frames=%u max_frames=%u",
                sample_rate,
                min_frames,
                max_frames);
    ShipmentEvaluatorPlugin *instance = from_plugin(plugin);
    if (!instance)
        return false;

    const double meter = instance->evaluation_meter_.load(std::memory_order_relaxed);
    instance->evaluation_meter_.store(advance_evaluation_meter(meter, false),
                                      std::memory_order_relaxed);
    instance->gui_callback_frame_threshold_ =
        std::max(1u, static_cast<uint32_t>(sample_rate / 30.0));
    instance->gui_callback_frame_accumulator_ = 0;
    char message[128];
    std::snprintf(message, sizeof(message),
                  "Shipment Evaluator: activate succeeded at %.0f Hz", sample_rate);
    os_log_info(shipment_evaluator_log(), "CLAP activate succeeded at %f Hz", sample_rate);
    instance->log_host(CLAP_LOG_INFO, message);
    return true;
}

void CLAP_ABI ShipmentEvaluatorPlugin::deactivate(const clap_plugin_t *plugin) {
    if (ShipmentEvaluatorPlugin *instance = from_plugin(plugin))
        instance->log_host(CLAP_LOG_INFO, "Shipment Evaluator: deactivated");
    reset(plugin);
}

bool CLAP_ABI ShipmentEvaluatorPlugin::start_processing(const clap_plugin_t *plugin) {
    if (ShipmentEvaluatorPlugin *instance = from_plugin(plugin)) {
        os_log_info(shipment_evaluator_log(), "CLAP start_processing called");
        instance->log_host(CLAP_LOG_INFO, "Shipment Evaluator: start_processing succeeded");
    }
    return true;
}

void CLAP_ABI ShipmentEvaluatorPlugin::stop_processing(const clap_plugin_t *plugin) {
    if (ShipmentEvaluatorPlugin *instance = from_plugin(plugin))
        instance->log_host(CLAP_LOG_INFO, "Shipment Evaluator: stop_processing");
}

void CLAP_ABI ShipmentEvaluatorPlugin::on_main_thread(const clap_plugin_t *plugin) {
    if (ShipmentEvaluatorPlugin *instance = from_plugin(plugin)) {
        instance->gui_.refresh();
        const uint32_t callback_count =
            instance->gui_callback_count_.fetch_add(1, std::memory_order_relaxed);
        if (callback_count % 30 == 0) {
            os_log_info(shipment_evaluator_log(),
                        "CLAP GUI refresh callback: count=%u meter=%.3f",
                        callback_count + 1,
                        instance->evaluation_meter_.load(std::memory_order_relaxed));
        }
    }
}

void CLAP_ABI ShipmentEvaluatorPlugin::reset(const clap_plugin_t *plugin) {
    ShipmentEvaluatorPlugin *instance = from_plugin(plugin);
    if (!instance)
        return;

    const double meter = instance->evaluation_meter_.load(std::memory_order_relaxed);
    instance->evaluation_meter_.store(advance_evaluation_meter(meter, false),
                                      std::memory_order_relaxed);
}

clap_process_status CLAP_ABI ShipmentEvaluatorPlugin::process(
    const clap_plugin_t *plugin,
    const clap_process_t *process) {
    ShipmentEvaluatorPlugin *instance = from_plugin(plugin);
    if (!instance || !process)
        return CLAP_PROCESS_ERROR;

    if (!instance->process_diagnostic_logged_.exchange(true, std::memory_order_relaxed)) {
        const clap_audio_buffer_t *input =
            process->audio_inputs_count && process->audio_inputs
                ? &process->audio_inputs[0]
                : nullptr;
        const clap_audio_buffer_t *output =
            process->audio_outputs_count && process->audio_outputs
                ? &process->audio_outputs[0]
                : nullptr;
        char message[256];
        std::snprintf(message,
                      sizeof(message),
                      "Shipment Evaluator process: frames=%u inputs=%u (ch=%u, f32=%d, f64=%d) "
                      "outputs=%u (ch=%u, f32=%d, f64=%d)",
                      process->frames_count,
                      process->audio_inputs_count,
                      input ? input->channel_count : 0,
                      input && input->data32,
                      input && input->data64,
                      process->audio_outputs_count,
                      output ? output->channel_count : 0,
                      output && output->data32,
                      output && output->data64);
        os_log_info(shipment_evaluator_log(),
                    "CLAP process: frames=%u inputs=%u in_channels=%u in_f32=%d in_f64=%d "
                    "outputs=%u out_channels=%u out_f32=%d out_f64=%d",
                    process->frames_count,
                    process->audio_inputs_count,
                    input ? input->channel_count : 0,
                    input && input->data32,
                    input && input->data64,
                    process->audio_outputs_count,
                    output ? output->channel_count : 0,
                    output && output->data32,
                    output && output->data64);
        instance->log_host(CLAP_LOG_INFO, message);
    }

    if (process->frames_count > 0 && process->audio_outputs &&
        process->audio_outputs_count > 0) {
        const clap_audio_buffer_t *input =
            process->audio_inputs && process->audio_inputs_count > 0
                ? &process->audio_inputs[0]
                : nullptr;
        clap_audio_buffer_t *output = &process->audio_outputs[0];
        const uint32_t channels = std::min(input ? input->channel_count : 0,
                                           output->channel_count);
        output->constant_mask = 0;

        if (output->data32) {
            for (uint32_t channel = 0; channel < output->channel_count; ++channel) {
                float *out = output->data32[channel];
                if (!out)
                    continue;

                const bool has_input = input && channel < channels &&
                    ((input->data32 && input->data32[channel]) ||
                     (input->data64 && input->data64[channel]));
                if (has_input && input->data32 && input->data32[channel]) {
                    if (out != input->data32[channel]) {
                        std::memcpy(out,
                                    input->data32[channel],
                                    process->frames_count * sizeof(float));
                    }
                    if (channel < 64 && (input->constant_mask & (uint64_t{1} << channel)))
                        output->constant_mask |= uint64_t{1} << channel;
                } else if (has_input && input->data64 && input->data64[channel]) {
                    for (uint32_t frame = 0; frame < process->frames_count; ++frame)
                        out[frame] = static_cast<float>(input->data64[channel][frame]);
                } else {
                    std::memset(out, 0, process->frames_count * sizeof(float));
                    if (channel < 64)
                        output->constant_mask |= uint64_t{1} << channel;
                }
            }
        } else if (output->data64) {
            for (uint32_t channel = 0; channel < output->channel_count; ++channel) {
                double *out = output->data64[channel];
                if (!out)
                    continue;

                const bool has_input = input && channel < channels &&
                    ((input->data64 && input->data64[channel]) ||
                     (input->data32 && input->data32[channel]));
                if (has_input && input->data64 && input->data64[channel]) {
                    if (out != input->data64[channel]) {
                        std::memcpy(out,
                                    input->data64[channel],
                                    process->frames_count * sizeof(double));
                    }
                    if (channel < 64 && (input->constant_mask & (uint64_t{1} << channel)))
                        output->constant_mask |= uint64_t{1} << channel;
                } else if (has_input && input->data32 && input->data32[channel]) {
                    for (uint32_t frame = 0; frame < process->frames_count; ++frame)
                        out[frame] = input->data32[channel][frame];
                } else {
                    std::memset(out, 0, process->frames_count * sizeof(double));
                    if (channel < 64)
                        output->constant_mask |= uint64_t{1} << channel;
                }
            }
        }
    }

    const uint32_t transport_flags = process->transport ? process->transport->flags : 0;
    const bool playing = (transport_flags & CLAP_TRANSPORT_IS_PLAYING) != 0;
    const uint32_t previous_flags =
        instance->last_transport_flags_.exchange(transport_flags, std::memory_order_relaxed);
    bool request_gui_refresh = previous_flags != transport_flags;
    if (previous_flags != transport_flags) {
        os_log_info(shipment_evaluator_log(),
                    "CLAP transport changed: present=%d flags=0x%x playing=%d",
                    process->transport != nullptr,
                    transport_flags,
                    playing);
    }

    if (playing) {
        const double meter = instance->evaluation_meter_.load(std::memory_order_relaxed);
        const double updated_meter = advance_evaluation_meter(meter, true);
        instance->evaluation_meter_.store(updated_meter, std::memory_order_relaxed);
        if (updated_meter > meter) {
            if (evaluation_is_complete(updated_meter))
                request_gui_refresh = true;
            instance->gui_callback_frame_accumulator_ += process->frames_count;
            if (instance->gui_callback_frame_accumulator_ >=
                instance->gui_callback_frame_threshold_) {
                instance->gui_callback_frame_accumulator_ %=
                    instance->gui_callback_frame_threshold_;
                request_gui_refresh = true;
            }
        }

        const uint64_t previous_frames =
            instance->playing_diagnostic_frames_.fetch_add(process->frames_count,
                                                            std::memory_order_relaxed);
        if (previous_frames >= 48000) {
            instance->playing_diagnostic_frames_.fetch_sub(48000,
                                                           std::memory_order_relaxed);
            os_log_info(shipment_evaluator_log(),
                        "CLAP meter advancing: meter=%.3f transport_flags=0x%x",
                        updated_meter,
                        transport_flags);
        }
    } else {
        instance->evaluation_meter_.store(advance_evaluation_meter(0.0, false),
                                          std::memory_order_relaxed);
        instance->playing_diagnostic_frames_.store(0, std::memory_order_relaxed);
        instance->gui_callback_frame_accumulator_ = 0;
    }

    if (request_gui_refresh && instance->host_ && instance->host_->request_callback)
        instance->host_->request_callback(instance->host_);

    return CLAP_PROCESS_CONTINUE;
}

const void *CLAP_ABI ShipmentEvaluatorPlugin::get_extension(const clap_plugin_t *,
                                                            const char *id) {
    os_log_info(shipment_evaluator_log(),
                "CLAP get_extension: %{public}s",
                id ? id : "(null)");

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

    static const clap_plugin_audio_ports_t audio_ports_extension = {
        .count = audio_port_count,
        .get = audio_port_get
    };

    static const clap_plugin_audio_ports_config_t audio_ports_config_extension = {
        .count = audio_ports_config_count,
        .get = audio_ports_config_get,
        .select = audio_ports_config_select
    };

    static const clap_plugin_audio_ports_config_info_t audio_ports_config_info_extension = {
        .current_config = audio_ports_config_current,
        .get = audio_ports_config_info_get
    };

    if (id && std::strcmp(id, CLAP_EXT_AUDIO_PORTS) == 0)
        return &audio_ports_extension;
    if (id && std::strcmp(id, CLAP_EXT_AUDIO_PORTS_CONFIG) == 0)
        return &audio_ports_config_extension;
    if (id && (std::strcmp(id, CLAP_EXT_AUDIO_PORTS_CONFIG_INFO) == 0 ||
               std::strcmp(id, CLAP_EXT_AUDIO_PORTS_CONFIG_INFO_COMPAT) == 0))
        return &audio_ports_config_info_extension;
    return id && std::strcmp(id, CLAP_EXT_GUI) == 0 ? &gui_extension : nullptr;
}

uint32_t CLAP_ABI ShipmentEvaluatorPlugin::audio_port_count(const clap_plugin_t *,
                                                            bool is_input) {
    os_log_info(shipment_evaluator_log(),
                "CLAP audio_port_count: is_input=%d count=1",
                is_input);
    return 1;
}

bool CLAP_ABI ShipmentEvaluatorPlugin::audio_port_get(const clap_plugin_t *,
                                                      uint32_t index,
                                                      bool is_input,
                                                      clap_audio_port_info_t *info) {
    os_log_info(shipment_evaluator_log(),
                "CLAP audio_port_get: index=%u is_input=%d",
                index,
                is_input);
    if (index != 0 || !info)
        return false;

    *info = {};
    info->id = 0;
    std::strncpy(info->name,
                 is_input ? "Stereo Input" : "Stereo Output",
                 sizeof(info->name) - 1);
    info->flags = CLAP_AUDIO_PORT_IS_MAIN;
    info->channel_count = 2;
    info->port_type = CLAP_PORT_STEREO;
    info->in_place_pair = 0;
    return true;
}

uint32_t CLAP_ABI ShipmentEvaluatorPlugin::audio_ports_config_count(const clap_plugin_t *) {
    os_log_info(shipment_evaluator_log(), "CLAP audio_ports_config_count: 1");
    return 1;
}

bool CLAP_ABI ShipmentEvaluatorPlugin::audio_ports_config_get(
    const clap_plugin_t *, uint32_t index, clap_audio_ports_config_t *config) {
    os_log_info(shipment_evaluator_log(),
                "CLAP audio_ports_config_get: index=%u",
                index);
    if (index != 0 || !config)
        return false;

    *config = {};
    config->id = 0;
    std::strncpy(config->name, "Stereo", sizeof(config->name) - 1);
    config->input_port_count = 1;
    config->output_port_count = 1;
    config->has_main_input = true;
    config->main_input_channel_count = 2;
    config->main_input_port_type = CLAP_PORT_STEREO;
    config->has_main_output = true;
    config->main_output_channel_count = 2;
    config->main_output_port_type = CLAP_PORT_STEREO;
    return true;
}

bool CLAP_ABI ShipmentEvaluatorPlugin::audio_ports_config_select(
    const clap_plugin_t *, clap_id config_id) {
    os_log_info(shipment_evaluator_log(),
                "CLAP audio_ports_config_select: id=%u",
                config_id);
    return config_id == 0;
}

clap_id CLAP_ABI ShipmentEvaluatorPlugin::audio_ports_config_current(const clap_plugin_t *) {
    return 0;
}

bool CLAP_ABI ShipmentEvaluatorPlugin::audio_ports_config_info_get(
    const clap_plugin_t *plugin,
    clap_id config_id,
    uint32_t port_index,
    bool is_input,
    clap_audio_port_info_t *info) {
    return config_id == 0 && audio_port_get(plugin, port_index, is_input, info);
}

bool CLAP_ABI ShipmentEvaluatorPlugin::gui_is_api_supported(const clap_plugin_t *,
                                                             const char *api,
                                                             bool is_floating) {
    os_log_info(shipment_evaluator_log(),
                "CLAP GUI is_api_supported: api=%{public}s floating=%d",
                api ? api : "(null)",
                is_floating);
    return api && std::strcmp(api, CLAP_WINDOW_API_COCOA) == 0;
}

bool CLAP_ABI ShipmentEvaluatorPlugin::gui_get_preferred_api(const clap_plugin_t *,
                                                              const char **api,
                                                              bool *is_floating) {
    if (!api || !is_floating)
        return false;

    *api = CLAP_WINDOW_API_COCOA;
    *is_floating = false;
    os_log_info(shipment_evaluator_log(),
                "CLAP GUI preferred API: Cocoa embedded");
    return true;
}

bool CLAP_ABI ShipmentEvaluatorPlugin::gui_create(const clap_plugin_t *plugin,
                                                  const char *api,
                                                  bool is_floating) {
    os_log_info(shipment_evaluator_log(),
                "CLAP GUI create: api=%{public}s floating=%d",
                api ? api : "(null)",
                is_floating);
    if (!gui_is_api_supported(plugin, api, is_floating))
        return false;

    ShipmentEvaluatorPlugin *instance = from_plugin(plugin);
    return instance && instance->gui_.create(is_floating);
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
    os_log_info(shipment_evaluator_log(),
                "CLAP GUI set_parent: api=%{public}s",
                window && window->api ? window->api : "(null)");
    ShipmentEvaluatorPlugin *instance = from_plugin(plugin);
    if (!instance || !window || !window->api ||
        std::strcmp(window->api, CLAP_WINDOW_API_COCOA) != 0)
        return false;

    return instance->gui_.set_parent(window->cocoa);
}

bool CLAP_ABI ShipmentEvaluatorPlugin::gui_set_transient(const clap_plugin_t *plugin,
                                                          const clap_window_t *window) {
    os_log_info(shipment_evaluator_log(),
                "CLAP GUI set_transient: api=%{public}s",
                window && window->api ? window->api : "(null)");
    if (!window || !window->api ||
        std::strcmp(window->api, CLAP_WINDOW_API_COCOA) != 0)
        return false;

    ShipmentEvaluatorPlugin *instance = from_plugin(plugin);
    return instance && instance->gui_.set_transient(window->cocoa);
}

void CLAP_ABI ShipmentEvaluatorPlugin::gui_suggest_title(const clap_plugin_t *plugin,
                                                         const char *title) {
    os_log_info(shipment_evaluator_log(),
                "CLAP GUI suggest_title: %{public}s",
                title ? title : "(null)");
    if (ShipmentEvaluatorPlugin *instance = from_plugin(plugin))
        instance->gui_.suggest_title(title);
}

bool CLAP_ABI ShipmentEvaluatorPlugin::gui_show(const clap_plugin_t *plugin) {
    os_log_info(shipment_evaluator_log(), "CLAP GUI show");
    ShipmentEvaluatorPlugin *instance = from_plugin(plugin);
    return instance && instance->gui_.show();
}

bool CLAP_ABI ShipmentEvaluatorPlugin::gui_hide(const clap_plugin_t *plugin) {
    os_log_info(shipment_evaluator_log(), "CLAP GUI hide");
    ShipmentEvaluatorPlugin *instance = from_plugin(plugin);
    return instance && instance->gui_.hide();
}
