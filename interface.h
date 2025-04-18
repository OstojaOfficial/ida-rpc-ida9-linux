#pragma once
#include "utils/lib-utils/ida-utils.h"

#ifdef _Release
	#include "utils/build-version/ver.h"
#endif
#ifdef _Release64
	#include "utils/build-version/ver64.h"
#endif

enum class combobox_ids : int {
	ID_ADDRESS_TYPE,
	ID_OUTPUT_TYPE
}; 

enum checkbox_bitmasks {
	rpc			  = ( 1 << 0 ),
	filename	  = ( 1 << 1 ),
	function_name = ( 1 << 2 ),
	address		  = ( 1 << 3 ),
	time_elapsed  = ( 1 << 4 ),
	output		  = ( 1 << 5 )
};

void show_options() {
	Options old_config = g_options;

	qstrvec_t sz_address_types, sz_output_types;
    sz_address_types.push_back("Function base");
	sz_address_types.push_back("Current cursor  ");
	sz_output_types.push_back ("Errors only");
	sz_output_types.push_back ("Errors and results");
	sz_output_types.push_back("Errors, results and interim steps  ");

	ushort checkbox_flags = 0;
	
	if ( g_options.rpc_enabled          ) checkbox_flags |= checkbox_bitmasks::rpc;
	if ( g_options.filename_enabled	    ) checkbox_flags |= checkbox_bitmasks::filename;
	if ( g_options.functionname_enabled ) checkbox_flags |= checkbox_bitmasks::function_name;
	if ( g_options.address_enabled      ) checkbox_flags |= checkbox_bitmasks::address;
	if ( g_options.timeelapsed_enabled  ) checkbox_flags |= checkbox_bitmasks::time_elapsed;
	if ( g_options.output_enabled       ) checkbox_flags |= checkbox_bitmasks::output;

	int result = ask_form(
        "IDA RPC Options\n"

		"<#Address to be displayed beside function name#Address type:" DROPDOWN_FIELD( ( int )combobox_ids::ID_ADDRESS_TYPE ) ":0::::>\n"
		"<#Type of info printed to output window#Output type :" DROPDOWN_FIELD( ( int )combobox_ids::ID_OUTPUT_TYPE ) ":0::::>\n"

		"<#Enable rich presence#RPC enabled:C>\n"            
		"<#Displays current filename#Filename enabled:C>\n"
		"<#Displays current function name#Function name enabled:C>\n"
		"<#Displays address ( of / within ) current function#Address enabled:C>\n"
		"<#Displays the time since IDA was opened#Time elapsed enabled:C>\n"
		"<#Prints various information to output window for debugging#Output enabled ( debug ):C>>\n"

		"IDA RPC version: " AUTO_VERSION_RELEASE_STR " by shigureJ\n\n",
        &sz_address_types,
		&g_options.address_type,

		&sz_output_types,
		&g_options.output_type,

        &checkbox_flags
    );

	g_options.rpc_enabled		   = ( ( checkbox_flags & checkbox_bitmasks::rpc           ) != 0 );
	g_options.filename_enabled	   = ( ( checkbox_flags & checkbox_bitmasks::filename      ) != 0 );
	g_options.functionname_enabled = ( ( checkbox_flags & checkbox_bitmasks::function_name ) != 0 );
	g_options.address_enabled	   = ( ( checkbox_flags & checkbox_bitmasks::address       ) != 0 );
	g_options.timeelapsed_enabled  = ( ( checkbox_flags & checkbox_bitmasks::time_elapsed  ) != 0 );
	g_options.output_enabled       = ( ( checkbox_flags & checkbox_bitmasks::output        ) != 0 );


	if (result == 0 || result == -1) { // User canceled
		g_options = old_config;
		
		// Respect output settings for cancellation message
		if (g_options.output_type >= static_cast<int>(output_type::errors_results_and_interim_steps) && g_options.output_enabled) {
			msg("[%s] Options cancelled/escaped - restoring old config\n", plugin_name);
		}
	} 
	else if (result == 1) { // User confirmed
		// Respect output settings for confirmation message
		if (g_options.output_type >= static_cast<int>(output_type::errors_results_and_interim_steps) && g_options.output_enabled) {
			msg("[%s] Options confirmed - applying and saving\n", plugin_name);
		}
		
		g_options.save();
	}
}