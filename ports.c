#include <miniupnpc/miniupnpc.h>
#include "err.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * I'm not implementing UPnP functionality yet
 * partially because many consider UPnP bad from
 * a security point of view, so users who want to use
 * z-torrent will have to manually setup port forwarding
 * and send z-torrent the port as a command line parameter,
 * or in a configuration file. If and when I decide to 
 * implement UPnP, it'll be implemented in this file.
 */

