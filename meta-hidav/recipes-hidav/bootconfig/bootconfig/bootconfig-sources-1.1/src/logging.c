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
 *  HidaV boot configuration user space tooling
 *  logging abstraction.
 *  Maintained by thilo fromm <fromm@dresearch-fe.de>.
 */

#include "logging.h"

uint32_t bc_log_channels = ( BC_LOG_SYSLOG );
uint32_t bc_log_levels   =    LOG_MASK( LOG_EMERG ) 
                            | LOG_MASK( LOG_ALERT )
                            | LOG_MASK( LOG_CRIT )
                            | LOG_MASK( LOG_ERR ) 
                            | LOG_MASK( LOG_WARNING )
                            | LOG_MASK( LOG_NOTICE )
                            | LOG_MASK( LOG_INFO ) 
                            | LOG_MASK( LOG_DEBUG ) 
			  ;

void get_log_config ( uint32_t * channels, uint32_t * levels )
{
    *channels = bc_log_channels;
    *levels   = bc_log_levels;
}
/* -- */

void set_log_config ( uint32_t channels, uint32_t levels )
{
    bc_log_channels = channels;
    bc_log_levels   = levels;
}
/* -- */
