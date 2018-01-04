/****************************************************************************/
/*  C6748.cmd                                                               */
/*  Copyright (c) 2010 Texas Instruments Incorporated                       */
/*  Author: Rafael de Souza                                                 */
/*                                                                          */
/*    Description: This file is a sample linker command file that can be    */
/*                 used for linking programs built with the C compiler and  */
/*                 running the resulting .out file on a C6748               */
/*                 device.  Use it as a guideline.  You will want to        */
/*                 change the memory layout to match your specific C6xxx    */
/*                 target system.  You may want to change the allocation    */
/*                 scheme according to the size of your program.            */
/*                                                                          */
/****************************************************************************/

MEMORY
{
    DSPL2ROM     o = 0x00700000  l = 0x00100000   /* 1MB L2 Internal ROM */
    DSPL2RAM     o = 0x00800000  l = 0x00040000   /* 256kB L2 Internal RAM */
    DSPL1PRAM    o = 0x00E00000  l = 0x00008000   /* 32kB L1 Internal Program RAM */
    DSPL1DRAM    o = 0x00F00000  l = 0x00008000   /* 32kB L1 Internal Data RAM */
    SHDSPL2ROM   o = 0x11700000  l = 0x00100000   /* 1MB L2 Shared Internal ROM */
    SHDSPL2RAM   o = 0x11800000  l = 0x00040000   /* 256kB L2 Shared Internal RAM */
    SHDSPL1PRAM  o = 0x11E00000  l = 0x00008000   /* 32kB L1 Shared Internal Program RAM */
    SHDSPL1DRAM  o = 0x11F00000  l = 0x00008000   /* 32kB L1 Shared Internal Data RAM */
    EMIFACS0     o = 0x40000000  l = 0x20000000   /* 512MB SDRAM Data (CS0) */
    EMIFACS2     o = 0x60000000  l = 0x02000000   /* 32MB Async Data (CS2) */
    EMIFACS3     o = 0x62000000  l = 0x02000000   /* 32MB Async Data (CS3) */
    EMIFACS4     o = 0x64000000  l = 0x02000000   /* 32MB Async Data (CS4) */
    EMIFACS5     o = 0x66000000  l = 0x02000000   /* 32MB Async Data (CS5) */
    SHRAM        o = 0x80000000  l = 0x00020000   /* 128kB Shared RAM */
    DDR2         o = 0xC0000000  l = 0x08000000   /* 128MB DDR2 Data */
    IMAGE		 o = 0xC8000000  l = 0x07000000   /* 128MB Image  Data IN DDR2*/
    IMAGE1		 o = 0xCf000000  l = 0x00800000   /* 128MB Image  Data IN DDR2*/
    IMAGE2		 o = 0xCf800000  l = 0x00800000   /* 128MB Image  Data IN DDR2*/
}                                              
                                               
SECTIONS                                       
{                                              
    .text          >  DSPL2RAM
    .stack         >  DSPL2RAM
    .bss           >  DDR2
    .cio           >  DDR2
    .const         >  DDR2
    .data          >  DDR2
    .switch        >  DDR2
    .sysmem        >  DDR2
    .far           >  DDR2
    .args          >  DDR2
    .ppinfo        >  DDR2
    .ppdata        >  DDR2
  
    /* COFF sections */
    .pinit         >  DDR2
    .cinit         >  DDR2
  
    /* EABI sections */
    .binit         >  DDR2
    .init_array    >  DDR2
    .neardata      >  DDR2
    .fardata       >  DDR2
    .rodata        >  DDR2
    .c6xabi.exidx  >  DDR2
    .c6xabi.extab  >  DDR2

    .IMAGE_MEM     > IMAGE
    .IMAGE_MEM1     > IMAGE1
    .IMAGE_MEM2     > IMAGE2
}
