/*
* ***************************************************************************
* Copyright (C) 2016 Marvell International Ltd.
* ***************************************************************************
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* Neither the name of Marvell nor the names of its contributors may be used
* to endorse or promote products derived from this software without specific
* prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
* OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
***************************************************************************
*/

#include <plat_marvell.h>


/* MMU entry for internal (register) space access */
#define MAP_DEVICE0	MAP_REGION_FLAT(DEVICE0_BASE,			\
					DEVICE0_SIZE,			\
					MT_DEVICE | MT_RW | MT_SECURE)

/*
 * Table of regions for various BL stages to map using the MMU.
 */
#if IMAGE_BL1
const mmap_region_t plat_arm_mmap[] = {
	MARVELL_MAP_SHARED_RAM,
	MAP_DEVICE0,
	{0}
};
#endif
#if IMAGE_BL2
const mmap_region_t plat_arm_mmap[] = {
	MARVELL_MAP_SHARED_RAM,
	MAP_DEVICE0,
	MARVELL_MAP_NS_DRAM1,
	{0}
};
#endif

#if IMAGE_BL2U
const mmap_region_t plat_arm_mmap[] = {
	MAP_DEVICE0,
	{0}
};
#endif

#if IMAGE_BLE
const mmap_region_t plat_arm_mmap[] = {
	MAP_DEVICE0,
	{0}
};
#endif

#if IMAGE_BL31
const mmap_region_t plat_arm_mmap[] = {
	MARVELL_MAP_SHARED_RAM,
	MAP_DEVICE0,
	MARVELL_MAP_NS_DRAM1,
	{0}
};
#endif
#if IMAGE_BL32
const mmap_region_t plat_arm_mmap[] = {
	MAP_DEVICE0,
	{0}
};
#endif

ARM_CASSERT_MMAP


/*
 * Following is the default definition that always
 * returns BL2 image details.
 */
image_desc_t bl2_img_desc;
image_desc_t *bl1_plat_get_image_desc(unsigned int image_id)
{
	bl2_img_desc.image_id = BL2_IMAGE_ID;
	bl2_img_desc.image_info.h.version = VERSION_1;
	bl2_img_desc.image_info.h.attr = SET_EXEC_STATE(EXECUTABLE);
	bl2_img_desc.image_info.image_base = BL2_BASE;
	bl2_img_desc.ep_info.h.attr = SET_SEC_STATE(SECURE);
	bl2_img_desc.ep_info.pc = BL2_BASE;

	return &bl2_img_desc;
}

