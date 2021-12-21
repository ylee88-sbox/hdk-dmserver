/*
############## Intel Corporation Confidential information. ############
#                                                                     #
#              Intel  confidential		                      #
#                                                                     #
# This listing is supplied under the terms of a  license  agreement   #
# with Intel Corporation and may not be copied nor disclosed except   #
# in accordance with the terms of the agreement.                      #
#                                                                     #
############## Intel Corporation Confidential information. ############

#######################################################################
# RCS information:                                                    #
# $Id: voTimeCorrection.h,v 1.1.2.1 1998/05/08 21:46:48 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/

#ifndef _VO_TIME_CORRECTION_H
#define _VO_TIME_CORRECTION_H

/*

    THIS FILE HAS THE PURPOSE TO ELIMINATE THE CONFLICT BETWEEN 

    /usr/include/sys/time.h                 and 
    $CADENCE/tools/dfII/include/voTime.h

    for rs6000_4.1.4

*/


#include <time.h> /* for clock_t and time_t */
   extern clock_t      voClock(void);
   extern time_t       voFastTime(void);

/* prevent future inclusions of voTime.h */

#define voTime_h

/* prevent future inclusions of voUnistd.h */

#define voUnistd_h

#endif
