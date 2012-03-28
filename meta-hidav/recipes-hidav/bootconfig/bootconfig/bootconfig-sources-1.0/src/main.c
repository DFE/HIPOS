/*
*  Copyright (C) 2011, 2012 DResearch Fahrzeugelektronik GmbH   
*  Written and maintained by Thilo Fromm <fromm@dresearch-fe.de>
*                                                               
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License  
*  as published by the Free Software Foundation; either version 
*  2 of the License, or (at your option) any later version.     
*
*
* HidaV boot configuration user space tooling.
*/

#include "logging.h"
#include "lowlevel.h"

int main(int argc, char ** argv)
{
    struct bootconfig bc;

    bc_ll_init( &bc, "/dev/mtd1" );
    /* success! */
    return 0;
}
