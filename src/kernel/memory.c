#include "gdt.h"
#include "io.h"
#include "memory.h"
#include "serial.h"
#include "stdlib.h"
#include "task.h"

BIOSDataArea const * const bios_data_area = MEMORY_MAP_BIOS_DATA_AREA;
MemorySection *root_memory_section;
void * const heap_base = MEMORY_MAP_KERNEL_HEAP_BEGIN;

void free(void *address)
{
	MemorySection *memory_section;
	cli_task();
	memory_section = (MemorySection *)(address - sizeof(*memory_section));
	if(memory_section->flags & MEMORY_SECTION_ALLOCATED)
	{
		memory_section->flags &= ~MEMORY_SECTION_ALLOCATED;
		if((void *)memory_section->previous + sizeof(*memory_section->previous) + memory_section->previous->size == (void *)memory_section && memory_section->previous->flags == memory_section->flags) // merge memory_section and memory_section->previous
		{
			if(root_memory_section == memory_section)root_memory_section = memory_section->previous;
			memory_section->previous->next = memory_section->next;
			memory_section->next->previous = memory_section->previous;
			memory_section->previous->size += sizeof(*memory_section) + memory_section->size;
			memory_section = memory_section->previous;
		}
		if((void *)memory_section + sizeof(*memory_section) + memory_section->size == (void *)memory_section->next && memory_section->next->flags == memory_section->flags) // merge memory_section and memory_section->next
		{
			if(root_memory_section == memory_section->next)root_memory_section = memory_section;
			memory_section->size += sizeof(*memory_section->next) + memory_section->next->size;
			memory_section->next->next->previous = memory_section;
			memory_section->next = memory_section->next->next;
		}
	}
	else ERROR(); // double free error!
	sti_task();
	return;
}

BIOSDataArea const *get_bios_data_area(void)
{
	return bios_data_area;
}

MemoryRegionDescriptor get_memory_region_descriptor(unsigned int index)
{
	return ((MemoryRegionDescriptor const *)MEMORY_MAP_MEMORY_INFORMATION)[index];
}

MemorySection const *get_root_memory_section(void)
{
	return root_memory_section;
}

unsigned int get_free_memory_space_size(void)
{
	unsigned int free_memory_space_size = 0;
	MemorySection *memory_section;
	cli_task();
	memory_section = root_memory_section;
	do
	{
		if(!(memory_section->flags & MEMORY_SECTION_ALLOCATED))free_memory_space_size += memory_section->size;
		memory_section = memory_section->next;
	} while(memory_section != root_memory_section);
	sti_task();
	return free_memory_space_size;
}

void init_memory(void)
{
	MemoryRegionDescriptor memory_region_descriptor;
	MemorySection *memory_section;
	unsigned int memory_region_descriptor_index = 0;;
	root_memory_section = NULL;
	print_serial("Check memory regions\n");
	do
	{
		memory_region_descriptor = get_memory_region_descriptor(memory_region_descriptor_index);
		printf_serial_without_malloc("base = %#018llx, length = %#018llx, type = %#010x, attribute = %#010x\n", memory_region_descriptor.base, memory_region_descriptor.length, memory_region_descriptor.type, memory_region_descriptor.attribute);
		if(memory_region_descriptor.type == MEMORY_REGION_AVAILABLE && (unsigned long long int)(unsigned int)heap_base + sizeof(MemorySection) < memory_region_descriptor.base + memory_region_descriptor.length && memory_region_descriptor.base < 0x0000000100000000)
		{
			if(memory_region_descriptor.base < (unsigned long long int)(unsigned int)heap_base)
			{
				memory_section = heap_base;
				if(0x0000000100000000 < memory_region_descriptor.base + memory_region_descriptor.length)memory_section->size = 0x00000000 - (unsigned int)heap_base - sizeof(*memory_section);
				else memory_section->size = (unsigned int)(memory_region_descriptor.base + memory_region_descriptor.length - (unsigned long long int)(unsigned int)heap_base) - sizeof(*memory_section);
			}
			else
			{
				memory_section = (MemorySection *)(unsigned int)memory_region_descriptor.base;
				if(0x0000000100000000 < memory_region_descriptor.base + memory_region_descriptor.length)memory_section->size = 0x00000000 - (unsigned int)memory_section - sizeof(*memory_section);
				else memory_section->size = (unsigned int)(memory_region_descriptor.length) - sizeof(*memory_section);
			}
			memory_section->flags = 0x00;
			if(!root_memory_section)
			{
				root_memory_section = memory_section;
				root_memory_section->previous = root_memory_section;
				root_memory_section->next = root_memory_section;
			}
			else
			{
				memory_section->previous = root_memory_section->previous;
				memory_section->next = root_memory_section;
				root_memory_section->previous->next = memory_section;
				root_memory_section->previous = memory_section;
			}
		}
		memory_region_descriptor_index++;
	} while(memory_region_descriptor.base != 0 || memory_region_descriptor.length != 0 || memory_region_descriptor.type != 0 || memory_region_descriptor.attribute != 0);
	print_serial("\nCheck memory sections\n");
	memory_section = root_memory_section;
	do
	{
		printf_serial_without_malloc("previous = %p, this = %p, next = %p, size = %#010x, flags = %#04x\n", memory_section->previous, memory_section, memory_section->next, memory_section->size, memory_section->flags);
		memory_section = memory_section->next;
	}while(memory_section != root_memory_section);
	new_line_serial();
}

void *malloc(size_t size)
{
	MemorySection *memory_section;
	if(!size)
	{
		ERROR();
		return NULL;
	}
	cli_task();
	memory_section = root_memory_section;
	do
	{
		if(!(memory_section->flags & MEMORY_SECTION_ALLOCATED) && size <= memory_section->size)
		{
			void *allocated = (void *)memory_section + sizeof(*memory_section);
			if(memory_section->size <= size + sizeof(MemorySection))memory_section->flags |= MEMORY_SECTION_ALLOCATED; // Can't devide the memory section
			else // devide the memory section
			{
				MemorySection *remaining_memory_section = (MemorySection *)((void *)memory_section + sizeof(*memory_section) + size);
				remaining_memory_section->previous = memory_section;
				remaining_memory_section->next = memory_section->next;
				remaining_memory_section->size = memory_section->size - size - sizeof(*remaining_memory_section);
				remaining_memory_section->flags = memory_section->flags;
				memory_section->next->previous = remaining_memory_section;
				memory_section->next = remaining_memory_section;
				memory_section->size = size;
				memory_section->flags |= MEMORY_SECTION_ALLOCATED;
			}
			root_memory_section = memory_section->next;
			sti_task();
			return allocated;
		}
		memory_section = memory_section->next;
	} while(memory_section != root_memory_section);
	sti_task();
	ERROR(); // Can't find enough size free memory section!
	return NULL;
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
	printf_serial("bios_data_area->nec_diskette_comtroller_status = %#04.2x\n", bios_data_area->nec_diskette_comtroller_status);
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
	printf_serial("bios_data_area->intra_applications_communications_area = %#04.2x\n", bios_data_area->intra_applications_communications_area);
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
	printf_serial("bios_data_area->dos_diskette_initialization_table = %#04.2x\n", bios_data_area->dos_diskette_initialization_table);
	printf_serial("bios_data_area->mode_command = %#010.8x\n", bios_data_area->mode_command);
}

