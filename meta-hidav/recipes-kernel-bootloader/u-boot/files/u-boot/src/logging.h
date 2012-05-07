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

#ifndef __BOOTINFO_LOG_H_
#define __BOOTINFO_LOG_H_

/*
 * Available log channels 
 */
#define BC_LOG_STDOUT   ( 1 << 0 )
#define BC_LOG_STDERR   ( 1 << 1 )
#define BC_LOG_SYSLOG   ( 1 << 2 )

void get_log_config ( uint32_t * channels, uint32_t * levels );
void set_log_config ( uint32_t   channels, uint32_t   levels );

/*
 * Log a message to the channels configured.
 * Printf formatting is supported. See "man syslog"
 * for severity levels.
 */
#define bc_log( severity, format, args... )  {                                              \
    extern uint32_t bc_log_channels, bc_log_levels;                                         \
    if ( bc_log_levels  & LOG_MASK( severity ) ) {                                          \
        if ( bc_log_channels & BC_LOG_STDOUT )                                              \
            printf( "%s::%s:%d " #format, __FILE__, __func__, __LINE__, ##args );           \
    }                                                                                       \
}

#endif /* __BOOTINFO_LOG_H_ */
