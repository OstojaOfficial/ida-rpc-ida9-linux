#pragma once

#define STRINGIFY_IDA_INPUT_FIELD( field_identifier_str, field_id ) field_identifier_str #field_id
#define DROPDOWN_FIELD( field_id ) STRINGIFY_IDA_INPUT_FIELD( "b", field_id )

namespace ida_utils
{
	bool is_idb_loaded( ) {
		return ( strlen( get_path( PATH_TYPE_IDB ) ) != 0 );
	}

	const char* get_current_filename() {
		static char filename[MAXSTR] = {0};
		ssize_t read_size = get_root_filename(filename, sizeof(filename));
		
		if (read_size > 0) {
			filename[sizeof(filename)-1] = '\0';
			return filename;
		}
		return "(no file)";
	}

	const char* get_current_processor_module() {
		static char processor_module[MAXSTR] = {0};
		if (!get_idp_name(processor_module, sizeof(processor_module))) {
			qstrncpy(processor_module, "(unknown)", sizeof(processor_module));
		}
		processor_module[sizeof(processor_module)-1] = '\0';
		return processor_module;
	}

	ea_t get_current_cursor_address() {
		ea_t cur_addr = get_screen_ea();
		if (cur_addr != BADADDR && is_code(get_flags(cur_addr))) {
			return cur_addr;
		}
		return BADADDR;
	}

	ea_t get_current_function_start_address() {
		ea_t cur_addr = get_current_cursor_address();
		if (cur_addr == BADADDR) return BADADDR;
		
		func_t* func = get_func(cur_addr);
		return func ? func->start_ea : BADADDR;
	}

	const char* get_current_function_name() {
		static char buffer[MAXSTR] = {0};
		buffer[0] = '\0';
	
		ea_t cur_addr = get_current_cursor_address();
		if (cur_addr == BADADDR) return "(invalid address)";
	
		func_t* func = get_func(cur_addr);
		if (!func) return "(no function)";
	
		qstring func_name;
		if (!get_func_name(&func_name, func->start_ea)) {
			return "(name retrieval failed)";
		}
	
		qstrncpy(buffer, func_name.c_str(), sizeof(buffer));
		buffer[sizeof(buffer)-1] = '\0';
		return buffer;
	}
}
