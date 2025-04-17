#include "options.h"
#include <cstdint>

// Added versioning and packed structure for serialization
#pragma pack(push, 1)  // Ensure no padding between fields
struct SavedOptions {
    uint32_t version = 1;  // Version identifier
    uint8_t rpc_enabled;
    uint8_t filename_enabled;
    uint8_t functionname_enabled;
    uint8_t address_enabled;
    int32_t address_type;
    uint8_t timeelapsed_enabled;
    uint8_t output_enabled;
    int32_t output_type;
};
#pragma pack(pop)

void Options::save(const char* config_name) {
    char save_path[QMAXPATH];
    qsnprintf(save_path, sizeof(save_path), "%s/%s", get_user_idadir(), config_name);

    FILE* file = qfopen(save_path, "wb");
    if (!file) {
        if (output_enabled) msg("[%s] Failed to create config file\n", plugin_name);
        return;
    }

    // Convert to storage format
    SavedOptions save;
    save.version = 1;
    save.rpc_enabled = static_cast<uint8_t>(rpc_enabled);
    save.filename_enabled = static_cast<uint8_t>(filename_enabled);
    save.functionname_enabled = static_cast<uint8_t>(functionname_enabled);
    save.address_enabled = static_cast<uint8_t>(address_enabled);
    save.address_type = address_type;
    save.timeelapsed_enabled = static_cast<uint8_t>(timeelapsed_enabled);
    save.output_enabled = static_cast<uint8_t>(output_enabled);
    save.output_type = output_type;

    if (qfwrite(file, &save, sizeof(SavedOptions)) != sizeof(SavedOptions)) {
        if (output_enabled) msg("[%s] Failed to write config\n", plugin_name);
    }
    qfclose(file);
}

void Options::load(const char* config_name) {
    char load_path[QMAXPATH];
    qsnprintf(load_path, sizeof(load_path), "%s/%s", get_user_idadir(), config_name);

    FILE* file = qfopen(load_path, "rb");
    if (!file) {
        if (output_enabled) msg("[%s] Config file not found\n", plugin_name);
        return;
    }

    SavedOptions load;
    if (qfread(file, &load, sizeof(SavedOptions)) != sizeof(SavedOptions)) {
        qfclose(file);
        if (output_enabled) msg("[%s] Corrupted config file\n", plugin_name);
        return;
    }
    qfclose(file);

    // Handle version mismatch
    if (load.version != 1) {
        if (output_enabled) msg("[%s] Config version mismatch\n", plugin_name);
        return;
    }

    // Convert from storage format
    rpc_enabled = load.rpc_enabled != 0;
    filename_enabled = load.filename_enabled != 0;
    functionname_enabled = load.functionname_enabled != 0;
    address_enabled = load.address_enabled != 0;
    address_type = load.address_type;
    timeelapsed_enabled = load.timeelapsed_enabled != 0;
    output_enabled = load.output_enabled != 0;
    output_type = load.output_type;
}

Options g_options;