#include "includes.h"
#include "options.h"
#include "interface.h"

#include "utils/lib-utils/ida-utils.h"
#include "utils/lib-utils/discord-utils.h"

#ifdef _Release
#include "utils/build-version/ver.h"
#endif
#ifdef _Release64
#include "utils/build-version/ver64.h"
#endif

int64_t     start_time;
const char* app_id = "1216680096501469254";

const char* IDAP_comment = "IDA plugin by shigureJ";
const char* IDAP_help    = "IDA plugin by shigureJ";
const char* IDAP_name    = "IDA RPC";
const char* IDAP_hotkey  = "Ctrl-Alt-R";

#pragma region callbacks

namespace callbacks {
    // IDP callback - Processor events
    int idaapi idp_callback(void*, int code, va_list) {
        switch (code) {
        case processor_t::ev_newfile:
        case processor_t::ev_oldfile:
        case processor_t::ev_newbinary:
            discord_utils::update_discord_presence(start_time);
            break;
        }
        return 0;
    }

    // IDB callback - Database events
    int idaapi idb_callback(void*, int code, va_list) {
        switch (code) {
        case idb_event::savebase:
        case idb_event::func_added:
        case idb_event::renamed:
            discord_utils::update_discord_presence(start_time);
            break;
        }
        return 0;
    }

    // UI callback - Interface events
    int idaapi ui_callback(void*, int code, va_list) {
        switch (code) {
        case ui_load_file:
        case ui_get_curline:
            discord_utils::update_discord_presence(start_time);
            break;
        }
        return 0;
    }

    // View callback - Navigation events
    int idaapi view_callback(void*, int code, va_list) {
        switch (code) {
        case view_loc_changed:
        case view_switched:
            discord_utils::update_discord_presence(start_time);
            break;
        }
        return 0;
    }

}

// Unified hook management
static bool hook_notification_point(
    hook_type_t hook_type,
    hook_cb_t* callback,
    const char* hook_name
) {
    const bool success = hook_to_notification_point(hook_type, callback);
    
        const char* status = success ? "successful" : "failed";

        if ( g_options.output_enabled ) {
            msg("[%s] Hook %s %s\n", IDAP_name, hook_name, status);
        }
    
    return success;
}

static plugmod_t* idaapi hook_callbacks() {
    bool all_hooks_successful = true;
    
    all_hooks_successful &= hook_notification_point(
        HT_IDP,
        (hook_cb_t*)(callbacks::idp_callback),
        "HT_IDP"
    );
    
    all_hooks_successful &= hook_notification_point(
        HT_IDB,
        (hook_cb_t*)(callbacks::idb_callback),
        "HT_IDB"
    );
    
    all_hooks_successful &= hook_notification_point(
        HT_UI,
        (hook_cb_t*)(callbacks::ui_callback),
        "HT_UI"
    );
    
    all_hooks_successful &= hook_notification_point(
        HT_VIEW,
        (hook_cb_t*)(callbacks::view_callback),
        "HT_VIEW"
    );

    return all_hooks_successful ? PLUGIN_KEEP : PLUGIN_SKIP;
}

static void idaapi unhook_callbacks() {
    unhook_from_notification_point(HT_IDP, ( hook_cb_t* ) callbacks::idp_callback);
    unhook_from_notification_point(HT_IDB, ( hook_cb_t* ) callbacks::idb_callback);
    unhook_from_notification_point(HT_UI, ( hook_cb_t* ) callbacks::ui_callback);
    unhook_from_notification_point(HT_VIEW, ( hook_cb_t* ) callbacks::view_callback);

    if ( g_options.output_enabled ) {
        msg("[%s] All callbacks unregistered\n", IDAP_name);
    }
}

#pragma endregion

static plugmod_t* idaapi IDAP_init( void )
{
    addon_info_t addon;
    addon.id       = "shigureJ.IDA.RPC";
    addon.name     = "IDA RPC";
    addon.producer = "shigureJ";
    addon.url      = "https://www.github.com/apfelteesaft";
    addon.version  = AUTO_VERSION_STR;
    addon.freeform = "Copyright (c) 2018 shigureJ <https://keybase.io/shigurej>\n"
                     "All rights reserved.\n";

    register_addon( &addon );

    g_options.load( );  // loaded before callbacks so they are able to output

    msg( "[%s] v%.2f LOADED\n", IDAP_name, AUTO_VERSION_RELEASE  );

    start_time = time( 0 );

    if ( g_options.rpc_enabled ) {

        if ( hook_callbacks( ) == PLUGIN_SKIP ) {

            if ( g_options.output_type >= ( int ) output_type::errors_results_and_interim_steps && g_options.output_enabled ) {
                msg( "[%s] %s -> one or more callback hooks failed plugin loading will be skipped\n", IDAP_name, __FUNCTION__ );
            }

            return PLUGIN_SKIP;
        }
    } else if ( !ida_utils::is_idb_loaded( ) && g_options.rpc_enabled ) {

        if ( g_options.output_type >= ( int ) output_type::errors_results_and_interim_steps && g_options.output_enabled ) {
            msg( "[%s] %s -> no idb currently loaded setting state to idle\n", IDAP_name, __FUNCTION__ );
        }

        return PLUGIN_SKIP;
    }

    discord_utils::discord_init( app_id );

    return PLUGIN_KEEP;
}

static void idaapi IDAP_term( void )
{

    unhook_callbacks( );
    Discord_ClearPresence( );
    Discord_Shutdown( );
    return;
}

static bool idaapi IDAP_run( size_t arg )
{

    show_options( );

    if ( g_options.rpc_enabled ) {

        if ( ida_utils::is_idb_loaded( ) && g_options.output_type >= ( int ) output_type::errors_and_results && g_options.output_enabled ) {
            msg( "[%s] Version: %.2f\n", IDAP_name, AUTO_VERSION_RELEASE );
            msg( "[%s] Built for IDA: %i\n", IDAP_name, IDP_INTERFACE_VERSION );
            msg( "[%s] Processor module: %s \n", IDAP_name, ida_utils::get_current_processor_module( ) );
            msg( "[%s] Currently open file: %s\n", IDAP_name, ida_utils::get_current_filename( ) );
            msg( "[%s] Current function: %s - 0x%a\n", IDAP_name, ida_utils::get_current_function_name( ), ida_utils::get_current_function_start_address( ) );
            msg( "[%s] Current selected address: 0x%a\n", IDAP_name, ida_utils::get_current_cursor_address( ) );
        }

        discord_utils::update_discord_presence( start_time );
    }

    return true;
}

plugin_t PLUGIN = {
    IDP_INTERFACE_VERSION,  // IDA version plugin was written for
    PLUGIN_FIX,             // Plugin flags
    IDAP_init,              // Init func
    IDAP_term,              // Clean up func
    IDAP_run,               // Main plugin body
    IDAP_comment,           // Comment for status line or for hints
    IDAP_help,              // Multiline help for the plugin
    IDAP_name,              // Plugin name shown in edit -> plugins menu
    IDAP_hotkey             // Hotkey to run the plugin
};