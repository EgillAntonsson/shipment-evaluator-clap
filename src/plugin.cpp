#include "MyPlugin.h"
#include <clap/clap.h>

// Static storage for plugin instance - necessary for C ABI compatibility
static MyPlugin* g_plugin = nullptr;

extern "C" {
    const clap_plugin_t *clap_plugin_create(const clap_host_t *host) {
        g_plugin = new MyPlugin(host);
        return g_plugin->get_clap_interface();
    }
    
    // Plugin entry point
    const clap_plugin_entry_t clap_entry = {
        .clap_version = CLAP_VERSION,
        .create = clap_plugin_create,
    };
}