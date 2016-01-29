global ScalarProdASM
global CopyVecASM


section .text
; rdi v1; rsi v2
ScalarProdASM:
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
