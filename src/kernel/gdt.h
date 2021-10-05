// bibliography
// https://wiki.osdev.org/Global_Descriptor_Table

#ifndef _GDT_H_
#define _GDT_H_

extern unsigned short whole_memory_segment_selector;
extern unsigned short kernel_code_segment_selector;
extern unsigned short boot_information_segment_selector;
extern unsigned short memory_map_segment_selector;
extern unsigned short loaded_disk_segment_selector;
extern unsigned short first_fat_segment_selector;
extern unsigned short second_fat_segment_selector;
extern unsigned short root_directory_entry_segment_selector;

typedef struct
{
	unsigned short limit_low;
		// limit 0:15 bits
	unsigned short base_low;
		// base 0:15 bits
	unsigned char base_mid;
		// base 16:23 bits
	unsigned char access_right;
	#define SEGMENT_DESCRIPTOR_ACCESSED 0x01
		// The CPU set this to 1 when the segment is accessed.
	#define SEGMENT_DESCRIPTOR_READABLE 0x02
		// Readable bit for code selectors.
		// Whether read access for this segment is allowed.
		// Write access is never allowed for code segment.
	#define SEGMENT_DESCRIPTOR_WRITABLE 0x02
		// Writable bit for data selectors.
		// Whether write access for this segment is allowed.
		// Read access is always allowed for data segment.
	#define SEGMENT_DESCRIPTOR_GROW_DOWN 0x04
		// Direction bit for data selectors.
		// Tells the direction.
		// 0 the segment grows up.
		// 1 the segment grows down.
	#define SEGMENT_DESCRIPTOR_COMFORMING 0x04
		// Comforming bit for code selectors
		// If 1 code in this segment can be executed from an equal or lower privilege level.
		// For example, code in ring 3 can far-jump to comforming code in a ring 2 segment.
		// The privl-bits present the highest privilege levle that allowed to execute the segment.
		// Fpr example, code in ring 0 cannot far-jump to a comforming code segmentwith privl==0x2, while 2 and 3 can.
		// Note that the privilege level remains the same, i.e. a far-jump from ring 3 to a privl==2-segment remains in ring 3 after the jump.
		// If 0 code in this segment can only be executed from the ring set in privl.
	#define SEGMENT_DESCRIPTOR_EXECUTABLE 0x08
		// Executable bit
		// If 1 code in this segment can be executed, i.e. a code selector.
		// IF 0 it is a data selector.
	#define SEGMENT_DESCRIPTOR_CODE_OR_DATA 0x10
		// Descriptor type.
		// This bit should be set for code or data segments and should be cleared system segments (e.g. a Task State Segment).
	#define SEGMENT_DESCRIPTOR_PRIVILEGE 0x60
		// The ring level.
		// 0 = highest (kernel)
		// 3 = lowest (user applications)
	#define SEGMENT_DESCRIPTOR_PRESENT 0x80
		// Present bit
		// This must be 1 for all valid selectors.
	unsigned char limit_high;
	#define SEGMENT_DESCRIPTOR_LIMIT_HIGH 0x0f
		// limit 16:19 bits
	#define SEGMENT_DESCRIPTOR_SIZE 0x40
		// If 0 the selector defines 16 bit protected mode.
		// If 1 it defines 32 bit protected mode.
	#define SEGMENT_DESCRIPTOR_GRANULARITY 0x80
		// If 0 the limit is in 1 blocks (byte granularity).
		// If 1 the limit is in 4 KiB blocks (page granularity).
	unsigned char base_high;
		// base 24:31 bits
} SegmentDescriptor;

void init_gdt(void);

// return a new segment selector
// return 0 if failed
unsigned short set_segment(unsigned int base, unsigned int limit, unsigned char access_right);

#endif
