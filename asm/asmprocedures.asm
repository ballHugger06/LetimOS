global loadGDTR
global getRFLAGS
global enableInterrupts
global disableInterrupts
global haltCpu
global breakpnt

section .text
breakpnt:
	int 3
	ret

haltCpu:
	hlt
	ret

loadGDTR:
	LGDT [rdi]
	ret

getRFLAGS:
	PUSHF
	POP rax
	ret

enableInterrupts:
	STI
	ret

disableInterrupts:
	CLI
	ret
