#include <miniupnpc/miniupnpc.h>
#include "err.h"
#include <stdlib.h>
#include <stdio.h>

// returns 0 on success
int init_ports() {
	struct UPNPDev *dev_list, *tmp;
	struct UPNPUrls *urls;
	struct IGDdatas *data;
	
	dev_list = upnpDiscover(0, NULL,
             NULL, 0, 0, NULL);
	
	if (dev_list == NULL) {
		fprintf(stderr, "Error: UPnP devices couldn't be found!\n");
		return -1;
	}
	
	if (UPNP_GetValidIGD(dev_list,
	

	return 0;

}
