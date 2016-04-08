global ScalarProdASM
global initDepthBufferASM
global initDepthBufferASM2
global CopyVecASM
global InterpolateASM
global Vec4Mat4ProductASM


section .data

BOUND: dd 1000.0, 1000.0, 1000.0, 1000.0

section .text


ScalarProdASM:
	; rdi v1; rsi v2
	push rbp
	mov rbp, rsp
	pxor xmm2, xmm2 ; suma
 	pxor xmm0, xmm0
 	pxor xmm1, xmm1
	movdqu xmm0, [rdi] ; v1_1 | v1_2 | v1_3 | v1_4
	movdqu xmm1, [rsi] ; v2_1 | v2_2 | v2_3 | v2_4
	mulps xmm0, xmm1 ; v1_1*v2_1 | v1_2*v2_2 | v1_3*v2_3 | v1_4*v2_4
	movdqu xmm2, xmm0 ; v1_1*v2_1 | v1_2*v2_2 | v1_3*v2_3 | v1_4*v2_4
	psrldq xmm2, 8 ; 0 | 0 | v1_1*v2_1 | v1_2*v2_2
	addps xmm0, xmm2; * | * | v1_3*v2_3 + v1_1*v2_1 | v1_4*v2_4 + v1_2*v2_2
	movdqu xmm2, xmm0 ; * | * | v1_3*v2_3 + v1_1*v2_1 | v1_4*v2_4 + v1_2*v2_2
	psrldq xmm2, 4 ; * | * | * | v1_3*v2_3 + v1_1*v2_1 
	addps xmm0, xmm2; * | * | * | v1_4*v2_4 + v1_2*v2_2 + v1_3*v2_3 + v1_1*v2_1 
	pop rbp
	ret


initDepthBufferASM:
	; rdi buffer; rsi tam
	push rbp
	mov rbp, rsp
	xor rax, rax
	xor rcx, rcx
	mov rax, rdi ; buffer
	movdqu xmm0, [BOUND]		;1000|  1000|  1000|  1000	
	.ciclo:
		cmp rcx, rsi
		je .fin
		movups [rax], xmm0
		add rax, 16
		add rcx, 4
		jmp .ciclo
	.fin:	
	pop rbp
	ret

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

CopyVecASM:
	; rdi dest; rsi or
	push rbp
	mov rbp, rsp
 	pxor xmm0, xmm0
	movdqu xmm0, [rsi] ; or_1 | or_2 | or_3 | or_4
	movdqu [rdi], xmm0 ; dest_1 | dest_2 | dest_3 | dest_4
	pop rbp
	ret


InterpolateASM:
	;rdi {grad_1; grad_2; grad_3; grad_4}; rsi {max_1; max_2 ; max_3 ;max_4 }; rdx {min_1; min_2; min_3; min_4; }
	; rcx {1 ;1; 1; 1} ;r8 {res_1; res_2 ;res_3 ;res_4}

	push rbp
	mov rbp, rsp
	movdqu xmm0, [rdi] ; grad
	movdqu xmm1, [rsi] ; max
	movdqu xmm2, [rdx] ; min
	xorps xmm3, xmm3 ; 0
	movdqu xmm4, [rcx] ; 1
	movdqu xmm6, xmm0 ; grad
	movdqu xmm7, xmm0
	subps xmm1, xmm2 ; max - min 
	pcmpgtd xmm6, xmm3 ; grad > 0
	pcmpeqd xmm7, xmm4; grad = 1
	pcmpgtd xmm4, xmm0 ; 1 > grad
	orps xmm4, xmm7 ; 1 = grad
	andps xmm4, xmm6 ; 1 >= grad > 0 | 1 >= grad > 0 | 1 > grad > 0 | 1 > grad > 0 (si se cumple me queda FFFF sino 0000)
	andps xmm4, xmm0 ; 1 > grad > 0 & grad | 1 > grad > 0 & grad | 1 > grad > 0 & grad | 1 > grad > 0 & grad (CLAMP)
	mulps xmm1, xmm4 ; (max - min)*clamp
	addps xmm2, xmm1 ; min + (max - min)*clamp
	movdqu [r8], xmm2 ; min + (max - min)*clamp
	pop rbp
	ret

Vec4Mat4ProductASM:
; rdi v, rsi m, rdx r, aux rcx
	push rbp
	mov rbp,rsp
	movdqu xmm0, [rdi] ; v_1 | v_2 | v_3 | v_4
	movdqu xmm1, [rsi]
	movdqu xmm2, [rsi + 16]
	movdqu xmm3, [rsi + 32]
	movdqu xmm4, [rsi + 48]
	movups xmm5, [rcx]
	mulps xmm1, xmm0
	mulps xmm2, xmm0
	mulps xmm3, xmm0
	mulps xmm4, xmm0
	haddps xmm1, xmm1
	haddps xmm1, xmm1
	haddps xmm2, xmm2
	haddps xmm2, xmm2
	haddps xmm3, xmm3
	haddps xmm3, xmm3
	haddps xmm4, xmm4
	haddps xmm4, xmm4
	mulps xmm1, xmm5
	pslldq xmm5, 4
	mulps xmm2, xmm5
	pslldq xmm5, 4
	mulps xmm3, xmm5
	pslldq xmm5, 4
	mulps xmm4, xmm5
	addps xmm1, xmm2
	addps xmm1, xmm3
	addps xmm1, xmm4
	movups [rdx], xmm1
	pop rbp
	ret