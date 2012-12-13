/*
 * Copyright 1996-1997 David J. McKay
 * Copyright 2005-2006 Erik Waling
 * Copyright 2006 Stephane Marchesin
 * Copyright 2007-2009 Stuart Bennett
 * Copyright 2009 Francisco Jerez
 * Copyright 2009 Marcin Kościelnicki
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <pciaccess.h>
#include <unistd.h>
#include "libio.h"

int arch;
void* regs;
void *fb;
void *ramin;


int NVGetArchitecture(volatile uint32_t *regs)
{
	int architecture = 0;

	/* We're dealing with >=NV10 */
	if ((regs[0] & 0x0f000000) > 0 )
		/* Bit 27-20 contain the architecture in hex */
		architecture = (regs[0] & 0xff00000) >> 20;
	/* NV04 or NV05 */
	else if ((regs[0] & 0xff00fff0) == 0x20004000)
		architecture = 0x04;

	return architecture;
}

//#define DIFFM(A,B,M) ((A%M + M - B%M)%M)
//#define MASK(field) ((0xffffffff >> (31 - ((1?field) - (0?field)))) << (0?field))
//#define XLATE(src, srclowbit, outfield) ((((src) >> (srclowbit)) << (0?outfield)) & MASK(outfield))

	struct pci_device* dev;
	struct pci_device_iterator* it;
int venid = 0x10de;

void init() {
	int ret;

	ret = pci_system_init();

	if(ret){
		fprintf(stderr,"Error calling pci_system_init().\n");

		exit(1);
	}

	struct pci_id_match nv_match = {venid, PCI_MATCH_ANY, PCI_MATCH_ANY, PCI_MATCH_ANY, 0x30000, 0xffff0000};
	it = pci_id_match_iterator_create(&nv_match);

	if(!it){
		fprintf(stderr,"Error calling pci_id_match_iterator_create().\n");

		pci_iterator_destroy(it);

		exit(1);
	}

	dev = pci_device_next(it);

	if(!dev){
		fprintf(stderr,"No devices found.\n");

		pci_iterator_destroy(it);
		pci_system_cleanup();

		exit(1);
	}

	ret = pci_device_probe(dev);

	if(ret){
		fprintf(stderr, "Error calling pci_device_probe().\n");

		pci_iterator_destroy(it);
		pci_system_cleanup();

		exit(1);
	}

	ret = pci_device_map_range(dev, dev->regions[0].base_addr, dev->regions[0].size, PCI_DEV_MAP_FLAG_WRITABLE,&regs);

	if(ret){
		fprintf(stderr, "Error calling pci_device_map_range().\n");

		pci_iterator_destroy(it);
		pci_system_cleanup();

		exit(1);
	}
	ret = pci_device_map_range(dev, dev->regions[1].base_addr, dev->regions[1].size, PCI_DEV_MAP_FLAG_WRITABLE,&fb);

	//ret = pci_device_map_range(dev, dev->regions[3].base_addr, dev->regions[3].size, PCI_DEV_MAP_FLAG_WRITABLE,&ramin);
/*	if(ret){
		fprintf(stderr, "Error calling pci_device_map_range().\n");

		pci_device_unmap_range(dev, regs, dev->regions[0].size);
		pci_iterator_destroy(it);
		pci_system_cleanup();

		exit(1);
	}*/
}

void fini() {
	//pci_device_unmap_range(dev, fb, dev->regions[1].size);
	//pci_device_unmap_range(dev, regs, dev->regions[0].size);
	pci_iterator_destroy(it);
	pci_system_cleanup();
}

