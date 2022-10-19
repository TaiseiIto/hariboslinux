#ifndef _BIOS_H_
#define _BIOS_H_

// https://stanislavs.org/helppc/bios_data_area.html
typedef struct _BIOSDataArea
{
	unsigned short com_port_address[0x04];
	unsigned short lpt_port_address[0x03];
	unsigned short ebda_base_address;
	unsigned short equipment_list_flags;
	unsigned char pcjr;
	unsigned short memory_size_before_ebda_kilobytes;
	unsigned char reserved_0;
	unsigned char ps2_bios_control_flags;
	unsigned short keyboard_flags;
	unsigned char storage_for_alternate_keypad_entry;
	unsigned short keyboard_buffer_head;
	unsigned short keyboard_buffer_tail;
	unsigned char keyboard_buffer[0x20];
	unsigned char drive_recalibration_status;
	unsigned char diskette_motor_status;
	unsigned char motor_shutoff_counter;
	unsigned char status_of_last_diskette_operation;
	unsigned char nec_diskette_comtroller_status[7];
	unsigned char current_video_mode;
	unsigned short number_of_screen_columns;
	unsigned short size_of_current_video_regen_buffer;
	unsigned short offset_of_current_vide_page;
	unsigned short cursor_position[8];
	unsigned char cursor_ending_scan_line;
	unsigned char cursor_starting_scan_line;
	unsigned char active_display_page_number;
	unsigned short base_port_address_for_active_crt_controller;
	unsigned char crt_mode_control_register_value;
	unsigned char cga_current_color_palette_mask_setting;
	unsigned int day_counter_0;
	unsigned char reserved_1;
	unsigned int daily_timer_control;
	unsigned char clocl_rollover_flag;
	unsigned char bios_break_flag;
	unsigned short soft_reset_flag;
	unsigned char status_of_last_hard_disk_operation;
	unsigned char number_of_hard_disks_attached;
	unsigned char xt_fixed_disk_drive_control_byte;
	unsigned char port_offset_to_current_fixed_disk_adapter;
	unsigned char time_out_value_for_lpt[4];
	unsigned char time_out_value_for_com[4];
	unsigned short keyboard_buffer_start_offset;
	unsigned short keyboard_buffer_end_offset;
	unsigned char rows_on_the_screen;
	unsigned short point_height_of_character_matrix;
	unsigned char video_mode_options;
	unsigned char ega_feature_bit_switches;
	unsigned char video_display_data_area;
	unsigned char display_combination_code_table_index;
	unsigned char last_diskette_data_rate_selected;
	unsigned char hard_disk_status_returned_by_controller;
	unsigned char hard_disk_error_returned_by_controller;
	unsigned char hard_disk_interrupt_control_flag;
	unsigned char combination_hard_floppy_disk_card;
	unsigned char drive_media_state[4];
	unsigned char track_currently_seeked_to_on_drive[2];
	unsigned char keyboard_mode_type;
	unsigned char keyboard_led_flags;
	unsigned int pointer_to_user_wait_complete_flag;
	unsigned int user_wait_time_out_value;
	unsigned char rtc_wait_function_flag;
	unsigned char lana_dma_channed_flags;
	unsigned char status_of_lana[2];
	unsigned int saved_hard_disk_interrupt_vector;
	void *bios_video_save_override_pointer_table;
	unsigned long long int reserved_2;
	unsigned char keyboard_nmi_control_flags;
	unsigned int keyboard_break_pending_flags;
	unsigned char port_60_single_byte_queue;
	unsigned char scan_code_of_last_key;
	unsigned char nmi_buffer_head_pointer;
	unsigned char nmi_buffer_tail_pointer;
	unsigned char nmi_scan_code_buffer[16];
	unsigned char reserved_3;
	unsigned short day_counter_1;
	unsigned long long int reserved_4[4];
	unsigned char intra_applications_communications_area[16];
	unsigned char print_screen_status_byte;
	unsigned char used_by_basic[3];
	unsigned char dos_single_diskette_mode;
	unsigned char post_word_area[10];
	unsigned char basic_shell_flag;
	unsigned short basics_default_ds_value;
	void *basic_int_1c_interrupt_handler;
	void *basic_int_23_interrupt_handler;
	void *basic_int_24_interrupt_handler;
	unsigned short reserved_5;
	unsigned short dos_dynamic_storage;
	unsigned char dos_diskette_initialization_table[14];
	unsigned int mode_command;
} __attribute__((packed)) BIOSDataArea;

BIOSDataArea const *get_bios_data_area(void);
void const *get_extended_bios_data_area(void);
void print_bios_data_area(BIOSDataArea const *bios_data_area);

#endif

