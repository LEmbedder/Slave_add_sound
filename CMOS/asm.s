	;AREA    |.text|, CODE, READONLY
	
;SPIREAD PROC
	;EXPORT		SPIREAD		
	
	;push {r2-r7}
;_spistart
	
	;ldr r2,#(0x40011000+0x14) ;clr
	
	;ldr r3,#(0x40011000+0x10) ;set
	
	;ldr r4,#(0x40010C08) ;data
	
	;ldr r5,#0x4000	;0x4000
	
	
	
;_spisloop
	;add r1,r1,r0
	;cmp r0,r1
	;bge _spiend
	

;_spiend
	;pop	{r2-r7}
	
	;mov pc,lr
	
	;ENDP
	;ALIGN
	
	;END
			