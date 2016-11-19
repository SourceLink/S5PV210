#include "s5pv210.h"

#define DMC0_MEMCONTROL		0x00202400	// MemControl	BL=4, 1Chip, DDR2 Type, dynamic self refresh, force precharge, dynamic power down off
#define DMC0_MEMCONFIG_0	0x20E00323	// MemConfig0	512MB config, 8 banks,Mapping Method[12:15]0:linear, 1:linterleaved, 2:Mixed
#define DMC0_MEMCONFIG_1	0x00F00313	// MemConfig1

#define DMC0_TIMINGA_REF        0x00000618	// TimingAref   7.8us*133MHz=1038(0x40E), 100MHz=780(0x30C), 20MHz=156(0x9C), 10MHz=78(0x4E)
#define DMC0_TIMING_ROW         0x2B34438A	// TimingRow    for @200MHz
#define DMC0_TIMING_DATA        0x24240000	// TimingData   CL=3
#define DMC0_TIMING_PWR         0x0BDC0343	// TimingPower


#define	DMC1_MEMCONTROL		0x00202400	// MemControl	BL=4, 2 chip, DDR2 type, dynamic self refresh, force precharge, dynamic power down off
#define DMC1_MEMCONFIG_0	0x40E00323	// MemConfig0	512MB config, 8 banks,Mapping Method[12:15]0:linear, 1:linterleaved, 2:Mixed
#define DMC1_MEMCONFIG_1	0x00F00313	// MemConfig1

#define DMC1_TIMINGA_REF        0x00000618      // TimingAref   7.8us*133MHz=1038(0x40E), 100MHz=780(0x30C), 20MHz=156(0x9C), 10MHz=78(0x4E)
#define DMC1_TIMING_ROW         0x2B34438A     	// TimingRow    for @200MHz
#define DMC1_TIMING_DATA        0x24240000      // TimingData   CL=3
#define DMC1_TIMING_PWR         0x0BDC0343      // TimingPower


.global mem_init

mem_init : 
	/* step2 */
	ldr	r0, =APB_DMC_0_BASE			/* DMC0_CONCONTROL */
	ldr	r1, =0x00101000				/* set ctrl_start_poin and ctrl_inc */
	str	r1, [r0, #DMC_PHYCONTROL0]
	
	ldr	r1, =0x00101002				/* set ctrl_dll_on */
	str	r1, [r0, #DMC_PHYCONTROL0]
	
	/* step3 */
	ldr	r1, =0x00000086
	str	r1, [r0, #DMC_PHYCONTROL1]
	
	/* step4 */
	ldr	r1, =0x00101003				/* set ctrl_start */
	str	r1, [r0, #DMC_PHYCONTROL0]

wait_dll_locked_0 :
	ldr	r1, [r0, #DMC_PHYSTATUS]
	and	r2, r1, #0x07
	cmp	r2, #0x07
	bne	wait_dll_locked_0
	
 	and 	r1, #0x3fc0   
  	mov 	r2, r1, LSL #18  
  	orr 	r2, r2, #0x100000  
  	orr 	r2, r2, #0x1000  
        
  	orr 	r1, r2, #0x3                		/* Force Value locking */  
  	str 	r1, [r0, #DMC_PHYCONTROL0] 

	/* step5 */
	ldr	r1, =0x0FFF2010
	str	r1, [r0, #DMC_CONCONTROL] 		/* auto refresh off */
	
	/* step6 */
	ldr	r1, =DMC0_MEMCONTROL
	str	r1, [r0, #DMC_MEMCONTROL]
	
	/* step7 */
	ldr	r1, =DMC0_MEMCONFIG_0
	str	r1, [r0,#DMC_MEMCONFIG0]

	/* step8 */
	ldr	r1, =0xFF000000
	str	r1, [r0, #DMC_PRECHCONFIG]
	ldr	r1, =0xFFFF00FF
	ldr	r1, [r0, #DMC_PWRDNCONFIG]
	
	/* step9 */
	ldr	r1, =DMC0_TIMINGA_REF			/* TimingAref */
	str	r1, [r0, #DMC_TIMINGAREF] 		
	
	ldr	r1, =DMC0_TIMING_ROW			/* TimingRow */			
	str	r1, [r0, #DMC_TIMINGROW]

	ldr	r1, =DMC0_TIMING_DATA			/* TimingData */			
	str	r1, [r0, #DMC_TIMINGDATA]
	
	ldr	r1, =DMC0_TIMING_PWR			/* TimingPower */			
	str	r1, [r0, #DMC_TIMINGPOWER]

	/* step14 */
	ldr	r1, =0x07000000				/* DirectCmd	chip0 Deselect */
	str	r1, [r0, #DMC_DIRECTCMD]
	
	/* step16 */
	ldr	r1, =0x01000000				/* DirectCmd	chip0 PALL */
	str	r1, [r0, #DMC_DIRECTCMD]
	
	/* step17 */
	ldr	r1, =0x00020000				/* DirectCmd	chip0 EMRS2 */
	str	r1, [r0, #DMC_DIRECTCMD]

	/* step18 */
	ldr	r1, =0x00030000				/* DirectCmd	chip0 EMRS3 */
	str	r1, [r0, #DMC_DIRECTCMD]
	
	/* step19 */
	ldr	r1, =0x00010400				/* DirectCmd	chip0 EMRS1 (MEM DLL on, DQS# disable) */
	str	r1, [r0, #DMC_DIRECTCMD]

	/* step20 */
	ldr	r1, =0x00000542				/* DirectCmd	chip0 MRS (MEM DLL reset) CL=4, BL=4 */
	str	r1, [r0, #DMC_DIRECTCMD]

	/* step21 */
	ldr	r1, =0x01000000				/* DirectCmd	chip0 PALL */
	str	r1, [r0, #DMC_DIRECTCMD]
	
	/* step22 */
	ldr	r1, =0x05000000				/* DirectCmd	chip0 REFA */ 
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x05000000				/* DirectCmd	chip0 REFA */ 
	str	r1, [r0, #DMC_DIRECTCMD]

	/* step23 */
	ldr	r1, =0x00000442				/* DirectCmd	chip0 MRS (MEM DLL unreset) */ 
	str	r1, [r0, #DMC_DIRECTCMD]

	/* step25 */
	ldr	r1, =0x00010780				/* DirectCmd	chip0 EMRS1 (OCD default) */ 
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x00010400				/* DirectCmd	chip0 EMRS1 (OCD exit) */
	str	r1, [r0, #DMC_DIRECTCMD]



	ldr	r1, =0x07100000				/* DirectCmd	chip1 Deselect */
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x01100000				/* DirectCmd	chip1 PALL */
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x00120000				/* DirectCmd	chip1 EMRS2 */
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x00130000				/* DirectCmd	chip1 EMRS3 */
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x00110400				/* DirectCmd	chip1 EMRS1 (MEM DLL on, DQS# disable) */
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x00100542				/* DirectCmd	chip1 MRS (MEM DLL reset) CL=4, BL=4 */
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x01100000				/* DirectCmd	chip1 PALL */
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x05100000				/* DirectCmd	chip1 REFA */
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x00100442				/* DirectCmd	chip1 MRS (MEM DLL unreset) */
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x00110780				/* DirectCmd	chip1 EMRS1 (OCD default) */
	str	r1, [r0, #DMC_DIRECTCMD]

	ldr	r1, =0x00110400				/* DirectCmd	chip1 EMRS1 (OCD exit) */
	str	r1, [r0, #DMC_DIRECTCMD]



	/* step27 */
	ldr	r1, =0x0FF02030				/* ConControl	auto refresh on */
	str	r1, [r0, #DMC_CONCONTROL]

	ldr	r1, =0xFFFF00FF				/* PwrdnConfig */
	str	r1, [r0, #DMC_PWRDNCONFIG]

	ldr	r1, =0x00202400				/* MemControl	BL=4, 1 chip, DDR2 type, dynamic self refresh, force precharge, dynamic power down off */
	str	r1, [r0, #DMC_MEMCONTROL]


/*******************************************************************************************/

/* DMC1 initialization */  
  	ldr	r0, =APB_DMC_1_BASE  
  	ldr	r1, =0x00101000             		/* Phycontrol0 DLL parameter setting */  
  	str 	r1, [r0, #DMC_PHYCONTROL0]  
     
  	ldr 	r1, =0x00000086             		/* Phycontrol1 DLL parameter setting */ 
  	str 	r1, [r0, #DMC_PHYCONTROL1]  
  	ldr 	r1, =0x00101002             		/* PhyControl0 DLL on */ 
  	str 	r1, [r0, #DMC_PHYCONTROL0]  
  	ldr 	r1, =0x00101003             		/* PhyControl0 DLL start */
  	str 	r1, [r0, #DMC_PHYCONTROL0]  
    
    
    
wait_dll_locked_1:  
  	ldr 	r1, [r0, #DMC_PHYSTATUS]    		/* Load Phystatus register value */ 
  	and 	r2, r1, #0x7  
  	cmp 	r2, #0x7              		 	/* Loop until DLL is locked */  
  	bne 	wait_dll_locked_1  
    
  	and 	r1, #0x3fc0   
  	mov 	r2, r1, LSL #18  
  	orr 	r2, r2, #0x100000  
  	orr 	r2, r2, #0x1000  
        
 	orr 	r1, r2, #0x3                		/* Force Value locking */ 
  	str 	r1, [r0, #DMC_PHYCONTROL0]  
     

  	 /* settinf fot DDR2 */  
 	ldr 	r0, =APB_DMC_1_BASE  

 	ldr 	r1, =0x0FFF2010             		/* auto refresh off */  
  	str 	r1, [r0, #DMC_CONCONTROL]  

  	ldr 	r1, =DMC1_MEMCONTROL            	/* MemControl BL=4,2chip,DDR2 type,dynamic self refresh,force precharge,dynamic power down off */
  	str 	r1, [r0, #DMC_MEMCONTROL]  

  	ldr 	r1, =DMC1_MEMCONFIG_0           	/* MemConfig0 512MB config,8 banks,Mapping Method[12:15]0:linear, 1:linterleaved, 2:Mixed */ 
  	str 	r1, [r0, #DMC_MEMCONFIG0]  

  	ldr 	r1, =0xFF000000  
  	str 	r1, [r0, #DMC_PRECHCONFIG]  

  	ldr 	r1, =DMC1_TIMINGA_REF           	/* TimingAref 7.8us*133MHz=1038(0x40E), 100MHz=780(0x30C), 20MHz=156(0x9C), 10MHz=78(0x4) */ 
  	str 	r1, [r0, #DMC_TIMINGAREF]  

  	ldr 	r1, =DMC1_TIMING_ROW            	/* TimingRow  for @200MHz */ 
  	str 	r1, [r0, #DMC_TIMINGROW]  

  	ldr 	r1, =DMC1_TIMING_DATA          	 	/* TimingData CL=3 */ 
  	str 	r1, [r0, #DMC_TIMINGDATA]  

  	ldr 	r1, =DMC1_TIMING_PWR            	/* TimingPower */ 
  	str 	r1, [r0, #DMC_TIMINGPOWER]  


  	ldr 	r1, =0x07000000            		/* DirectCmd  chip0 Deselect */
  	str 	r1, [r0, #DMC_DIRECTCMD]  

	ldr 	r1, =0x01000000             		/* DirectCmd  chip0 PALL */ 
	str 	r1, [r0, #DMC_DIRECTCMD]  

	ldr 	r1, =0x00020000             		/* DirectCmd  chip0 EMRS2 */ 
	str 	r1, [r0, #DMC_DIRECTCMD]  

 	ldr 	r1, =0x00030000             		/* DirectCmd  chip0 EMRS3 */ 
  	str 	r1, [r0, #DMC_DIRECTCMD]  

  	ldr 	r1, =0x00010400             		/* DirectCmd  chip0 EMRS1 (MEM DLL on, DQS# disable) */ 
  	str 	r1, [r0, #DMC_DIRECTCMD]  

  	ldr 	r1, =0x00000542             		/* DirectCmd  chip0 MRS (MEM DLL reset) CL=4, BL=4 */ 
  	str 	r1, [r0, #DMC_DIRECTCMD]  

  	ldr 	r1, =0x01000000             		/* DirectCmd  chip0 PALL */  
  	str 	r1, [r0, #DMC_DIRECTCMD]  

  	ldr 	r1, =0x05000000            		/* DirectCmd  chip0 REFA */ 
  	str 	r1, [r0, #DMC_DIRECTCMD]  

  	ldr 	r1, =0x05000000            		/* DirectCmd  chip0 REFA */ 
  	str 	r1, [r0, #DMC_DIRECTCMD]  

 	ldr 	r1, =0x00000442             		/* DirectCmd  chip0 MRS (MEM DLL unreset) */  
  	str 	r1, [r0, #DMC_DIRECTCMD]  

  	ldr 	r1, =0x00010780             		/* DirectCmd  chip0 EMRS1 (OCD default) */ 
  	str 	r1, [r0, #DMC_DIRECTCMD]  

  	ldr 	r1, =0x00010400             		/* DirectCmd  chip0 EMRS1 (OCD exit) */  
  	str 	r1, [r0, #DMC_DIRECTCMD]  



  	ldr 	r1, =0x07100000             		/* DirectCmd  chip1 Deselect */  
  	str 	r1, [r0, #DMC_DIRECTCMD]  

  	ldr 	r1, =0x01100000             		/* DirectCmd  chip1 PALL */  
  	str 	r1, [r0, #DMC_DIRECTCMD]  

  	ldr 	r1, =0x00120000            	 	/* DirectCmd  chip1 EMRS2 */  
  	str 	r1, [r0, #DMC_DIRECTCMD]  

  	ldr 	r1, =0x00130000            		/* DirectCmd  chip1 EMRS3 */ 
  	str 	r1, [r0, #DMC_DIRECTCMD]  

  	ldr 	r1, =0x00110440             		/* DirectCmd  chip1 EMRS1 (MEM DLL on, DQS# disable) */  
  	str 	r1, [r0, #DMC_DIRECTCMD]  

  	ldr 	r1, =0x00100542            	 	/* DirectCmd  chip1 MRS (MEM DLL reset) CL=4, BL=4 */ 
  	str 	r1, [r0, #DMC_DIRECTCMD]  

  	ldr 	r1, =0x01100000             		/* DirectCmd  chip1 PALL */ 
  	str 	r1, [r0, #DMC_DIRECTCMD]  

  	ldr 	r1, =0x05100000             		/* DirectCmd  chip1 REFA  */
  	str 	r1, [r0, #DMC_DIRECTCMD]  

  	ldr 	r1, =0x05100000             		/* DirectCmd  chip1 REFA  */
  	str 	r1, [r0, #DMC_DIRECTCMD]  

  	ldr 	r1, =0x00100442             		/* DirectCmd  chip1 MRS (MEM DLL unreset) */
  	str 	r1, [r0, #DMC_DIRECTCMD]  

  	ldr 	r1, =0x00110780             		/* DirectCmd  chip1 EMRS1 (OCD default) */ 
  	str 	r1, [r0, #DMC_DIRECTCMD]  

  	ldr 	r1, =0x00110400             		/* DirectCmd  chip1 EMRS1 (OCD exit) */ 
  	str 	r1, [r0, #DMC_DIRECTCMD]  



	/* step27 */
  	ldr 	r1, =0x0FF02030             		/* ConControl auto refresh on */ 
  	str 	r1, [r0, #DMC_CONCONTROL]  

  	ldr 	r1, =0xFFFF00FF             		/* PwrdnConfig */     
  	str 	r1, [r0, #DMC_PWRDNCONFIG]  

  	ldr 	r1, =DMC1_MEMCONTROL        		/* MemControl BL=4, 2 chip, DDR2 type, dynamic self refresh, force precharge, dynamic power down off  */
  	str 	r1, [r0, #DMC_MEMCONTROL]  
  
  	mov 	pc, lr  







