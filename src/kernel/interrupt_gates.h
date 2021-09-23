#ifndef _INTERRUPT_GATES_H_
#define _INTERRUPT_GATES_H_

// devide by 0 exception gate
void interrupt_gate0x00(void);

// debug exception gate
void interrupt_gate0x01(void);

// non maskable interrupt gate
void interrupt_gate0x02(void);

// breakpoint exception gate
void interrupt_gate0x03(void);

// overflow exception gate
void interrupt_gate0x04(void);

// bound range exceeded exception gate
void interrupt_gate0x05(void);

// invalid opcode exception gate
void interrupt_gate0x06(void);

// device not available exception gate
void interrupt_gate0x07(void);

// double fault exception gate
void interrupt_gate0x08(void);

// coprocessor segment overrun exception gate
void interrupt_gate0x09(void);

// invalid TSS exception gate
void interrupt_gate0x0a(void);

// segment not present exception gate
void interrupt_gate0x0b(void);

// stack segment fault exception gate
void interrupt_gate0x0c(void);

// general protection fault exception gate
void interrupt_gate0x0d(void);

// page fault exception gate
void interrupt_gate0x0e(void);

// reserved exception gate
void interrupt_gate0x0f(void);

// x87 floating point exception gate
void interrupt_gate0x10(void);

// alignment check exception gate
void interrupt_gate0x11(void);

// machine check exception gate
void interrupt_gate0x12(void);

// SIMD floating point exception gate
void interrupt_gate0x13(void);

// virtualization exception gate
void interrupt_gate0x14(void);

// reserved exception gate
void interrupt_gate0x15(void);

// reserved exception gate
void interrupt_gate0x16(void);

// reserved exception gate
void interrupt_gate0x17(void);

// reserved exception gate
void interrupt_gate0x18(void);

// reserved exception gate
void interrupt_gate0x19(void);

// reserved exception gate
void interrupt_gate0x1a(void);

// reserved exception gate
void interrupt_gate0x1b(void);

// reserved exception gate
void interrupt_gate0x1c(void);

// reserved exception gate
void interrupt_gate0x1d(void);

// security exception gate
void interrupt_gate0x1e(void);

// security exception gate
void interrupt_gate0x1f(void);

// security exception gate
void interrupt_gate0x20(void);

// keyboard interrupt gate
void interrupt_gate0x21(void);

// slave pic interrupt gate
void interrupt_gate0x22(void);

// COM2 interrupt gate
void interrupt_gate0x23(void);

// COM1 interrupt gate
void interrupt_gate0x24(void);

// LPT2 interrupt gate
void interrupt_gate0x25(void);

// disk interrupt gate
void interrupt_gate0x26(void);

// LPT1 interrupt gate
void interrupt_gate0x27(void);

// real time clock interrupt gate
void interrupt_gate0x28(void);

// peripheral0 interrupt gate
void interrupt_gate0x29(void);

// peripheral1 interrupt gate
void interrupt_gate0x2a(void);

// peripheral2 interrupt gate
void interrupt_gate0x2b(void);

// mouse interrupt gate
void interrupt_gate0x2c(void);

// FPU error exception gate
void interrupt_gate0x2d(void);

// primary ATA hard disk interrupt gate
void interrupt_gate0x2e(void);

// secondary ATA hard disk interrupt gate
void interrupt_gate0x2f(void);

// other interrupt gates
void interrupt_gate0x30(void);
void interrupt_gate0x31(void);
void interrupt_gate0x32(void);
void interrupt_gate0x33(void);
void interrupt_gate0x34(void);
void interrupt_gate0x35(void);
void interrupt_gate0x36(void);
void interrupt_gate0x37(void);
void interrupt_gate0x38(void);
void interrupt_gate0x39(void);
void interrupt_gate0x3a(void);
void interrupt_gate0x3b(void);
void interrupt_gate0x3c(void);
void interrupt_gate0x3d(void);
void interrupt_gate0x3e(void);
void interrupt_gate0x3f(void);
void interrupt_gate0x40(void);
void interrupt_gate0x41(void);
void interrupt_gate0x42(void);
void interrupt_gate0x43(void);
void interrupt_gate0x44(void);
void interrupt_gate0x45(void);
void interrupt_gate0x46(void);
void interrupt_gate0x47(void);
void interrupt_gate0x48(void);
void interrupt_gate0x49(void);
void interrupt_gate0x4a(void);
void interrupt_gate0x4b(void);
void interrupt_gate0x4c(void);
void interrupt_gate0x4d(void);
void interrupt_gate0x4e(void);
void interrupt_gate0x4f(void);
void interrupt_gate0x50(void);
void interrupt_gate0x51(void);
void interrupt_gate0x52(void);
void interrupt_gate0x53(void);
void interrupt_gate0x54(void);
void interrupt_gate0x55(void);
void interrupt_gate0x56(void);
void interrupt_gate0x57(void);
void interrupt_gate0x58(void);
void interrupt_gate0x59(void);
void interrupt_gate0x5a(void);
void interrupt_gate0x5b(void);
void interrupt_gate0x5c(void);
void interrupt_gate0x5d(void);
void interrupt_gate0x5e(void);
void interrupt_gate0x5f(void);
void interrupt_gate0x60(void);
void interrupt_gate0x61(void);
void interrupt_gate0x62(void);
void interrupt_gate0x63(void);
void interrupt_gate0x64(void);
void interrupt_gate0x65(void);
void interrupt_gate0x66(void);
void interrupt_gate0x67(void);
void interrupt_gate0x68(void);
void interrupt_gate0x69(void);
void interrupt_gate0x6a(void);
void interrupt_gate0x6b(void);
void interrupt_gate0x6c(void);
void interrupt_gate0x6d(void);
void interrupt_gate0x6e(void);
void interrupt_gate0x6f(void);
void interrupt_gate0x70(void);
void interrupt_gate0x71(void);
void interrupt_gate0x72(void);
void interrupt_gate0x73(void);
void interrupt_gate0x74(void);
void interrupt_gate0x75(void);
void interrupt_gate0x76(void);
void interrupt_gate0x77(void);
void interrupt_gate0x78(void);
void interrupt_gate0x79(void);
void interrupt_gate0x7a(void);
void interrupt_gate0x7b(void);
void interrupt_gate0x7c(void);
void interrupt_gate0x7d(void);
void interrupt_gate0x7e(void);
void interrupt_gate0x7f(void);
void interrupt_gate0x80(void);
void interrupt_gate0x81(void);
void interrupt_gate0x82(void);
void interrupt_gate0x83(void);
void interrupt_gate0x84(void);
void interrupt_gate0x85(void);
void interrupt_gate0x86(void);
void interrupt_gate0x87(void);
void interrupt_gate0x88(void);
void interrupt_gate0x89(void);
void interrupt_gate0x8a(void);
void interrupt_gate0x8b(void);
void interrupt_gate0x8c(void);
void interrupt_gate0x8d(void);
void interrupt_gate0x8e(void);
void interrupt_gate0x8f(void);
void interrupt_gate0x90(void);
void interrupt_gate0x91(void);
void interrupt_gate0x92(void);
void interrupt_gate0x93(void);
void interrupt_gate0x94(void);
void interrupt_gate0x95(void);
void interrupt_gate0x96(void);
void interrupt_gate0x97(void);
void interrupt_gate0x98(void);
void interrupt_gate0x99(void);
void interrupt_gate0x9a(void);
void interrupt_gate0x9b(void);
void interrupt_gate0x9c(void);
void interrupt_gate0x9d(void);
void interrupt_gate0x9e(void);
void interrupt_gate0x9f(void);
void interrupt_gate0xa0(void);
void interrupt_gate0xa1(void);
void interrupt_gate0xa2(void);
void interrupt_gate0xa3(void);
void interrupt_gate0xa4(void);
void interrupt_gate0xa5(void);
void interrupt_gate0xa6(void);
void interrupt_gate0xa7(void);
void interrupt_gate0xa8(void);
void interrupt_gate0xa9(void);
void interrupt_gate0xaa(void);
void interrupt_gate0xab(void);
void interrupt_gate0xac(void);
void interrupt_gate0xad(void);
void interrupt_gate0xae(void);
void interrupt_gate0xaf(void);
void interrupt_gate0xb0(void);
void interrupt_gate0xb1(void);
void interrupt_gate0xb2(void);
void interrupt_gate0xb3(void);
void interrupt_gate0xb4(void);
void interrupt_gate0xb5(void);
void interrupt_gate0xb6(void);
void interrupt_gate0xb7(void);
void interrupt_gate0xb8(void);
void interrupt_gate0xb9(void);
void interrupt_gate0xba(void);
void interrupt_gate0xbb(void);
void interrupt_gate0xbc(void);
void interrupt_gate0xbd(void);
void interrupt_gate0xbe(void);
void interrupt_gate0xbf(void);
void interrupt_gate0xc0(void);
void interrupt_gate0xc1(void);
void interrupt_gate0xc2(void);
void interrupt_gate0xc3(void);
void interrupt_gate0xc4(void);
void interrupt_gate0xc5(void);
void interrupt_gate0xc6(void);
void interrupt_gate0xc7(void);
void interrupt_gate0xc8(void);
void interrupt_gate0xc9(void);
void interrupt_gate0xca(void);
void interrupt_gate0xcb(void);
void interrupt_gate0xcc(void);
void interrupt_gate0xcd(void);
void interrupt_gate0xce(void);
void interrupt_gate0xcf(void);
void interrupt_gate0xd0(void);
void interrupt_gate0xd1(void);
void interrupt_gate0xd2(void);
void interrupt_gate0xd3(void);
void interrupt_gate0xd4(void);
void interrupt_gate0xd5(void);
void interrupt_gate0xd6(void);
void interrupt_gate0xd7(void);
void interrupt_gate0xd8(void);
void interrupt_gate0xd9(void);
void interrupt_gate0xda(void);
void interrupt_gate0xdb(void);
void interrupt_gate0xdc(void);
void interrupt_gate0xdd(void);
void interrupt_gate0xde(void);
void interrupt_gate0xdf(void);
void interrupt_gate0xe0(void);
void interrupt_gate0xe1(void);
void interrupt_gate0xe2(void);
void interrupt_gate0xe3(void);
void interrupt_gate0xe4(void);
void interrupt_gate0xe5(void);
void interrupt_gate0xe6(void);
void interrupt_gate0xe7(void);
void interrupt_gate0xe8(void);
void interrupt_gate0xe9(void);
void interrupt_gate0xea(void);
void interrupt_gate0xeb(void);
void interrupt_gate0xec(void);
void interrupt_gate0xed(void);
void interrupt_gate0xee(void);
void interrupt_gate0xef(void);
void interrupt_gate0xf0(void);
void interrupt_gate0xf1(void);
void interrupt_gate0xf2(void);
void interrupt_gate0xf3(void);
void interrupt_gate0xf4(void);
void interrupt_gate0xf5(void);
void interrupt_gate0xf6(void);
void interrupt_gate0xf7(void);
void interrupt_gate0xf8(void);
void interrupt_gate0xf9(void);
void interrupt_gate0xfa(void);
void interrupt_gate0xfb(void);
void interrupt_gate0xfc(void);
void interrupt_gate0xfd(void);
void interrupt_gate0xfe(void);
void interrupt_gate0xff(void);

#endif

