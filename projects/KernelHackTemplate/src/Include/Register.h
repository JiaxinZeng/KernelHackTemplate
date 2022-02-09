#ifndef K_HACK_TEMPLATE_REGISTER
#define K_HACK_TEMPLATE_REGISTER

inline void write_cr3(unsigned long long val)
{
	__asm__ __volatile__("mov %0, %%cr3":: "q"(val) : "memory");
}

#endif
