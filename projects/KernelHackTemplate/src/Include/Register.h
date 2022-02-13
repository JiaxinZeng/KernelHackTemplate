#ifndef K_HACK_TEMPLATE_REGISTER
#define K_HACK_TEMPLATE_REGISTER

#define WRITE_CR3(val) 	__asm__ __volatile__("mov %0, %%cr3" \
: \
: "q"(val) \
: "memory")

#endif
