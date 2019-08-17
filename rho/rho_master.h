/************************************************************************
 *  File: rho_master.h
 *  Group: Rho Core
 ***********************************************************************/
 
#ifndef rho_master_h
#define rho_master_h

#if defined __linux || defined __APPLE__
#include "rho_core.h"
#else
#include "rho_client.h"
#endif

#endif /* rho_master_h */
