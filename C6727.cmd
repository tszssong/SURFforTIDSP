/****************************************************************************/
/*  C6727.cmd                                                               */
/*  Copyright (c) 2012 Texas Instruments Incorporated                       */
/*  Author: Rafael de Souza                                                 */
/*                                                                          */
/*    Description: This file is a sample linker command file that can be    */
/*                 used for linking programs built with the C compiler and  */
/*                 running the resulting .out file on a C6727               */
/*                 device.  Use it as a guideline.  You will want to        */
/*                 change the memory layout to match your specific C6xxx    */
/*                 target system.  You may want to change the allocation    */
/*                 scheme according to the size of your program.            */
/*                                                                          */
/****************************************************************************/

MEMORY
{
    IRAM:        o = 0x10000000  l = 0x00040000   /* 256kB Internal RAM */
    SDRAM:       o = 0x80000000  l = 0x08000000   /* 128MB External SDRAM */

    IMAGE		 o = 0x88000000  l = 0x07000000   /* 128MB Image  Data IN DDR2*/
    IMAGE1		 o = 0x8f000000  l = 0x00800000   /* 128MB Image  Data IN DDR2*/
    IMAGE2		 o = 0x8f800000  l = 0x00800000   /* 128MB Image  Data IN DDR2*/

	ASYNC:       o = 0x90000000  l = 0x10000000   /* 128MB External Async/Flash */
    }
                                               
SECTIONS                                       
{                                              
    .text          >  SDRAM
    .stack         >  SDRAM
    .bss           >  SDRAM
    .cio           >  SDRAM
    .const         >  SDRAM
    .data          >  SDRAM
    .switch        >  SDRAM
    .sysmem        >  SDRAM
    .far           >  SDRAM
    .args          >  SDRAM
    .ppinfo        >  SDRAM
    .ppdata        >  SDRAM
  
    /* COFF sections */
    .pinit         >  SDRAM
    .cinit         >  SDRAM
  
    /* EABI sections */
    .binit         >  SDRAM
    .init_array    >  SDRAM
    .neardata      >  SDRAM
    .fardata       >  SDRAM
    .rodata        >  SDRAM
    .c6xabi.exidx  >  SDRAM
    .c6xabi.extab  >  SDRAM

	.IMAGE_MEM     > IMAGE
    .IMAGE_MEM1     > IMAGE1
    .IMAGE_MEM2     > IMAGE2
}
