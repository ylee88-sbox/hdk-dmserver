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
# $Id: dmsC_parser_proto.h,v 1.1.2.1 1997/08/13 23:06:50 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
#ifndef _IDMS_PARSER_PROTO_H
#define _IDMS_PARSER_PROTO_H

/*----------------------------------------------------------------------------*
 * Created by Sorin to store the prototypes used by the idms parser with ircs *
 * and idmb                                                                   *
 *                                                                            *
 * Warning: NOT SELF SUFFICIENT !                                             *
 *----------------------------------------------------------------------------*/

void dmsSeparateByWhatever(char c, char *is, char *o1, char *o2);
void myReplyFuncCB(String result,voidStar replyClientData);

#endif
