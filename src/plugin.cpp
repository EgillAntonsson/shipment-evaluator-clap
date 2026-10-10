#include "ShipmentEvaluatorPlugin.h"
#include <clap/clap.h>
#include <cstring>

const clap_plugin_descriptor_t *shipment_evaluator_descriptor();

static bool CLAP_ABI clap_entry_init(const char *) {
    return true;
}

static void CLAP_ABI clap_entry_deinit() {}

static uint32_t CLAP_ABI get_plugin_count(const clap_plugin_factory_t *) {
    return 1;
}

static const clap_plugin_descriptor_t *CLAP_ABI get_plugin_descriptor(
    const clap_plugin_factory_t *, uint32_t index) {
    return index == 0 ? shipment_evaluator_descriptor() : nullptr;
}

static const clap_plugin_t *CLAP_ABI create_plugin(
    const clap_plugin_factory_t *, const clap_host_t *host, const char *plugin_id) {
    const clap_plugin_descriptor_t *descriptor = shipment_evaluator_descriptor();
    if (!plugin_id || std::strcmp(plugin_id, descriptor->id) != 0)
        return nullptr;

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
    .clap_version = CLAP_VERSION,
    .init = clap_entry_init,
    .deinit = clap_entry_deinit,
    .get_factory = get_factory,
};
}