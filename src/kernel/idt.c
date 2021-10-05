#include "gdt.h"
#include "idt.h"
#include "interrupt_gates.h"
#include "io.h"
#include "serial.h"

#define IDT_BEGIN ((InterruptDescriptor *)0x00268000)
#define IDT_END (IDT_BEGIN + 0x00000100)

void init_idt(void)
{
	// devide by 0 exception
	set_gate(IDT_BEGIN + 0x00000000, interrupt_gate0x00, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// debug exception
	set_gate(IDT_BEGIN + 0x00000001, interrupt_gate0x01, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// non maskable interrupt
	set_gate(IDT_BEGIN + 0x00000002, interrupt_gate0x02, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// breakpoint exception
	set_gate(IDT_BEGIN + 0x00000003, interrupt_gate0x03, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// overflow exception
	set_gate(IDT_BEGIN + 0x00000004, interrupt_gate0x04, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// bound range exceeded exception
	set_gate(IDT_BEGIN + 0x00000005, interrupt_gate0x05, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// invalid opcode exception
	set_gate(IDT_BEGIN + 0x00000006, interrupt_gate0x06, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// device not available exception
	set_gate(IDT_BEGIN + 0x00000007, interrupt_gate0x07, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// double fault exception
	set_gate(IDT_BEGIN + 0x00000008, interrupt_gate0x08, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// coprocessor segment overrun exception
	set_gate(IDT_BEGIN + 0x00000009, interrupt_gate0x09, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// invalid TSS exception
	set_gate(IDT_BEGIN + 0x0000000a, interrupt_gate0x0a, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// segment not present exception
	set_gate(IDT_BEGIN + 0x0000000b, interrupt_gate0x0b, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// stack segment fault exception
	set_gate(IDT_BEGIN + 0x0000000c, interrupt_gate0x0c, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// general protection fault exception
	set_gate(IDT_BEGIN + 0x0000000d, interrupt_gate0x0d, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// page fault exception
	set_gate(IDT_BEGIN + 0x0000000e, interrupt_gate0x0e, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate(IDT_BEGIN + 0x0000000f, interrupt_gate0x0f, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// x87 floating point exception
	set_gate(IDT_BEGIN + 0x00000010, interrupt_gate0x10, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// alignment check exception
	set_gate(IDT_BEGIN + 0x00000011, interrupt_gate0x11, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// machine check exception
	set_gate(IDT_BEGIN + 0x00000012, interrupt_gate0x12, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// SIMD floating point exception
	set_gate(IDT_BEGIN + 0x00000013, interrupt_gate0x13, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// virtualization exception
	set_gate(IDT_BEGIN + 0x00000014, interrupt_gate0x14, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate(IDT_BEGIN + 0x00000015, interrupt_gate0x15, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate(IDT_BEGIN + 0x00000016, interrupt_gate0x16, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate(IDT_BEGIN + 0x00000017, interrupt_gate0x17, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate(IDT_BEGIN + 0x00000018, interrupt_gate0x18, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate(IDT_BEGIN + 0x00000019, interrupt_gate0x19, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate(IDT_BEGIN + 0x0000001a, interrupt_gate0x1a, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate(IDT_BEGIN + 0x0000001b, interrupt_gate0x1b, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate(IDT_BEGIN + 0x0000001c, interrupt_gate0x1c, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate(IDT_BEGIN + 0x0000001d, interrupt_gate0x1d, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// security exception
	set_gate(IDT_BEGIN + 0x0000001e, interrupt_gate0x1e, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate(IDT_BEGIN + 0x0000001f, interrupt_gate0x1f, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// timer interrupt
	set_gate(IDT_BEGIN + 0x00000020, interrupt_gate0x20, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// kerboard interrupt
	set_gate(IDT_BEGIN + 0x00000021, interrupt_gate0x21, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// slave pic interrupt
	set_gate(IDT_BEGIN + 0x00000022, interrupt_gate0x22, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// COM2 interrupt
	set_gate(IDT_BEGIN + 0x00000023, interrupt_gate0x23, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// COM1 interrupt
	set_gate(IDT_BEGIN + 0x00000024, interrupt_gate0x24, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// LPT2 interrupt
	set_gate(IDT_BEGIN + 0x00000025, interrupt_gate0x25, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// disk interrupt
	set_gate(IDT_BEGIN + 0x00000026, interrupt_gate0x26, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// LPT1 interrupt
	set_gate(IDT_BEGIN + 0x00000027, interrupt_gate0x27, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// real time clock interrupt
	set_gate(IDT_BEGIN + 0x00000028, interrupt_gate0x28, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// peripheral0 interrupt
	set_gate(IDT_BEGIN + 0x00000029, interrupt_gate0x29, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// peripheral1 interrupt
	set_gate(IDT_BEGIN + 0x0000002a, interrupt_gate0x2a, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// peripheral2 interrupt
	set_gate(IDT_BEGIN + 0x0000002b, interrupt_gate0x2b, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// mouse interrupt
	set_gate(IDT_BEGIN + 0x0000002c, interrupt_gate0x2c, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// FPU error exception
	set_gate(IDT_BEGIN + 0x0000002d, interrupt_gate0x2d, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// primary ATA hard disk interrupt
	set_gate(IDT_BEGIN + 0x0000002e, interrupt_gate0x2e, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// secondary ATA hard disk interrupt
	set_gate(IDT_BEGIN + 0x0000002f, interrupt_gate0x2f, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// other interrupts
	set_gate(IDT_BEGIN + 0x00000030, interrupt_gate0x30, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000031, interrupt_gate0x31, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000032, interrupt_gate0x32, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000033, interrupt_gate0x33, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000034, interrupt_gate0x34, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000035, interrupt_gate0x35, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000036, interrupt_gate0x36, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000037, interrupt_gate0x37, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000038, interrupt_gate0x38, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000039, interrupt_gate0x39, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000003a, interrupt_gate0x3a, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000003b, interrupt_gate0x3b, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000003c, interrupt_gate0x3c, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000003d, interrupt_gate0x3d, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000003e, interrupt_gate0x3e, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000003f, interrupt_gate0x3f, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000040, interrupt_gate0x40, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000041, interrupt_gate0x41, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000042, interrupt_gate0x42, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000043, interrupt_gate0x43, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000044, interrupt_gate0x44, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000045, interrupt_gate0x45, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000046, interrupt_gate0x46, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000047, interrupt_gate0x47, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000048, interrupt_gate0x48, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000049, interrupt_gate0x49, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000004a, interrupt_gate0x4a, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000004b, interrupt_gate0x4b, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000004c, interrupt_gate0x4c, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000004d, interrupt_gate0x4d, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000004e, interrupt_gate0x4e, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000004f, interrupt_gate0x4f, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000050, interrupt_gate0x50, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000051, interrupt_gate0x51, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000052, interrupt_gate0x52, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000053, interrupt_gate0x53, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000054, interrupt_gate0x54, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000055, interrupt_gate0x55, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000056, interrupt_gate0x56, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000057, interrupt_gate0x57, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000058, interrupt_gate0x58, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000059, interrupt_gate0x59, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000005a, interrupt_gate0x5a, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000005b, interrupt_gate0x5b, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000005c, interrupt_gate0x5c, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000005d, interrupt_gate0x5d, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000005e, interrupt_gate0x5e, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000005f, interrupt_gate0x5f, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000060, interrupt_gate0x60, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000061, interrupt_gate0x61, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000062, interrupt_gate0x62, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000063, interrupt_gate0x63, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000064, interrupt_gate0x64, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000065, interrupt_gate0x65, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000066, interrupt_gate0x66, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000067, interrupt_gate0x67, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000068, interrupt_gate0x68, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000069, interrupt_gate0x69, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000006a, interrupt_gate0x6a, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000006b, interrupt_gate0x6b, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000006c, interrupt_gate0x6c, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000006d, interrupt_gate0x6d, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000006e, interrupt_gate0x6e, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000006f, interrupt_gate0x6f, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000070, interrupt_gate0x70, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000071, interrupt_gate0x71, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000072, interrupt_gate0x72, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000073, interrupt_gate0x73, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000074, interrupt_gate0x74, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000075, interrupt_gate0x75, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000076, interrupt_gate0x76, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000077, interrupt_gate0x77, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000078, interrupt_gate0x78, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000079, interrupt_gate0x79, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000007a, interrupt_gate0x7a, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000007b, interrupt_gate0x7b, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000007c, interrupt_gate0x7c, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000007d, interrupt_gate0x7d, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000007e, interrupt_gate0x7e, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000007f, interrupt_gate0x7f, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000080, interrupt_gate0x80, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000081, interrupt_gate0x81, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000082, interrupt_gate0x82, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000083, interrupt_gate0x83, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000084, interrupt_gate0x84, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000085, interrupt_gate0x85, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000086, interrupt_gate0x86, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000087, interrupt_gate0x87, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000088, interrupt_gate0x88, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000089, interrupt_gate0x89, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000008a, interrupt_gate0x8a, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000008b, interrupt_gate0x8b, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000008c, interrupt_gate0x8c, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000008d, interrupt_gate0x8d, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000008e, interrupt_gate0x8e, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000008f, interrupt_gate0x8f, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000090, interrupt_gate0x90, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000091, interrupt_gate0x91, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000092, interrupt_gate0x92, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000093, interrupt_gate0x93, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000094, interrupt_gate0x94, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000095, interrupt_gate0x95, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000096, interrupt_gate0x96, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000097, interrupt_gate0x97, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000098, interrupt_gate0x98, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x00000099, interrupt_gate0x99, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000009a, interrupt_gate0x9a, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000009b, interrupt_gate0x9b, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000009c, interrupt_gate0x9c, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000009d, interrupt_gate0x9d, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000009e, interrupt_gate0x9e, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x0000009f, interrupt_gate0x9f, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000a0, interrupt_gate0xa0, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000a1, interrupt_gate0xa1, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000a2, interrupt_gate0xa2, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000a3, interrupt_gate0xa3, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000a4, interrupt_gate0xa4, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000a5, interrupt_gate0xa5, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000a6, interrupt_gate0xa6, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000a7, interrupt_gate0xa7, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000a8, interrupt_gate0xa8, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000a9, interrupt_gate0xa9, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000aa, interrupt_gate0xaa, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000ab, interrupt_gate0xab, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000ac, interrupt_gate0xac, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000ad, interrupt_gate0xad, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000ae, interrupt_gate0xae, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000af, interrupt_gate0xaf, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000b0, interrupt_gate0xb0, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000b1, interrupt_gate0xb1, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000b2, interrupt_gate0xb2, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000b3, interrupt_gate0xb3, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000b4, interrupt_gate0xb4, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000b5, interrupt_gate0xb5, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000b6, interrupt_gate0xb6, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000b7, interrupt_gate0xb7, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000b8, interrupt_gate0xb8, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000b9, interrupt_gate0xb9, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000ba, interrupt_gate0xba, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000bb, interrupt_gate0xbb, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000bc, interrupt_gate0xbc, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000bd, interrupt_gate0xbd, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000be, interrupt_gate0xbe, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000bf, interrupt_gate0xbf, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000c0, interrupt_gate0xc0, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000c1, interrupt_gate0xc1, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000c2, interrupt_gate0xc2, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000c3, interrupt_gate0xc3, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000c4, interrupt_gate0xc4, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000c5, interrupt_gate0xc5, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000c6, interrupt_gate0xc6, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000c7, interrupt_gate0xc7, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000c8, interrupt_gate0xc8, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000c9, interrupt_gate0xc9, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000ca, interrupt_gate0xca, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000cb, interrupt_gate0xcb, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000cc, interrupt_gate0xcc, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000cd, interrupt_gate0xcd, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000ce, interrupt_gate0xce, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000cf, interrupt_gate0xcf, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000d0, interrupt_gate0xd0, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000d1, interrupt_gate0xd1, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000d2, interrupt_gate0xd2, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000d3, interrupt_gate0xd3, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000d4, interrupt_gate0xd4, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000d5, interrupt_gate0xd5, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000d6, interrupt_gate0xd6, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000d7, interrupt_gate0xd7, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000d8, interrupt_gate0xd8, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000d9, interrupt_gate0xd9, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000da, interrupt_gate0xda, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000db, interrupt_gate0xdb, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000dc, interrupt_gate0xdc, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000dd, interrupt_gate0xdd, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000de, interrupt_gate0xde, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000df, interrupt_gate0xdf, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000e0, interrupt_gate0xe0, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000e1, interrupt_gate0xe1, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000e2, interrupt_gate0xe2, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000e3, interrupt_gate0xe3, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000e4, interrupt_gate0xe4, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000e5, interrupt_gate0xe5, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000e6, interrupt_gate0xe6, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000e7, interrupt_gate0xe7, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000e8, interrupt_gate0xe8, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000e9, interrupt_gate0xe9, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000ea, interrupt_gate0xea, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000eb, interrupt_gate0xeb, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000ec, interrupt_gate0xec, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000ed, interrupt_gate0xed, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000ee, interrupt_gate0xee, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000ef, interrupt_gate0xef, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000f0, interrupt_gate0xf0, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000f1, interrupt_gate0xf1, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000f2, interrupt_gate0xf2, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000f3, interrupt_gate0xf3, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000f4, interrupt_gate0xf4, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000f5, interrupt_gate0xf5, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000f6, interrupt_gate0xf6, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000f7, interrupt_gate0xf7, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000f8, interrupt_gate0xf8, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000f9, interrupt_gate0xf9, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000fa, interrupt_gate0xfa, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000fb, interrupt_gate0xfb, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000fc, interrupt_gate0xfc, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000fd, interrupt_gate0xfd, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000fe, interrupt_gate0xfe, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	set_gate(IDT_BEGIN + 0x000000ff, interrupt_gate0xff, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);

	// load IDT
	lidt(0x07ff, IDT_BEGIN);

	// check IDT
	for(InterruptDescriptor const *interrupt_descriptor_checker = IDT_BEGIN; interrupt_descriptor_checker < IDT_END; interrupt_descriptor_checker++)
	{
		printf_serial("Interrupt Descriptor %p\n", interrupt_descriptor_checker);
		printf_serial("\t offset_low\t%#06x\n", interrupt_descriptor_checker->offset_low);
		printf_serial("\t selector\t%#06x\n", interrupt_descriptor_checker->selector);
		printf_serial("\t zero\t\t%#04x\n", interrupt_descriptor_checker->zero);
		printf_serial("\t flags\t\t%#04x\n", interrupt_descriptor_checker->flags);
		printf_serial("\t offset_high\t%#06x\n", interrupt_descriptor_checker->offset_high);
	}
	new_line_serial();
}

void set_gate(InterruptDescriptor *interrupt_descriptor, void (*handler)(void), unsigned char flags)
{
	interrupt_descriptor->offset_low = (unsigned short)((unsigned int)handler & 0x0000ffff);
	interrupt_descriptor->selector = kernel_code_segment_selector;
	interrupt_descriptor->zero = 0;
	interrupt_descriptor->flags = flags | INTERRUPT_DESCRIPTOR_PRESENT;
	interrupt_descriptor->offset_high = (unsigned short)((unsigned int)handler >> 16 & 0x0000ffff);
}

