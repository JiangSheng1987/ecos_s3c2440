//=============================================================================
//
//      s3c2440x_nand_common_sp.inl
//
//      NAND flash support for the S3C2440X family - small page chips
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####   
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Ricky Wu (rickleaf.wu@gmail.com)
// Date:        2011-04-18
//
//####DESCRIPTIONEND####
//=============================================================================

// NOTE: This package is a bit of a layering violation at present.
// It depends on the BOARD driver (for read_data_bulk). As such it has to be a
// .inl file at present.

#include <cyg/nand/nand_device.h>
#include <cyg/nand/nand_ecc.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/plf_io.h>
#include <cyg/infra/cyg_ass.h>

#include <pkgconf/hal_arm_arm9_mini2440.h>
#include <cyg/devs/nand/s3c2440x_nand_common.h>

/* Hardware ECC ======================================================== */

static void s3c2440x_ecc_init_sp(cyg_nand_device *dev)
{
    cyg_uint32 reg;
    diag_printf("s3c2440x_ecc_init_sp\n");
    HAL_READ_UINT32(NFCONT,reg);
    reg = (reg & ~(1 << 5)) | S3C2440_NFCONT_INITECC;
    HAL_WRITE_UINT32(NFCONT,reg);
}

static void s3c2440x_ecc_calc_sp(cyg_nand_device *dev, const CYG_BYTE *dat, CYG_BYTE *ecc)
{
    // NB! This only works when ecc_stride == page size!
    // It's utterly dependent on the flash layout - that ECC is in
    // the first four bytes of the spare area - and that the chip driver
    // will reset itself to the top of the spare area in its read_finish().
    unsigned i, nread=0;
    const unsigned need = dev->ecc->ecc_size;
    CYG_BYTE ecctemp[need];

    for (i=0; i<CYG_NAND_OOB_MAX_APP_SLOTS; i++) {
        CYG_ASSERTC(dev->oob->ecc[i].pos == 0);
        //change_read_column(dev, (1 << dev->page_bits) + dev->oob->ecc[i].pos);
        int n_thisvec = dev->oob->ecc[i].len;
        read_data_bulk(dev, ecctemp, n_thisvec);
        // we ignore the read but the ECC unit picks it up. From the
        // datasheet: "Values in ECC_PR and ECC_NPR are valid and locked
        // until a new start condition occurs."
        nread += n_thisvec;
        if (nread >= need) break;
    }

    HAL_READ_UINT8(NFECC0,ecc[0]);
    HAL_READ_UINT8(NFECC1,ecc[1]);
    HAL_READ_UINT8(NFECC2,ecc[2]);
    HAL_READ_UINT8(NFECC3,ecc[3]);
}

static int s3c2440x_ecc_repair(cyg_nand_device *dev,
                            CYG_BYTE *dat, size_t nbytes,
                            CYG_BYTE *read_ecc, const CYG_BYTE *calc_ecc)
{
    /* The ECC unit in does the ECC check on read for us,
     * _provided_ we read strictly in the order (pagedata, ecc bytes).
     * See atmel_k9_read_finish() above.
     */
    if (read_ecc[0] == calc_ecc[0] &&
	 read_ecc[1] == calc_ecc[1] &&
	 read_ecc[2] == calc_ecc[2] &&
	 read_ecc[3] == calc_ecc[3])
	   return 0;
    diag_printf("s3c244x_nand_correct_data: not implemented\n");
    return -1;
}

CYG_NAND_ECC_ALG_HW(s3c2440x_ecc_smallpage, 512, 4,
        s3c2440x_ecc_init_sp, s3c2440x_ecc_calc_sp, s3c2440x_ecc_repair);

const cyg_nand_oob_layout s3c2440x_oob_smallpage = {
    .ecc_size = 4,
    .ecc = { { .pos=0, .len=4 } },
    .app_size = 10,
    .app = { { .pos =6, .len=10 } },
};

