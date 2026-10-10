#include "ShipmentEvaluatorPlugin.h"
#include <clap/clap.h>
#include <cstring>
#include <os/log.h>

static os_log_t shipment_factory_log() {
    static os_log_t logger = os_log_create("rocks.egill.shipment-evaluator", "factory");
    return logger;
}

const clap_plugin_descriptor_t *shipment_evaluator_descriptor();

static bool CLAP_ABI clap_entry_init(const char *plugin_path) {
    os_log_info(shipment_factory_log(),
                "CLAP entry init: path=%{public}s",
                plugin_path ? plugin_path : "(null)");
    return true;
}

static void CLAP_ABI clap_entry_deinit() {
    os_log_info(shipment_factory_log(), "CLAP entry deinit");
}

static uint32_t CLAP_ABI get_plugin_count(const clap_plugin_factory_t *) {
    return 1;
}

static const clap_plugin_descriptor_t *CLAP_ABI get_plugin_descriptor(
    const clap_plugin_factory_t *, uint32_t index) {
    return index == 0 ? shipment_evaluator_descriptor() : nullptr;
}

static const clap_plugin_t *CLAP_ABI create_plugin(
    const clap_plugin_factory_t *, const clap_host_t *host, const char *plugin_id) {
    os_log_info(shipment_factory_log(),
                "CLAP create_plugin called with id=%{public}s",
                plugin_id ? plugin_id : "(null)");
    const clap_plugin_descriptor_t *descriptor = shipment_evaluator_descriptor();
    if (!plugin_id || std::strcmp(plugin_id, descriptor->id) != 0) {
        os_log_error(shipment_factory_log(),
                     "CLAP create_plugin rejected id=%{public}s expected=%{public}s",
                     plugin_id ? plugin_id : "(null)",
                     descriptor->id);
        return nullptr;
    }

    return (new ShipmentEvaluatorPlugin(host))->get_clap_interface();
}

static const clap_plugin_factory_t plugin_factory = {
    .get_plugin_count = get_plugin_count,
    .get_plugin_descriptor = get_plugin_descriptor,
    .create_plugin = create_plugin,
};

static const void *CLAP_ABI get_factory(const char *factory_id) {
    return factory_id && std::strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) == 0
        ? &plugin_factory
        : nullptr;
}

extern "C" {
const clap_plugin_entry_t clap_entry = {
    .clap_version = kShipmentEvaluatorClapVersion,
    .init = clap_entry_init,
    .deinit = clap_entry_deinit,
    .get_factory = get_factory,
};
}