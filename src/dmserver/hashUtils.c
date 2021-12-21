/*----------------------------------------------------------------------------*/
/*   Copyright (C) 1996-7 Intel Corporation. All Rights Reserved.             */
/*   Program: For hash table implementation to take care of proper response   */
/*            from DMserver for multiple clients for dmsCGetLastCellViewInfo()*/
/*            dmsCReportMessage() INTERS PIs.                                 */
/*   Authors:                                                                 */
/*   Shesha Krishnapura    shesha@scdt.intel.com            5-5134            */
/*   Vipul Lal             vipulX@hppfs.sc.intel.com        5-3404            */
/*   Software Platform Engineering, Computing Technology                      */
/*   Date: 1996/12/02                                                         */
/*----------------------------------------------------------------------------*/
/* $Id: hashUtils.c,v 1.1.1.1 1997/02/20 19:17:21 vipulX Exp $ */
/* $Log: hashUtils.c,v $
 * Revision 1.1.1.1  1997/02/20  19:17:21  vipulX
 * DMServer
 * */
#include <string.h>
#include "hashUtils.h"


INFO_NODE *Hash_Table[HASH_TABLE_SIZE];		/* Hash table */

int
hashGetHashIndex(char *client_id) {
   
    unsigned long cid;

    cid = (unsigned long) strtol(client_id, (char **)NULL, 36);
    return (cid % HASH_TABLE_SIZE);
}

INFO_NODE *
hashGetHashPtr(char *client_id) {

    return (Hash_Table[hashGetHashIndex(client_id)]);
}

int
hashGetNodePtr(char *client_id, INFO_NODE **hash_ptr_ptr) {

    if ((*hash_ptr_ptr = hashGetHashPtr(client_id)) == NULL)
       return(FALSE);

    while (*hash_ptr_ptr != NULL) {
       if (strncmp((*hash_ptr_ptr)->client_id, client_id, CLIENT_ID_SIZE) != 0)
          *hash_ptr_ptr = (*hash_ptr_ptr)->next_node;
       else 
          return(TRUE);
    }
    return(FALSE);			/* Should never come here */   
} 

int 
hashCreate(char *client_id) {

    INFO_NODE *hash_ptr;			
    int hash_index;

    if ((hash_ptr = hashGetHashPtr(client_id)) != NULL) {
					/* If not first node */
       while ((hash_ptr != NULL) && (hash_ptr->next_node != NULL)) {
	  if (strncmp(hash_ptr->client_id, client_id, CLIENT_ID_SIZE) != 0)
             hash_ptr = hash_ptr->next_node;
          else
	     return(FALSE);		/* Duplicate client ID case */
       }
       if (strncmp(hash_ptr->client_id, client_id, CLIENT_ID_SIZE) == 0)
	  return(FALSE);		/* Duplicate client ID case */
       
       hash_ptr->next_node = (INFO_NODE *) calloc(1, sizeof(INFO_NODE));
       hash_ptr->next_node->prev_node = hash_ptr;
       hash_ptr = hash_ptr->next_node;
    }
    else {				/* else if first node */
       hash_index = hashGetHashIndex(client_id);
       Hash_Table[hash_index] = (INFO_NODE *) calloc(1, sizeof(INFO_NODE));
       hash_ptr = Hash_Table[hash_index];
       hash_ptr->prev_node = NULL;
    }
    strncpy(hash_ptr->client_id, client_id, CLIENT_ID_SIZE);
#ifdef OH_MACH_LINUX
    hash_ptr->client_id[CLIENT_ID_SIZE-1] = '\0'; /* "" */
#else
    hash_ptr->client_id[CLIENT_ID_SIZE-1] = "";
#endif
    hash_ptr->next_node = NULL;
    return(TRUE);
}

int 
hashDelete(char *client_id) {

    INFO_NODE *hash_ptr, *release_hash_ptr;
    int hash_index;

    if (hashGetNodePtr(client_id, &hash_ptr)) {
       if ((hash_ptr->next_node == NULL) && (hash_ptr->prev_node == NULL)) {
	  hash_index = hashGetHashIndex(client_id);
	  Hash_Table[hash_index] = NULL;
	  free(hash_ptr);
	  return(TRUE);
       }
       if ((hash_ptr->next_node == NULL) && (hash_ptr->prev_node != NULL)) {
	  hash_ptr->prev_node->next_node = NULL;
	  free(hash_ptr);
	  return(TRUE);
       }
       if ((hash_ptr->next_node != NULL) && (hash_ptr->prev_node == NULL)) {
	  hash_index = hashGetHashIndex(client_id);
	  Hash_Table[hash_index] = hash_ptr->next_node;
	  hash_ptr->next_node->prev_node = NULL;
	  free(hash_ptr);
	  return(TRUE);
       }
       if ((hash_ptr->next_node != NULL) && (hash_ptr->prev_node != NULL)) {
	  hash_ptr->next_node->prev_node = hash_ptr->prev_node;
	  hash_ptr->prev_node->next_node = hash_ptr->next_node;
	  free(hash_ptr);
	  return(TRUE);
       }
    }
    else 
       return(FALSE);
}

int 
hashSetReportMsg(char *client_id) {

    INFO_NODE *hash_ptr;

    if (hashGetNodePtr(client_id, &hash_ptr)) {
       strncpy(hash_ptr->rep_message, dmsCReportMessage(), REP_MESSAGE_SIZE);
       return(TRUE);
    }
    else
       return(FALSE);
}

void
hashCopyLastCellViewInfo(DMSINFO *hash_info, DMSINFO *src_info) {

    hash_info->location = src_info->location;
    strcpy(hash_info->libPath, src_info->libPath);
    strcpy(hash_info->libName, src_info->libName);
    strcpy(hash_info->cellName, src_info->cellName);
    strcpy(hash_info->viewName, src_info->viewName);
    strcpy(hash_info->verName, src_info->verName);
    strcpy(hash_info->coUser, src_info->coUser);
    strcpy(hash_info->fullPathName, src_info->fullPathName);
}

int 
hashSetLastCellViewInfo(char *client_id) {

    DMSINFO *info;
    INFO_NODE *hash_ptr;

    if (hashGetNodePtr(client_id, &hash_ptr)) {
       dmsCGetLastCellViewInfo(&info);
       (void) hashCopyLastCellViewInfo(&(hash_ptr->dm_info_struct), info);
       return(TRUE);
    }
    else
       return(FALSE);
}

char *
hashGetReportMsg(char *client_id) {

    INFO_NODE *hash_ptr;

    if (hashGetNodePtr(client_id, &hash_ptr))
       return(hash_ptr->rep_message); 
    else
       return(NULL);
}

int 
hashGetLastCellViewInfo(char *client_id, DMSINFO **info) {

    INFO_NODE *hash_ptr;

    if (hashGetNodePtr(client_id, &hash_ptr)) {
       *info = &(hash_ptr->dm_info_struct);
       return(TRUE); 
    }
    else
       return(FALSE);
}
