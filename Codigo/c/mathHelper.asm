global initDepthBufferASM2

section .data

BOUND: dd 1000.0, 1000.0, 1000.0, 1000.0

section .text

initDepthBufferASM2:
	; rdi buffer; rsi vec_max; rdx tam
	push rbp
	mov rbp, rsp
	xor rax, rax
	xor rcx, rcx
	mov rax, rdi ; buffer
	movdqu xmm0, [rsi] ; FLT_MAX | FLT_MAX | FLT_MAX | FLT_MAX
	.ciclo:
		cmp rcx, rdx
		je .fin
		movups [rax], xmm0
		add rax, 16
		add rcx, 4
		jmp .ciclo
	.fin:	
	pop rbp
	ret
