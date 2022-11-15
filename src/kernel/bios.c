#include "bios.h"
#include "memory.h"
#include "serial.h"
#include "stdlib.h"
#include "task.h"

BIOSDataArea const * const bios_data_area = MEMORY_MAP_BIOS_DATA_AREA;
BIOSInterface *(* const _call_bios)(unsigned char interrupt_number, BIOSInterface *arguments) = (BIOSInterface *(* const)(unsigned char, BIOSInterface *))MEMORY_MAP_CALL_BIOS;

BIOSInterface call_bios(unsigned char interrupt_number, BIOSInterface arguments)
{
	BIOSInterface result;
	switch_polling_serial_mode();
	result = *_call_bios(interrupt_number, &arguments);
	switch_interrupt_serial_mode();
	return result;
}

BIOSDataArea const *get_bios_data_area(void)
{
	return bios_data_area;
}

void const *get_extended_bios_data_area(void)
{
	return (void const *)((unsigned int)get_bios_data_area()->ebda_base_address << 4);
}

void print_bios_data_area(BIOSDataArea const *bios_data_area)
{
	for(unsigned int i = 0; i < _countof(bios_data_area->com_port_address); i++)printf_serial("bios_data_area->com_port_address[%d] = %#06.4x\n", i, bios_data_area->com_port_address[i]);
	for(unsigned int i = 0; i < _countof(bios_data_area->lpt_port_address); i++)printf_serial("bios_data_area->lpt_port_address[%d] = %#06.4x\n", i, bios_data_area->lpt_port_address[i]);
	printf_serial("bios_data_area->ebda_base_address = %#06.4x\n", bios_data_area->ebda_base_address);
	printf_serial("bios_data_area->equipment_list_flags = %#06.4x\n", bios_data_area->equipment_list_flags);
	printf_serial("bios_data_area->pcjr = %#04.2x\n", bios_data_area->pcjr);
	printf_serial("bios_data_area->memory_size_before_ebda_kilobytes = %#06.4x\n", bios_data_area->memory_size_before_ebda_kilobytes);
	printf_serial("bios_data_area->ps2_bios_control_flags = %#04.2x\n", bios_data_area->ps2_bios_control_flags);
	printf_serial("bios_data_area->keyboard_flags = %#06.4x\n", bios_data_area->keyboard_flags);
	printf_serial("bios_data_area->storage_for_alternate_keypad_entry = %#04.2x\n", bios_data_area->storage_for_alternate_keypad_entry);
	printf_serial("bios_data_area->keyboard_buffer_head = %#06.4x\n", bios_data_area->keyboard_buffer_head);
	printf_serial("bios_data_area->keyboard_buffer_tail = %#06.4x\n", bios_data_area->keyboard_buffer_tail);
	for(unsigned int i = 0; i < _countof(bios_data_area->keyboard_buffer); i++)printf_serial("bios_data_area->keyboard_buffer[%#04.2x] = %#04.2x\n", i, bios_data_area->keyboard_buffer[i]);
	printf_serial("bios_data_area->drive_recalibration_status = %#04.2x\n", bios_data_area->drive_recalibration_status);
	printf_serial("bios_data_area->diskette_motor_status = %#04.2x\n", bios_data_area->diskette_motor_status);
	printf_serial("bios_data_area->motor_shutoff_counter = %#04.2x\n", bios_data_area->motor_shutoff_counter);
	printf_serial("bios_data_area->status_of_last_diskette_operation = %#04.2x\n", bios_data_area->status_of_last_diskette_operation);
	for(unsigned int i = 0; i < _countof(bios_data_area->nec_diskette_comtroller_status); i++)printf_serial("bios_data_area->nec_diskette_comtroller_status[%#04.2x] = %#04.2x\n", i, bios_data_area->nec_diskette_comtroller_status[i]);
	printf_serial("bios_data_area->current_video_mode = %#04.2x\n", bios_data_area->current_video_mode);
	printf_serial("bios_data_area->number_of_screen_columns = %#06.4x\n", bios_data_area->number_of_screen_columns);
	printf_serial("bios_data_area->size_of_current_video_regen_buffer = %#06.4x\n", bios_data_area->size_of_current_video_regen_buffer);
	printf_serial("bios_data_area->offset_of_current_vide_page = %#06.4x\n", bios_data_area->offset_of_current_vide_page);
	for(unsigned int i = 0; i < _countof(bios_data_area->cursor_position); i++)printf_serial("bios_data_area->cursor_position[%#04.2x] = %#06.4x\n", i, bios_data_area->cursor_position[i]);
	printf_serial("bios_data_area->cursor_ending_scan_line = %#04.2x\n", bios_data_area->cursor_ending_scan_line);
	printf_serial("bios_data_area->cursor_starting_scan_line = %#04.2x\n", bios_data_area->cursor_starting_scan_line);
	printf_serial("bios_data_area->active_display_page_number = %#04.2x\n", bios_data_area->active_display_page_number);
	printf_serial("bios_data_area->base_port_address_for_active_crt_controller = %#06.4x\n", bios_data_area->base_port_address_for_active_crt_controller);
	printf_serial("bios_data_area->crt_mode_control_register_value = %#04.2x\n", bios_data_area->crt_mode_control_register_value);
	printf_serial("bios_data_area->cga_current_color_palette_mask_setting = %#04.2x\n", bios_data_area->cga_current_color_palette_mask_setting);
	printf_serial("bios_data_area->day_counter_0 = %#010.8x\n", bios_data_area->day_counter_0);
	printf_serial("bios_data_area->reserved_1 = %#04.2x\n", bios_data_area->reserved_1);
	printf_serial("bios_data_area->daily_timer_control = %#010.8x\n", bios_data_area->daily_timer_control);
	printf_serial("bios_data_area->clocl_rollover_flag = %#04.2x\n", bios_data_area->clocl_rollover_flag);
	printf_serial("bios_data_area->bios_break_flag = %#04.2x\n", bios_data_area->bios_break_flag);
	printf_serial("bios_data_area->soft_reset_flag = %#06.4x\n", bios_data_area->soft_reset_flag);
	printf_serial("bios_data_area->status_of_last_hard_disk_operation = %#04.2x\n", bios_data_area->status_of_last_hard_disk_operation);
	printf_serial("bios_data_area->number_of_hard_disks_attached = %#04.2x\n", bios_data_area->number_of_hard_disks_attached);
	printf_serial("bios_data_area->xt_fixed_disk_drive_control_byte = %#04.2x\n", bios_data_area->xt_fixed_disk_drive_control_byte);
	printf_serial("bios_data_area->port_offset_to_current_fixed_disk_adapter = %#04.2x\n", bios_data_area->port_offset_to_current_fixed_disk_adapter);
	for(unsigned int i = 0; i < _countof(bios_data_area->time_out_value_for_lpt); i++)printf_serial("bios_data_area->time_out_value_for_lpt[%#04.2x] = %#04.2x\n", i, bios_data_area->time_out_value_for_lpt[i]);
	for(unsigned int i = 0; i < _countof(bios_data_area->time_out_value_for_com); i++)printf_serial("bios_data_area->time_out_value_for_com[%#04.2x] = %#04.2x\n", i, bios_data_area->time_out_value_for_com[i]);
	printf_serial("bios_data_area->keyboard_buffer_start_offset = %#06.4x\n", bios_data_area->keyboard_buffer_start_offset);
	printf_serial("bios_data_area->keyboard_buffer_end_offset = %#06.4x\n", bios_data_area->keyboard_buffer_end_offset);
	printf_serial("bios_data_area->rows_on_the_screen = %#04.2x\n", bios_data_area->rows_on_the_screen);
	printf_serial("bios_data_area->point_height_of_character_matrix = %#06.4x\n", bios_data_area->point_height_of_character_matrix);
	printf_serial("bios_data_area->video_mode_options = %#04.2x\n", bios_data_area->video_mode_options);
	printf_serial("bios_data_area->ega_feature_bit_switches = %#04.2x\n", bios_data_area->ega_feature_bit_switches);
	printf_serial("bios_data_area->video_display_data_area = %#04.2x\n", bios_data_area->video_display_data_area);
	printf_serial("bios_data_area->display_combination_code_table_index = %#04.2x\n", bios_data_area->display_combination_code_table_index);
	printf_serial("bios_data_area->last_diskette_data_rate_selected = %#04.2x\n", bios_data_area->last_diskette_data_rate_selected);
	printf_serial("bios_data_area->hard_disk_status_returned_by_controller = %#04.2x\n", bios_data_area->hard_disk_status_returned_by_controller);
	printf_serial("bios_data_area->hard_disk_error_returned_by_controller = %#04.2x\n", bios_data_area->hard_disk_error_returned_by_controller);
	printf_serial("bios_data_area->hard_disk_interrupt_control_flag = %#04.2x\n", bios_data_area->hard_disk_interrupt_control_flag);
	printf_serial("bios_data_area->combination_hard_floppy_disk_card = %#04.2x\n", bios_data_area->combination_hard_floppy_disk_card);
	for(unsigned int i = 0; i < _countof(bios_data_area->drive_media_state); i++)printf_serial("bios_data_area->drive_media_state[%#04.2x] = %#04.2x\n", i, bios_data_area->drive_media_state[i]);
	for(unsigned int i = 0; i < _countof(bios_data_area->track_currently_seeked_to_on_drive); i++)printf_serial("bios_data_area->track_currently_seeked_to_on_drive[%#04.2x] = %#04.2x\n", i, bios_data_area->track_currently_seeked_to_on_drive[i]);
	printf_serial("bios_data_area->keyboard_mode_type = %#04.2x\n", bios_data_area->keyboard_mode_type);
	printf_serial("bios_data_area->keyboard_led_flags = %#04.2x\n", bios_data_area->keyboard_led_flags);
	printf_serial("bios_data_area->pointer_to_user_wait_complete_flag = %#010.8x\n", bios_data_area->pointer_to_user_wait_complete_flag);
	printf_serial("bios_data_area->user_wait_time_out_value = %#010.8x\n", bios_data_area->user_wait_time_out_value);
	printf_serial("bios_data_area->rtc_wait_function_flag = %#04.2x\n", bios_data_area->rtc_wait_function_flag);
	printf_serial("bios_data_area->lana_dma_channed_flags = %#04.2x\n", bios_data_area->lana_dma_channed_flags);
	for(unsigned int i = 0; i < _countof(bios_data_area->status_of_lana); i++)printf_serial("bios_data_area->status_of_lana[%#04.2x] = %#04.2x\n", i, bios_data_area->status_of_lana[i]);
	printf_serial("bios_data_area->saved_hard_disk_interrupt_vector = %#010.8x\n", bios_data_area->saved_hard_disk_interrupt_vector);
	printf_serial("bios_data_area->bios_video_save_override_pointer_table = %p\n", bios_data_area->bios_video_save_override_pointer_table);
	printf_serial("bios_data_area->keyboard_nmi_control_flags = %#04.2x\n", bios_data_area->keyboard_nmi_control_flags);
	printf_serial("bios_data_area->keyboard_break_pending_flags = %#010.8x\n", bios_data_area->keyboard_break_pending_flags);
	printf_serial("bios_data_area->port_60_single_byte_queue = %#04.2x\n", bios_data_area->port_60_single_byte_queue);
	printf_serial("bios_data_area->scan_code_of_last_key = %#04.2x\n", bios_data_area->scan_code_of_last_key);
	printf_serial("bios_data_area->nmi_buffer_head_pointer = %#04.2x\n", bios_data_area->nmi_buffer_head_pointer);
	printf_serial("bios_data_area->nmi_buffer_tail_pointer = %#04.2x\n", bios_data_area->nmi_buffer_tail_pointer);
	for(unsigned int i = 0; i < _countof(bios_data_area->nmi_scan_code_buffer); i++)printf_serial("bios_data_area->nmi_scan_code_buffer[%#04.2x] = %#04.2x\n", i, bios_data_area->nmi_scan_code_buffer[i]);
	printf_serial("bios_data_area->day_counter_1 = %#06.4x\n", bios_data_area->day_counter_1);
	for(unsigned int i = 0; i < _countof(bios_data_area->intra_applications_communications_area); i++)printf_serial("bios_data_area->intra_applications_communications_area[%#04.2x] = %#04.2x\n", i, bios_data_area->intra_applications_communications_area[i]);
	printf_serial("bios_data_area->print_screen_status_byte = %#04.2x\n", bios_data_area->print_screen_status_byte);
	for(unsigned int i = 0; i < _countof(bios_data_area->used_by_basic); i++)printf_serial("bios_data_area->used_by_basic[%#04.2x] = %#04.2x\n", i, bios_data_area->used_by_basic[i]);
	printf_serial("bios_data_area->dos_single_diskette_mode = %#04.2x\n", bios_data_area->dos_single_diskette_mode);
	for(unsigned int i = 0; i < _countof(bios_data_area->post_word_area); i++)printf_serial("bios_data_area->post_word_area[%#04.2x] = %#04.2x\n", i, bios_data_area->post_word_area[i]);
	printf_serial("bios_data_area->basic_shell_flag = %#04.2x\n", bios_data_area->basic_shell_flag);
	printf_serial("bios_data_area->basics_default_ds_value = %#06.4x\n", bios_data_area->basics_default_ds_value);
	printf_serial("bios_data_area->basic_int_1c_interrupt_handler = %p\n", bios_data_area->basic_int_1c_interrupt_handler);
	printf_serial("bios_data_area->basic_int_23_interrupt_handler = %p\n", bios_data_area->basic_int_23_interrupt_handler);
	printf_serial("bios_data_area->basic_int_24_interrupt_handler = %p\n", bios_data_area->basic_int_24_interrupt_handler);
	printf_serial("bios_data_area->dos_dynamic_storage = %#06.4x\n", bios_data_area->dos_dynamic_storage);
	for(unsigned int i = 0; i < _countof(bios_data_area->dos_diskette_initialization_table); i++)printf_serial("bios_data_area->dos_diskette_initialization_table[%#04.2x] = %#04.2x\n", i, bios_data_area->dos_diskette_initialization_table[i]);
	printf_serial("bios_data_area->mode_command = %#010.8x\n", bios_data_area->mode_command);
}

