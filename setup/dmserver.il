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
# $Id: dmserver.il,v 1.1 1996/02/29 19:21:10 athfw Exp $
#                                                                     #
#######################################################################
#######################################################################
*/

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; launch the DesignSync form of idmSlave (unless we're told not to OR the Athena version of
;;   the iDMS utilities appears to be in use...)
;;
;; then see if the user needs the idmb DFII interface
;;
;; then define some Genesys specific (?) Cadence V4.4/5.0 library meta-data dumping procedures...
;;

;; replacing the test for idms tool with always true
;; just replace the "t" with the complicated expression below
;; (exists idmsTool (parseString (getShellEnvVar "SETUP_TOOLS")) (rexMatchp "\\<idms\\>" idmsTool))
;;
;; Right now the condition is always true so the unless block will never be executed
;; I will remove it alltogether in the next big release of dmserver
;;
;; Sorin 11/6/2008

(unless (or  (getShellEnvVar "NO_DS_IDMSLAVE") t
        )

	/*
	** 
	** The following code
	**           - defines the mechanism by which the child process idmSlave is started and terminated
	**           - defines the SKILL API that enables opus to communicate with / drive idmSlave
	**
	** idmSlave is an idms.a linked process that is similar in function to that of idmb.  idmSlave and
	## the dmsSkill* API enable opus based applications to perform idms defined/constrained operations
	** against managed data.
	**
	**		Author:			Daaman Hejmadi, Frameworks
	**/
	/*-----------------------------------------------------------------*/
	/* Main difference between the 4.3 and 4.4 code:
	** In 4.3: the list props were being set and got thru dfII skill
	** In 4.4: the list props will be got from the idmsSlave
	**						-sh 3/17/00
	**/

	/* procedure to send a message to the idmSlave
	*/
	procedure(idmSendMsgToSlave(msg)
		prog( (message)
			unless( idmsSlaveIsAlive 
					if( idmsSlaveInWait then
						printf("-I- dmserver.il: idmSendMsgToSlave: idmSlave is in wait ...\n")
						return()
				else 
						printf("-I- dmserver.il: idmSendMsgToSlave: Restarting idmSlave ...\n")
					(ipcKillProcess idmsSlaveId)
					dmsSkillTerminateSlave()
					dmsSkillStartSlave() 
					return()
				)
			)
			unless( msg
				return()
			)
			
			sprintf(message "%s\n" msg)
			when( idmsDebugOn
				printf("-D- dmserver.il: idmSendMsgToSlave: \"%s\"\n" message)
			)
			return( (ipcWriteProcess idmsSlaveId message) )
		)
	)

	/*****
	****** procedure to get a message from the idmSlave
	****/

	procedure(idmGetMsgFromSlave(@optional synch)
		prog( (data msgStore numTimeOuts continueRead indx tmp)
			
			unless( synch || idmsSlaveIsAlive 
				printf("-I- dmserver.il: idmGetMsgFromSlave: idmSlave is not alive\n")
				return("f")
			)

			continueRead = t
			numTimeOuts = idmsNumTimeOuts
			msgStore = ""
			while( continueRead
			;; follow a simple algo - read till you find that ":endSlave"
			;; is a part of the string - then you are done
				data = (ipcReadProcess idmsSlaveId idmsBlockPeriod)
				--numTimeOuts
				if( data && strlen(data) > 0 then
					when( idmsDebugOn
						printf("-D- dmserver.il: idmGetMsgFromSlave: \"%s\"\n" data)
					)
					msgStore = strcat(msgStore data)
				else
					if(numTimeOuts > 0 then
						printf(
			"-I- dmserver.il: idmGetMsgFromSlave: no response yet from idmSlave, will look again up to %d more times\n"
						numTimeOuts)
					)
				)
				indx = nindex(msgStore ":endSlave")
				if( indx then
					msgStore = substring(msgStore 1 (indx-1))
					; get the front of the string
					tmp = index(msgStore "slave:")
					if( tmp then
						msgStore = substring(tmp 7)
						return(msgStore)
					else
						continueRead = nil	
					)
				else 
					when( numTimeOuts <= 0
						continueRead = nil
					)
				)
			)
			printf("-I- dmserver.il: idmGetMsgFromSlave: idmSlave message not received\n")
			printf("-I- dmserver.il: idmGetMsgFromSlave: check if serv and idmSlave processes are running\n")
			return()
	))

	/***
	*** The Handler routines for the idmSlave process
	***/
			
	/* if idmSlave terminates - simply set the idmsSlaveIsAlive flag to nil */
	procedure( idmPOSThandler( childId exitStatus )
				(setq idmsSlaveIsAlive nil)
				printf("-I- dmserver.il: idmPOSThandler: idmSlave Terminated\n")
				t
	)

	/* if idmSlave has an error simply print the error */
	procedure( idmERRhandler( childId childData )
		when( childId == idmsSlaveId 
				printf("-I- dmserver.il: idmERRhandler:  %s\n" childData)
		)
		t
	)

	/* the io handler - the main job is to simply mention that
	** the call completed successfully
	** 	 It is not being used - since we are using the synch mode
	** of operation.
	*/
	procedure( idmIOhandler( childId childData )
		prog( ()
			when( childId == idmsSlaveId
				when( idmsDebugOn
					printf("-D- dmserver.il: idmIOhandler: %s\n" childData)
				)
				/* get the data into a list form */
				when( idmsSlaveInWait 
					 when( substring(childData 7 5) == "ready" 
							idmsSlaveIsAlive = t
							idmsSlaveInWait = nil

						; set old context  if any
						when( idmsCurrentTool
						dmsSkillSetTool(idmsCurrentTool)
						)
								 )
							)
				return(t)
			)
			return()
		)
	)
	/*****
	******  	End of Handler routines 
	*****/

	/* internal routine that will build a string given a list of 
	** arguments - this is the string that is to be sent to 
	** the slave process for execution
	*/
	procedure(idmBuildString(argList)
		prog( (x length outString)

		outString = ""

		; the seperator char for this is a colon :
		foreach(x argList
			case(typep(x)
				('list   sprintf(x "%L" x))
				('flonum sprintf(x "%L" x)) 
				('fixnum sprintf(x "%L" x)) 
			)
			length = strlen(x)
			when( (strlen(outString)+length+11 >= 8191)
				return()
			)
			sprintf(outString "%s:%d:%s" outString length x)
		)
		return(outString)
	))


	/* internal routine that is called by the slave using the
	** skill pipes - it simply collects a string and stores
	** the string in a variable
	*/
	/***************************************************************************
	* not mecessary in 4.4x 			-sh 1/20/00
	procedure(idmCollectString(argList)
		prog( (num str)
			
		when( idmsDebugOn
						printf("-D- dmserver.il: idmCollectString: idmCollectString received argList=%L\n" argList)
		)
		unless( length(argList) == 2
			|| numberp(car(argList))
			|| stringp(cadr(argList))
			return()
		)
		num = car(argList)
		str = cadr(argList)
		

		if( num == 1
		then	idmsSearchPathString = ""
		else 
				 unless( num == idmsSearchPathPacketNumber+1
			return()	
				 )
		)

		sprintf(idmsSearchPathString "%s %s" idmsSearchPathString str)
		idmsSearchPathPacketNumber = num

		when( idmsDebugOn
			printf("-D- dmserver.il: idmCollectString: Received packet %d string =  %s\n" num str)
			printf("-D- dmserver.il: idmCollectString: Total string %s\n" idmsSearchPathString)
		) 

		return(t)
	))
	***************************************************************************/

	/****************
	*******        SKILL PUBLIC INTERFACE BEGINS HERE
	****************/

	/* idmSlave Control routines */

	;;
	;; procedure to spawn the idmSlave process - a separate 'C' process that can parse dms.pth
	;;	files and then perform searches against the defined paths and/or various DM actions,
	;;	all under the control of DFII...
	;;	We did NOT want to build a SKILL based parser for dms.pth files
	;;	(and as building a custom "blend" of DFII is no longer possible and even when it was
	;;	presented a validation nightmare) we elected to construct a separate 'C' code process (idmSlave)
	;;
	;; As of the ~ww12f release of dmserver, the individual releases of idms, idmb and dmserver have been merged
	;; BUT there is a subtle difference between HPUX and Linux builds... 
	;;
	;; On the hpux platform under $DMSERVER_BIN_DIR one will find
	;;
	;; $DM_SYSTEM :== DFII43 -> idmSlave               - for use with DFII v4.3
	;; $DM_SYSTEM :== SYNC   -> idmSlave_sync_wrapper  - for use with DesignSync
	;; $DM_SYSTEM :== NOVEND -> idmSlave_novendor      - for use with none of the above
	;;
	;; On the linux platform under $DMSERVER_BIN_DIR one will find
	;;
	;; $DM_SYSTEM :== DFII43 -> <<<ERROR>>>
	;; $DM_SYSTEM :== SYNC   -> idmSlave_sync_wrapper  which will call  idmSlave  - for use with DesignSync
	;; $DM_SYSTEM :== NOVEND -> idmSlave_novendor                                 - for use with none of the above
	;;
	procedure(dmsSkillStartSlave(@optional synch)
		prog( (ready command tmpPri tmpFn session CAD_ROOT DMSERVER_VER IDMS SYNC_DIR SYNC_ARCH exeName)	

			; if slave already exists do nothing 
			when( idmsSlaveIsAlive 
				return(t)
			)

			/* get the environment variable for where the idmSlave process
			** is stored
			*/
			
			(if (and	(setq CAD_ROOT (getShellEnvVar "CAD_ROOT"))
								(setq DMSERVER_VER (getShellEnvVar "DMSERVER_VER")))
			then
				(setq IDMS (strcat CAD_ROOT "/dmserver/" DMSERVER_VER "/bin"))
			else
				printf("**dmsSkillStartSlave: Unable to determine value for CAD_ROOT or DMSERVER_VER.  Adjust tool environment and rerun\n")
				return()
			)

			(if (equal "SYNC" (or (getShellEnvVar "DM_SYSTEM") ""))
			then
				(unless	(setq SYNC_DIR (getShellEnvVar "SYNC_DIR"))
					printf("**dmsSkillStartSlave: Env Variable SYNC_DIR not set, set it and rerun\n")
					return()
				)
				(unless	(setq SYNC_ARCH (getShellEnvVar "SYNC_ARCH"))
					printf("**dmsSkillStartSlave: Env Variable SYNC_ARCH not set, set it and rerun\n")
					return()
				)
				(setq exeName "idmSlave_sync_wrapper")
				
			else
				printf("**dmsSkillStartSlave: Cannot start DFIIv43 or novendor idmSlave process\n") 
				return()
			)

			(setq tmpPri (ipcGetPriority) )

			;; Its critical that the following DMS helper processes run at the highest priority
			(ipcSetPriority 0)

			(sprintf dbSecure "%s/unified/secure/dbac" (getShellEnvVar "MOUNTS"))
			(if (isFile dbSecure)
			then
				sprintf(command "%s %s/%s" dbSecure IDMS exeName)
			else
				sprintf(command "%s/%s" IDMS exeName)
			)

			;; NOTE: ipcGetPid returns PID of THIS (the parent) process not that of a child process...

			(if (and (boundp 'idmsDebugOn) idmsDebugOn)
			then
				(setq logFile (sprintf nil "/tmp/idmSlave.log_%d.%s" (ipcGetPid) (buildString (parseString (getCurrentTime)) "")))
				(printf "-D- dmserver.il: dmsSkillStartSlave: idmSlave log in %s\n" logFile)
			else
				(setq logFile "")
			)
			
			; spawn off slave with appropriate handlers
			(printf "-I- dmserver.il: starting %s ...\n" command)
			(setq idmsSlaveId  (ipcBeginProcess
						command          	; command
						""               	; local host
						"idmIOhandler"    	; io handler
						"idmERRhandler"   	; error handler
						"idmPOSThandler"  	; post function
						logFile          		; log file if debugging (see note above)
			))

			(ipcSetPriority tmpPri)

			unless( synch
							idmsSlaveInWait = t
						return(t)
			)

			; Wait for the slave process to start
			(ipcWaitForProcess idmsSlaveId)

			; Now we have to wait for the slave to initialize itself
			(setq ready (parseString (idmGetMsgFromSlave t) " "))
			(when (equal (car ready) "ready")
				(when (greaterp (length ready) 1)
					;; starting with the DFII v4.4.* aware opus44ww34 release we now see "ready name-of-slaves-cds.lib-file"
					(setq idmsSlaveCdsLibFile (cadr ready))
				)

				;; Tell Cadence to tell idmSlave to exit when Cadence exits (so that idmSlaves cds.lib file is removed)
				(regExitBefore 'dmsSkillTerminateSlave)

				(setq idmsSlaveIsAlive t)
				(return t)
			)

			return(nil)
		)
	)

	procedure(dmsSkillTerminateSlave()
		prog( (msg)
		when( idmsSlaveIsAlive
			msg = idmBuildString(list("Quit"))
			unless( idmSendMsgToSlave(msg)

				;; Tell Cadence that it doesn't need to tell idmSlave to exit when Cadence exits (so that idmSlaves cds.lib file is removed)
				(remExitProc 'dmsSkillTerminateSlave)

				return()
			)
		)

		;; give child the opertunity to exit gracefully
		(hiSleep 2)

		when( (ipcIsAliveProcess idmsSlaveId) 
			;; this will NOT allow the child to clean up gracefully so we leave the
			;; $WORK_AREA_ROOT_DIR/rundir/.opus_<pid>_cds.lib file  But... we know its name and can manually nuke it.
			(ipcKillProcess idmsSlaveId)
			(when	(and	(boundp 'idmsSlaveCdsLibFile)
					(stringp idmsSlaveCdsLibFile)
					(isFile idmsSlaveCdsLibFile)
				)
				(deleteFile idmsSlaveCdsLibFile)
			)
		)
		idmsCurrentTool = nil
				return(t)
	))

	procedure(dmsSkillIsSlaveAlive()
		prog( (message reply)
			when( idmsSlaveIsAlive
				message = idmBuildString(list("Alive"))
				unless( idmSendMsgToSlave(message)
					return()
				)
				reply = idmGetMsgFromSlave()
				unless( reply
					return()
				)
				when( reply == "true"
					return(t)
				)
			)
			return()
		)
	)
	/* end of control routines */

	/* general search path procedures */

	procedure(dmsSkillReparseDMS(@optional fileName)
		prog( (message reply)
			if( fileName then
				message = idmBuildString(list("Reparse" fileName))
			else 
				message = idmBuildString(list("Reparse" "#"))
			)
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave() 	
			if( reply == "f" then 
				return()
			else 
				printf("Reparsed the dmspath and changed search path\n")  
				return(t)
			)
		)
	)

	procedure(dmsSkillSetSearchPath()
		errset(error("dmsSkillSetSearchPath is not available in 4.4.x\n") t)
	)


	procedure(dmsSkillSetTool(toolName)
		prog( (message reply)
						when( idmsCurrentTool==toolName return(t))
			message = idmBuildString(list("SetTool" toolName))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
			if( reply == "f" then 
				return()
			else 
				printf("-I- dmserver.il: dmsSkillSetTool: idmSlave's search path now equal to that of tool:%s \n" toolName)
				; set global var to remember the current tool
				idmsCurrentTool = toolName
				return(t)
			)
		)	
	)

	/* list Attribute PI procedures */
	/* op 1 = set */
	/* op 2 = get */
	/* op 3 = del */
	/*****************************************************************
	For 4.4.x: idmListAttrOpFunc and idmListAttrFunc have been deleted
	because they are nolonger necessary. The lists are converted into
	strings by idmBuildString and sent to idmSlave where they are being
	stored as strings as Sync meta-data. While retrieving list attrs, 
	this skill parent receives strings from the idmSlave child.
	*****************************************************************/


	/* Attribute PI procedures */
	procedure(dmsSkillSetAttribute(libName cellName viewName 
		versionName attrName attrType attrValue @optional toolName)
		prog( (message lib ver reply)

			if( libName then
				lib = libName
			else
				lib = "#"
			)
			if( versionName then
				ver = versionName
			else
				ver = "#"
			)
			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message = idmBuildString(list("SetAttr" 
				lib cellName viewName ver attrName attrType attrValue))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
			if( reply == "f" then 
				return()
			else 
				return(t)
			)
		)
	)

	procedure(dmsSkillGetAttribute(libName cellName viewName 
		versionName attrName @optional toolName)
		prog( (message lib ver reply tempVar)
			if( libName then
				lib = libName
			else
				lib = "#"
			)
			if( versionName then
				ver = versionName
			else
				ver = "#"
			)
			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message = idmBuildString(list("GetAttr" 
				lib cellName viewName ver attrName))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
			if( reply == "f" then 
				return()
			else
				; parse the reply and return a list
				tempVar = parseString(reply " ")
				if(car(tempVar) == "list" then
					reply = list(car(tempVar) cdr(tempVar))
				)
				reply = list(car(tempVar) buildString(cdr(tempVar) " "))
				return(reply)
			)
		)	
	)

	procedure(dmsSkillDeleteAttribute(libName cellName viewName 
		versionName attrName @optional toolName)
		prog( (message lib ver reply tempVar)
			if( libName then
				lib = libName
			else
				lib = "#"
			)
			if( versionName then
				ver = versionName
			else
				ver = "#"
			)
			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message = idmBuildString(list("DelAttr" 
				lib cellName viewName ver attrName))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
			if( reply == "t" then
				return(t)
			else
				return()
			)
		)	
	)


	/* core idms procedures */
	procedure(dmsSkillCreateCellView(cellName viewName @optional toolName)
		prog( (message reply)

			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message = idmBuildString(list("CreateCV" cellName viewName))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
			if( reply == "f" then 
				return()
			else 
				return(reply)
			)
		)	
	)


	procedure(dmsSkillCreateUnixFile(cellName viewName @optional toolName)
		prog( (message reply)
			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message = idmBuildString(list("CreateUxF" cellName viewName))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
			if( reply == "f" then 
				return()
			else 	
				return(reply)
			)
		)
	)

	procedure(dmsSkillReadonlyCellViewType(cellName viewType @optional toolName)
		prog( (message reply)

			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message =  idmBuildString(list("ReadCVType" cellName viewType))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
			if( reply == "f" then 
				return()
			else 	
				return(parseString(reply " "))
			)
		)
	)

	procedure(dmsSkillCheckoutCellView(libName cellName viewName versionName
		@optional toolName)
		prog( (message lib ver reply)
			if( libName then
				lib = libName
			else
				lib = "#"
			)
			if( versionName then
				ver = versionName
			else
				ver = "#"
			)

			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message = idmBuildString(list("Checkout" 
							lib cellName viewName ver))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
			if( reply == "f" then 
				return()
			else 
				return(reply)
			)
		)
	)

	procedure(dmsSkillLoadCellView(cellName viewName @optional toolName)
		prog( (message reply)

			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message = idmBuildString(list("Load" cellName viewName))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
			if( reply == "f" then 
				return()
			else 
				return(reply)
			)
		)
	)

	procedure(dmsSkillEditCellView(libName cellName viewName versionName
		@optional toolName)
		prog( (message lib ver reply)
			if( libName then
				lib = libName
			else
				lib = "#"
			)
			if( versionName then
				ver = versionName
			else
				ver = "#"
			)

			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message = idmBuildString(list("Edit"
				lib cellName viewName ver))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
			if( reply == "f" then 
				return()
			else 
				return(reply)	
			)
		)
	)

	procedure(dmsSkillReadOrEditByPolicy(cellName viewName @optional toolName)
		prog( (message reply)

			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message = idmBuildString(list("ReadEdit"
						cellName viewName))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
			if( reply == "f" then 
				return()
			else 
				return(reply)	
			)
		)
	)


	procedure(dmsSkillReadonlyCellView(cellName viewName @optional toolName)
		prog( (message reply)

			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message = idmBuildString(list("ReadOnly"
						cellName viewName))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
			if( reply == "f" then 
				return()
			else 
				return(reply)	
			)
		)
	)

	procedure(dmsSkillSaveCellView(cellName viewName @optional toolName)
		prog( (message reply)

			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message =  idmBuildString(list("Save" cellName viewName))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
			if( reply == "f" then 
				return()
			else 
				return(reply)
			)
		)
	)

	procedure(dmsSkillCheckinCellView(libName cellName viewName versionName
		advanceFlag logMessage path @optional toolName)
		prog( (message lib ver adv log p reply)
			if( libName then
				lib = libName
			else
				lib = "#"
			)
			if( versionName then
				ver = versionName
			else
				ver = "#"
			)
			if( advanceFlag then
				adv = "1"
			else
				adv = "0"
			)
			if( logMessage then
				log = logMessage	
			else
				log = "#"
			)
			if( path then
				p = path	
			else
				p = "#"
			)

			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message = idmBuildString(list("Checkin" 
						lib cellName viewName ver adv log p))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
			if( reply == "f" then 
	;			println("Checkin failed")
				return()
			else 
	;			println("Checkin succeeded")
				return(t)
			)
		)
	)

	procedure(dmsSkillUpdateCellView(libName cellName viewName 
		advanceFlag logMessage @optional toolName)
		prog( (message lib adv log reply)
			if( libName then
				lib = libName
			else
				lib = "#"
			)
			if( advanceFlag then
				adv = "1"
			else
				adv = "0"
			)
			if( logMessage then
				log = logMessage	
			else
				log = "#"
			)

			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message = idmBuildString(list("Update" 
					lib cellName viewName adv log))
			unless( idmSendMsgToSlave(message)
				return()
			)			
			reply = idmGetMsgFromSlave()
			if( reply == "f" then 
	;			println("Update failed")
				return()
			else 
				return(reply)
			)
		)
	)

	procedure(dmsSkillInstallCellView(cellName viewName logMessage
		@optional toolName)
		prog( (message log reply)

			if( logMessage then
				log = logMessage	
			else
				log = "#"
			)
			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message = idmBuildString(list("Install" cellName viewName log))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
			if( reply == "f" then 
				return()
			else 
				return(reply)
			)
		)
	)

	procedure(dmsSkillCancelCheckoutCellView(libName cellName viewName versionName
		@optional toolName)
		prog( (message lib ver reply)
				if( libName then
					lib = libName
				else
					lib = "#"
				)
				if( versionName then
					ver = versionName
				else
					ver = "#"
				)
				
				when( toolName
					unless( toolName == idmsCurrentTool
						unless( dmsSkillSetTool(toolName)
							return()
						)
					)
				)

				message = idmBuildString(list("UnCheckout"
						lib cellName viewName ver))
						unless( idmSendMsgToSlave(message)
					return()
				)
				reply = idmGetMsgFromSlave()
				if( reply == "f" then
	;				println("Cancel Checkout failed")
					return()
				else
	;				println("Cancel Checkout succeeded")
					return(t)
				)
		)
	)

	procedure(dmsSkillInfoCellView(searchLib cellName viewName
		@optional toolName)
		prog( (message reply replyList)
			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message = idmBuildString(list("Info" searchLib cellName viewName))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
							if( reply=="f" then
									return()
							else
					replyList = parseString(reply " ")
									case( car(replyList)
										(("0" "1") replyList=list(car(replyList) "" 
																cadr(replyList) caddr(replyList)  "" "" 
																"" cadddr(replyList) "")
										)
										("2" replyList=list(car(replyList) "" cadr(replyList) 
													caddr(replyList) "" "" cadddr(replyList) 
													car(cddddr(replyList)) "")
										)
										(("3" "4" "5" "6" "7") nil)
									)
									return(replyList)
							)
		)
	)

	procedure(dmsSkillVersionNumber()
		prog( (message reply)
			message = idmBuildString(list("Version"))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
			return(reply)
		)
	)

	procedure(dmsSkillGetWorkingDir(viewName @optional toolName)
		prog( (message reply)

			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message = idmBuildString(list("WorkDir" viewName))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
						if( reply == "f" /* then */
					return()
			/* else */
									return(reply)
							)
		)
	)

	procedure(dmsSkillGetWorkingLib(viewName @optional toolName)
		prog( (message reply)

			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message = idmBuildString(list("WorkLib" viewName))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
							if( reply=="f" /* then */
									return()
							/* else */
							return(reply)
							)
		)
	)

	procedure(dmsSkillPurgeCellView(cellName viewName @optional toolName)
		prog( (message reply)
			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message = idmBuildString(list("Purge" cellName viewName))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
			when( reply == "f"
				return()
			)
			return(t)
		)
	)

	procedure(dmsSkillReportMessage(@optional toolName)
		prog( (message reply)
			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message = idmBuildString(list("Report"))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
			return(reply)
		)
	)

	procedure(dmsSkillPrintPath(viewName @optional toolName)
		prog( (message reply)
			when( toolName
				unless( toolName == idmsCurrentTool
					unless( dmsSkillSetTool(toolName)
						return()
					)
				)
			)
			message = idmBuildString(list("PrintPath" viewName))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
			return(reply)	
		)
	)

	procedure(dmsSkillGetLastCellViewInfo()
		prog( (message reply replyList fpName)
			message = idmBuildString(list("LastInfo"))
			unless( idmSendMsgToSlave(message)
				return()
			)
			reply = idmGetMsgFromSlave()
							if( reply=="f" then
									return()
							else
									replyList = parseString(reply " ")
									case( car(replyList)
										("0" replyList=list(car(replyList) "" "" "" "" "" "" ""))
										("1" replyList=list(car(replyList) "" cadr(replyList) 
													caddr(replyList) "" "" cadddr(replyList) "")
										)
										("2" replyList=list(car(replyList) "" cadr(replyList)
													caddr(replyList) "" "" cadddr(replyList) "")
										)
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; BEGIN HACK HACK HACK HACK ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
										("3" when( length(replyList)==7
					fpName = list("" car(last(replyList)))
					rplacd(cdr(cddddr(replyList)) fpName)
						 )
				)
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; END HACK HACK HACK HACK ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;			(("3" "4" "5") nil)
				(("4" "5") nil)
									)
									return(replyList)
							)
		)
	)




	/**********************
		The code to be executed when the file is loaded  (provided there is not an already executing idmSlave process)
	**********************/

	(unless (and (boundp 'idmsSlaveId) (nequal idmsSlaveId nil))

		; make all export lists to be the blank string - no
		; other users should see anyone else's checked out 
		; files

		; set the global variables
		idmsSlaveId = nil
		idmsSlaveIsAlive = nil
				idmsSlaveInWait = nil
		idmsCurrentTool = nil
		idmsSearchPathString = ""
		idmsSearchPathPacketNumber = 0
		
		unless( boundp('idmsBlockPeriod)
			idmsBlockPeriod = 3
		)

		unless( boundp('idmsNumTimeOuts)
			idmsNumTimeOuts = 30
		)

		; debug based on a flag being set 
		unless( boundp('idmsDebugOn)
			idmsDebugOn = nil  
		)

		; start the slave based on a flag being set 
		unless( boundp('idmsSynchStart)
			idmsSynchStart = t
		)

		dmsSkillStartSlave(idmsSynchStart)

		(if (getShellEnvVar "NO_IDMSLAVE_BASED_CDSLIB")
		then
			;; continue to define this global, the path to the cds.lib file being used
			(setq dfiiCdsLibFile (ddGetForcedLib))
		else
			;; CSO
			;; The list of data libraries that a DFII V4.4 integrated application
			;; knows about is specified in a "cds.lib" file.  DFII locates its DEFAULT
			;; "cds.lib" file by:
			;; being told explicitly where it is via a  -cdslib <fileName> switch
			;; or looking for it in the directories specified by the
			;; {$CDS_SITE,$CDS_INST_DIR/share}/cdssetup/setup.loc file (both of which are
			;; most likely RO files).  If you want to modify the list of known libraries
			;; you need a RW-able cds.lib file...  If none of the cds.lib files found down
			;; a setup.loc search path are RW, the app will want to create its own RW-able
			;; cds.lib file (no inheritance of data or libs in the RO cds.lib files happens...).
			;; The RW-able cds.lib will be created in the first writable dir defined by the
			;; setup.loc file (typically "." (aka the apps current working/invocation dir)).
			;; 
			;; If two DFII app were invoked from the same working directory, they
			;; would share a "cds.lib" file.  This might not be desired/appropriate.
			;; Sooo  we want to ensure that there is a separate "cds.lib" file for
			;; each DFII or DFII-integrated application invoked.  idms.a has therefore been
			;; adjusted
			;; 	(upon DB init) to create and use as a "cds.lib" file:
			;; 									$WORK_AREA_ROOT_DIR/rundir/.<appName>_<PID>_cds.lib
			;; 	(upon exit) destroy same
			;; idmb and idmSlave will both re-generate their "cds.lib" files each time
			;; they are told to view the $DMSPATH as a different tool.  Furthermore,
			;; idmSlave has been adjusted to return (with the ready to begin
			;; operations message) the name of its "cds.lib" file (so that we can use the
			;; information in that file to drive DFII...  see below).
			;; 
			;; DFII presents a small challenge...  if you change the cds.lib file with (ddSetForcedLib <fileName>)
			;; the Lib Manager will continue to use the cds.lib file determined at application startup...
			;; The resolution of this problem is to invoke icde/layoutPlus/icfb/layout/etc with a  -cdslib <fileName>
			;; switch....
			;; 
			;; NOTE: $WORK_AREA_ROOT_DIR/rundir/...  is a hard-coded pre-requisite work-space dir
			;;       (driven by Tejas data-centric work space initiative)
			;; 
			;; As DFII is not itself idms.a linked we have to use the idmSlave to define the correct set
			;; of accessible libraries.  We do this by calling
			;; (dmsSkillSetTool) with an appropriate <toolName> and then performing a UNIX file copy
			;; of idmSlave's "cds.lib" file to DFII's "cds.lib" file, which is then followed by a call to 
			;; (ddUpdateLibList)
			;; 
			;; The big problem is... mapping the DFII's applications invocation name to a
			;; toolName mentioned in the $DMSPATH file....
			;; 
			;; If you setup the tool bundle opus_fubpv both opus_sch and opus_lay seem
			;; to get "set up".   If the user invokes opus_sch, they are likely to get
			;; icde.exe invoked.  If they invoke opus_lay, they are likely to get
			;; layoutPlus.exe Then there are folk that know they can also execute
			;; icca.exe, icds.exe, icfb.exe, icms.exe, layout.exe, msfb.exe
			;; 
			;; Decision/policy:  if we are icde.exe, call (dmsSkillSetTool "opus_sch")
			;; else (dmsSkillSetTool "opus_lay")
			;;
			;; the problem is that Cadence is rather inconsistent regarding how they format their version ID strings...
			;;
			;; if we are icde.exe:       (getVersion) -> "@(#)$CDS: icde.exe version 4.4.6 09/13/2002 01:51 (cds11701) $"
			;; if we are layoutPlus.exe: (getVersion) -> "layoutPlus version 4.4.6 Fri Sep 13 01:53:09 PDT 2002 (cds11701)"
			;; if we are icfb.exe:       (getVersion) -> "@(#)$CDS: icfb.exe version 4.4.6 09/13/2002 01:55 (cds11701) $"
			;; if we are iccc.exe:       (getVersion) -> "icca version 4.4.6 Fri Sep 13 01:50:26 PDT 2002 (cds11701)"
			;; if we are layout.exe:     (getVersion) -> "layout version 4.4.6 Fri Sep 13 01:52:44 PDT 2002 (cds11701)"
			;;
			;; so the executables name is the word immediately before "version" sans any trailing ".exe"


			(let (toolName toolBundleName vidWordsList lastWord)
				;; get toolName (icde, layoutPlus, layout, icfb)
				(setq vidWordsList (parseString (getVersion) " "))
				(setq lastWord (car vidWordsList))
				(prog (wrd)
					(foreach wrd (cdr vidWordsList)
						(when (equal wrd "version")
							(setq toolName (car (parseString lastWord ".")))
							(return)
						)
						(setq lastWord wrd)
					)
					(error "dmserver.il: unable to parse versionId string (%s)\n             to obtain executable name...\n" (getVersion))
					(setq toolName "unknown")
				)

				;; another global like idmsSlaveCdsLibFile
				(if (nequal (setq dfiiCdsLibFile (ddGetForcedLib)) "")
				then

					(if (equal "icde" toolName)
						(setq toolBundleName "opus_sch") ;; schematics ONLY data search paths
						(setq toolBundleName "opus_lay") ;; schematics AND layout data search paths
					)
					(printf "-I- dmserver.il: building cds.lib file with libpath elments for %s\n" toolBundleName)
					(dmsSkillSetTool toolBundleName)

					(system (strcat "/bin/cp -f " idmsSlaveCdsLibFile " " dfiiCdsLibFile))
					(ddSetForcedLib dfiiCdsLibFile)
					(ddUpdateLibList)
				else
					error("dmserver.il: don't know where %s's cds.lib file is.  missing -cdslib switch?\nDMSPATH defined libs will not be available...\n" toolName)
				)
			)
		) ;; if (getShellEnvVar "NO_IDMSLAVE_BASED_CDSLIB")


		; define some variables that applications can
		; use for looking at values of skill interface 
		; return codes

		; Info
		idmsSearchWholeRwpaths               = "0"
		idmsSearchLibOnly                    = "1"
		
		idmsInfoCvNotFoundOnRwpaths          = "0"
		idmsInfoCvNotFoundInAnyLibrary       = "1"
		idmsInfoCvIsInWorkdir                = "2"
		idmsInfoCvCheckedOutByMe             = "3"
		idmsInfoCvIsInReadonlyUnixdir        = "4"
		idmsInfoCvIsInReadonlyLibrary        = "5"
		idmsInfoCvCheckedOutByAnotherUser    = "6"
		idmsInfoCvCanBeCheckedOut            = "7"
		idmsInfoUnknownEvent                 = "77"

		; Last Cellview Location
		idmsCvNotFound                       = "0"
		idmsCvInWorkdir                      = "1"
		idmsCvInUnixdir                      = "2"
		idmsCvInRwlib                        = "3"
		idmsCvInRolib                        = "4"
		idmsCvInWorklib                      = "5"
		

		; set a global flag indicating that this 
		; context is successfully loaded - to be used
		; by other contexts that depend on this one

		idmsContextIsLoaded = t


	)
)

(when (getShellEnvVar "IDMB_DFII_GUI")

	/*************
	 this is the skill source code which will provide a DFII GUI interface to
	 idmb - running as a service provider for DFII.

	 Given that we are already running idmSlave (see code up above) to get
	 all all of the raw data programatically, the need to fire up yet another
	 child process under DFII to interact with idmb is ... curious...


	 The io handler is the core - and invokes the right procedure in 
	 stack the right variables

	 While I have tried to make this architecture with as few assumptions as
	 possible - there is an hidden assumption that the second request to the
	 server would be answered only after action on the first is complete. 
	 If this is not the case then the idmbIOhandler has to be
	 rearchitected in a re-entrant manner

	For 4.4.x: Since RWS is unavailable, the server is started from the
	dfII session thru Skill IPC in asyncronous mode. The readFromProcess
	will display in the encapWindow.

	***************/


	count=0

	cPmt = list("Cell Name" "*" "s")
	fPmt = list("Configuration Name" "NONE" "s")
	f2Pmt = list("Configuration Name" "NONE" "s")
	fiPmt = list("Config Instance Name" "NONE" "s")
	mPmt = list("Config Comment" "Comment" "s")
	aPmt = list("Append to Previous Comment" list("no" "yes") "r" 2)
	ePmt = list("Expand Sub-configurations" list("no" "yes") "r" 2)
	bPmt = list("Binding Type" list("Static" "Time (Most Recent by Time)" "Primary Index (Most Recent by Primary Index)") "r" 1)
	gPmt = list("Log Message" "checkin" "s")
	hPmt = list("Library Path" "." "s")
	iPmt = list("Advance Primary Index" list("no" "yes") "r" 2)
	lPmt = list("Library Name" "*" "s")
	l2Pmt = list("Diff Library Name" "*" "s")
	slPmt = list("Sub-Config Library" "*" "s")
	pPmt = list("Property Name" "propName" "s")
	rPmt = list("Version Name" "current" "s")
	tPmt = list("Tool Name" "idmb" "s")
	uPmt = list("Property Value" "value" "s")
	vPmt = list("View Name" "*" "s")
	xPmt = list("Unix Command" "ls" "s")
	yPmt = list("Property Type" list("String" "Integer" "Boolean" "Time") "r" 4)
	flPmt = list("File List" "cellview.list" "s")
	wgPmt = list("Working Group" "" "s")
	ugPmt = list("User Group" "" "s")
	tfPmt = list("Text File Name" "config.ascii" "s")
	opPmt = list("Output File Name" "" "s")
	ouPmt = list("Checkout User Name" "*" "s")
	cuPmt = list("Checkin Owner Name" "*" "s")
	bwPmt = list("By Whom" "*" "s")
	ftPmt = list("From Time" getCurrentTime() "s")
	ttPmt = list("To Time" getCurrentTime() "s")
	baPmt = list("By Action" list("*" "Add" "Create" "Checkin" "Checkout" "Delete" "Un-Checkout" "Extract") "r" 4)
	vrPmt = list("Version Name" "*" "s")
	dirPmt = list("To Directory" "." "s")

	unless(boundp('_idmbEncapWindowID)
		_idmbEncapWindowID = nil
	)
	unless(boundp('_idmbSlaveId)
		_idmbSlaveId = nil
	)

			
	/*******************************************************
	*******        SKILL IPC PROCEDURES      ***************
	********************************************************/

	/* procedure to spawn the idmbSlave process */
	procedure(idmbSkillStartSlave()
	prog( (command logFile)	
		; if slave and window already exist do nothing 
		when( boundp('_idmbSlaveId) && idmbSkillIsSlaveAlive() && 
							boundp('_idmbEncapWindowID) &&
							windowp(_idmbEncapWindowID)
			return(t)
		)

		unless(idmbSkillIsSlaveAlive() 

			/** These guys need to be loaded before idmSlave 
					can start so the sync trigs can be registered **/
			unless(boundp('dssvInit)
				load(strcat(getShellEnvVar("SYNC_DIR") "/cds/skill/dssInit.il"))
			)
			unless(boundp('dssTrigRegistered)
				load(strcat(getShellEnvVar("SYNC_DIR") "/cds/skill/dssTrigger.il"))
			)
			unless(boundp('dssTrigPID) && dssTrigPID
				dssTrigStartIPCChan()
			)

			/* get the environment variable for where the idmSlave process is stored */
			command = getShellEnvVar("DMSERVER_BIN_DIR")
			unless( command
				printf("**idmbWarn: Env Variable DMSERVER_BIN_DIR not set, set it and rerun\n") 
			return()
			)
			sprintf(command "%s/idmb" command)
			logFile = sprintf(nil "/tmp/idmSlave.log_%d.%d"  (ipcGetPid)
							stringToTime(getCurrentTime()))

			; spawn off slave with appropriate handlers

			(setq _idmbSlaveId  (ipcBeginProcess
					command          	; command
					""               	; local host
					'idmbGetMsgFromSlave	;to handle incomming data
					nil
					nil			;post function
					logFile
				))

			; Now we have to wait for the slave 
			(ipcWaitForProcess _idmbSlaveId)
		)
		printf("**idmbInfo: idmb log file is %s\n" logFile) 

		unless(windowp(_idmbEncapWindowID)
			idmbCreateWindow()
		)

	)
	)


	procedure( idmbCreateWindow()
		; hiSetFont("text" "Rom14.500")
		_idmbEncapWindowID=hiEncap()
		hiSetWindowName(_idmbEncapWindowID "iDMB 2.11.a8")
		hiSetIconName(_idmbEncapWindowID "iDMB")
		hiSetEncapSkillCmd(_idmbEncapWindowID "idmbSendCommand")
		idmbAddMenusToEncapWindow(_idmbEncapWindowID)
	)

	procedure(idmbSendCommand(message)
		prog( (idmbCommand stringList)

		if(message then
			stringList = parseString(message "|")

			if(!stringList then stringList=list(""))
			foreach(element stringList
				 sprintf(idmbCommand "%s\n" element)
				 ;printf("Message to Child is %L\n" idmbCommand) 
				 (ipcWriteProcess _idmbSlaveId idmbCommand)
			)
			hiSetEncapPrompt(_idmbEncapWindowID "Input Options:")
		)
	))

	/*****
	get a message from the idmSlave and pass it to encap window
	****/

	procedure(idmbGetMsgFromSlave(_idmbSlaveId data)

	;fprintf(poport "%s" data)
	;drain(poport)

	hiSetEncapHistory(_idmbEncapWindowID data)

	)

	procedure(idmbSkillIsSlaveAlive()
		when(_idmbSlaveId
			(ipcIsAliveProcess _idmbSlaveId)
		)
	)


	procedure(idmbChoiceNameFormProc(unknown)
		foreach(element symbolList
			sprintf(str "choiceNameForm->%s->value" 
									(if stringp(element) element sprintf(nil "%L" element)))
			idmbSendCommand(evalstring(str))
		)
	)

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; Form Creation
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	procedure(idmbSetChoiceName(formTitle fieldList)
	prog((tPrompt)

		if(!fieldList then return(t))
		promptList=nil
		symbolList=nil
		foreach( field fieldList
			tmpPrompt = car(field)
			tmpValue = cadr(field)
			tmpType = caddr(field)
			symbol = gensym("fieldPrompt")
			case(tmpType
				("s" fieldHandle=hiCreateStringField(?name symbol
								?prompt tmpPrompt
																	?value tmpValue))
				("r" fieldHandle=hiCreateRadioField(?name symbol
								?prompt tmpPrompt
								?itemsPerRow cadddr(field)
								?value car(tmpValue)
								?choices tmpValue))
				("c" fieldHandle=hiCreateCyclicField(?name symbol
								?prompt tmpPrompt
								?value car(tmpValue)
								?choices tmpValue))
				("l" fieldHandle=hiCreateLabel(?name symbol
								?labelText tmpPrompt))
			)
			promptList = append(promptList list( fieldHandle ))
			symbolList = append(symbolList list( symbol ))
		)

		choiceNameForm = hiCreateForm( 'choiceNameForm formTitle
																		`idmbChoiceNameFormProc()
																		promptList
																		)

		return(hiDisplayForm(choiceNameForm))
	))

	procedure(idmbSendMessage(formTitle message choice)
		prog( ()

		idmbSendCommand(message)

		if(!idmbSetChoiceName(formTitle choice) then
			idmbSendCommand(";\n")
			hiSetEncapPrompt(_idmbEncapWindowID "Command:")
		else
			idmbSendCommand("IDMB_GO_DO_IT\n")
		)
		return(t)
	))

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; Query functions
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	qvText = "View Database"
	qvItem = hiCreateMenuItem(?name 'qvItem
			?itemText qvText
			?callback "idmbSendMessage(qvText \"v\"
				list(tPmt lPmt cPmt vPmt rPmt cuPmt ouPmt))"
	)

	feText = "Location of Files"
	feItem = hiCreateMenuItem(?name 'feItem
			?itemText feText
			?callback "idmbSendMessage(feText \"e\" list(tPmt lPmt cPmt vPmt))"
	)

	;rzText = "Diff Versions"
	;rzItem = hiCreateMenuItem(?name 'rzItem
	;    ?itemText rzText
	;    ?callback "idmbSendMessage(rzText \"z\"
	;			list(tPmt lPmt cPmt vPmt rPmt rPmt))"
	;)

	rrText = "Log History"
	rrItem = hiCreateMenuItem(?name 'rrItem
			?itemText rrText
			?callback "idmbSendMessage(rrText \"r\" list(tPmt lPmt cPmt vPmt))"
	)

	quryList = list(
					qvItem
		feItem
	;	rzItem
		rrItem
					)

	qurySubMenu = hiCreatePulldownMenu(
					'qurySubMenu
					"Query"
					quryList
					""
	)

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; DMS.PTH functions
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	ssText = "Search Path"
	ssItem = hiCreateMenuItem(?name 'ssItem
			?itemText ssText
			?callback "idmbSendMessage(ssText \"s|s|\" list(tPmt))"
	)

	swText = "Working Directory"
	swItem = hiCreateMenuItem(?name 'swItem
			?itemText swText
			?callback "idmbSendMessage(swText \"s|w|\" list(tPmt vPmt))"
	)

	slText = "Working Library"
	slItem = hiCreateMenuItem(?name 'slItem
			?itemText slText
			?callback "idmbSendMessage(slText \"s|l|\" list(tPmt vPmt))"
	)

	srText = "Reparse DMS.PATH"
	srItem = hiCreateMenuItem(?name 'srItem
			?itemText srText
			?callback "idmbSendMessage(srText \"s|r|\" nil)"
	)

	spText = "Print Parsed Result"
	spItem = hiCreateMenuItem(?name 'spItem
			?itemText spText
			?callback "idmbSendMessage(spText \"s|p|\" list(tPmt vPmt))"
	)

	pathList = list(
		srItem
		spItem
					ssItem
		swItem
		slItem
					)

	pathSubMenu = hiCreatePulldownMenu(
					'pathSubMenu
					"DMS.PATH"
					pathList
					""
	)

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; File I/O
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	riText = "Install a File"
	riItem = hiCreateMenuItem(?name 'riItem
			?itemText riText
			?callback "idmbSendMessage(riText \"n\" list(tPmt cPmt vPmt))"
	)

	rlText = "Checkout Files"
	rlItem = hiCreateMenuItem(?name 'rlItem
			?itemText rlText
			?callback "idmbSendMessage(rlText \"l\" list(tPmt lPmt fPmt cPmt vPmt rPmt))"
	)

	rwText = "Checkin Files"
	rwItem = hiCreateMenuItem(?name 'rwItem
			?itemText rwText
			?callback "idmbSendMessage(rwText \"w\"
				list(tPmt lPmt cPmt vPmt rPmt gPmt fPmt iPmt))"
	)

	rcText = "Cancel Checkout Files"
	rcItem = hiCreateMenuItem(?name 'rcItem
			?itemText rcText
			?callback "idmbSendMessage(rcText \"c\" list(tPmt lPmt cPmt vPmt))"
	)

	rfText = "Fetch Files"
	rfItem = hiCreateMenuItem(?name 'rfItem
			?itemText rfText
			?callback "idmbSendMessage(rfText \"f\" list(tPmt lPmt fPmt cPmt vPmt rPmt))"
	)

	fiText = "Import Files"
	fiItem = hiCreateMenuItem(?name 'fiItem
			?itemText fiText
			?callback "idmbSendMessage(fiText \"i\" list(tPmt lPmt flPmt))"
	)

	fxText = "Export Files"
	fxItem = hiCreateMenuItem(?name 'fxItem
			?itemText fxText
			?callback "idmbSendMessage(fxText \"x\" list(tPmt lPmt flPmt dirPmt))"
	)

	fgText = "Policy Readonly/Checkout Files"
	fgItem = hiCreateMenuItem(?name 'fgItem
			?itemText fgText
			?callback "idmbSendMessage(fgText \"g\" list(tPmt lPmt fPmt cPmt vPmt rPmt))"
	)

	fdText = "Delete Files"
	fdItem = hiCreateMenuItem(?name 'fdItem
			?itemText fdText
			?callback "idmbSendMessage(fdText \"d\" list(tPmt lPmt cPmt vPmt))"
	)

	fileList = list(
					riItem
		rlItem
		rwItem
		rcItem
		rfItem
					fiItem
		fxItem
		fgItem
		fdItem
					)

	fileSubMenu = hiCreatePulldownMenu(
					'fileSubMenu
					"File I/O"
					fileList
					""
	)

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; Configuration functions
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	ylText = "List All Configuration"
	ylItem = hiCreateMenuItem(?name 'ylItem
			?itemText ylText
			?callback "idmbSendMessage(ylText \"y|l|\" list(tPmt lPmt))"
	)

	ycText = "Create a Configuration"
	ycItem = hiCreateMenuItem(?name 'ycItem
			?itemText ycText
			?callback "idmbSendMessage(ycText \"y|c|\" list(tPmt lPmt fPmt))"
	)

	ytText = "Dump Contents into Text"
	ytItem = hiCreateMenuItem(?name 'ytItem
			?itemText ytText
			?callback "idmbSendMessage(ytText \"y|t|\" list(tPmt lPmt fPmt tfPmt ePmt))"
	)

	yoText = "Load Configuration from Text"
	yoItem = hiCreateMenuItem(?name 'yoItem
			?itemText yoText
			?callback "idmbSendMessage(yoText \"y|o|\" list(tPmt lPmt tfPmt))"
	)

	ydText = "Delete a Configuration"
	ydItem = hiCreateMenuItem(?name 'ydItem
			?itemText ydText
			?callback "idmbSendMessage(ydText \"y|d|\" list(tPmt lPmt fPmt))"
	)

	yaText = "Add Cellviews"
	yaItem = hiCreateMenuItem(?name 'yaItem
			?itemText yaText
			?callback "idmbSendMessage(yaText \"y|a|\"
				list(tPmt lPmt fPmt cPmt vPmt rPmt bPmt))"
	)

	yrText = "Remove Cellviews"
	yrItem = hiCreateMenuItem(?name 'yrItem
			?itemText yrText
			?callback "idmbSendMessage(yrText \"y|r|\"
				list(tPmt lPmt fPmt cPmt vPmt rPmt))"
	)

	yfText = "Diff Two Configurations"
	yfItem = hiCreateMenuItem(?name 'yfItem
			?itemText yfText
			?callback "idmbSendMessage(yfText \"y|f|\" list(tPmt lPmt fPmt l2Pmt f2Pmt opPmt))"
	)

	ynText = "Insert a Configuration to Another"
	ynItem = hiCreateMenuItem(?name 'ynItem
			?itemText ynText
			?callback "idmbSendMessage(ynText \"y|n|\" list(tPmt lPmt fPmt f2Pmt))"
	)

	yxText = "Export Files in a Configuration"
	yxItem = hiCreateMenuItem(?name 'yxItem
			?itemText yxText
			?callback "idmbSendMessage(yxText \"y|x|\" list(tPmt lPmt fPmt dirPmt))"
	)

	ysText = "Add Sub-Configuration"
	ysItem = hiCreateMenuItem(?name 'ysItem
			?itemText ysText
			?callback "idmbSendMessage(ysText \"y|s|\"
				list(tPmt lPmt fPmt slPmt f2Pmt))"
	)

	yvText = "Remove Sub-Configuration"
	yvItem = hiCreateMenuItem(?name 'yvItem
			?itemText yvText
			?callback "idmbSendMessage(yvText \"y|v|\"
				list(tPmt lPmt fPmt slPmt f2Pmt))"
	)

	yiText = "Create Configuration Instance"
	yiItem = hiCreateMenuItem(?name 'yiItem
			?itemText yiText
			?callback "idmbSendMessage(yiText \"y|i|\"
				list(tPmt lPmt fPmt fiPmt))"
	)

	ymText = "Create Configuration Comment"
	ymItem = hiCreateMenuItem(?name 'ymItem
			?itemText ymText
			?callback "idmbSendMessage(ymText \"y|m|\"
				list(tPmt lPmt fPmt mPmt aPmt))"
	)

	confList = list(
					ylItem
		ycItem
		ytItem
		yoItem
		ydItem
		yaItem
		yrItem
		yfItem
		ynItem
		yxItem
		ysItem
		yvItem
		yiItem
		ymItem
					)

	confSubMenu = hiCreatePulldownMenu(
					'confSubMenu
					"Configuration"
					confList
					""
	)

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; Audittrail functions
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	aeText = "Enable Audittrail"
	aeItem = hiCreateMenuItem(?name 'aeItem
			?itemText aeText
			?callback "idmbSendMessage(aeText \"a|e|\" list(tPmt lPmt))"
	)

	adText = "Disable Audittrail"
	adItem = hiCreateMenuItem(?name 'adItem
			?itemText adText
			?callback "idmbSendMessage(adText \"a|d|\" list(tPmt lPmt))"
	)

	aiText = "Inquire Audittrail"
	aiItem = hiCreateMenuItem(?name 'aiItem
			?itemText aiText
			?callback "idmbSendMessage(aiText \"a|i|\"
				list(tPmt lPmt bwPmt ftPmt ttPmt cPmt vPmt vrPmt baPmt))"
	)

	audtList = list(
		aiItem
					aeItem
		adItem
					)

	audtSubMenu = hiCreatePulldownMenu(
					'audtSubMenu
					"Audittrail"
					audtList
					""
	)

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; Property functions
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	pcText = "Create Property"
	pcItem = hiCreateMenuItem(?name 'pctem
			?itemText pcText
			?callback "idmbSendMessage(pcText \"p|c|\"
				list(tPmt lPmt cPmt vPmt rPmt pPmt yPmt uPmt))"
	)

	plText = "List Property"
	plItem = hiCreateMenuItem(?name 'plItem
			?itemText plText
			?callback "idmbSendMessage(plText \"p|l|\" list(tPmt lPmt cPmt vPmt rPmt))"
	)

	prText = "Remove Property"
	prItem = hiCreateMenuItem(?name 'prItem
			?itemText prText
			?callback "idmbSendMessage(prText \"p|r|\"
				list(tPmt lPmt cPmt vPmt rPmt pPmt))"
	)

	propList = list(
		plItem
					pcItem
		prItem
					)

	propSubMenu = hiCreatePulldownMenu(
					'propSubMenu
					"Property"
					propList
					""
	)

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; Misc functions
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	procedure(idmbEditDmspath()
		sprintf(tmpString "vi(\"%s\")" getShellEnvVar("DMSPATH"))
		if( evalstring(tmpString) then idmbSendMessage(meText "s|r|" nil))
	)

	muText = "Memory Usage"
	muItem = hiCreateMenuItem(?name 'muItem
			?itemText muText
			?callback "idmbSendMessage(muText \"m|m|\" nil)"
	)

	;mkText = "Execute Unix Command"
	;mkItem = hiCreateMenuItem(?name 'mkItem
	;    ?itemText mkText
	;    ?callback "idmbSendMessage(mkText \"m|u|\" list(xPmt))"
	;)

	meText = "Edit DMS.PATH File"
	meItem = hiCreateMenuItem(?name 'meItem
			?itemText meText
			?callback "idmbEditDmspath"
	)

	miscList = list(
					muItem
	;	mkItem
		meItem
					)

	miscSubMenu = hiCreatePulldownMenu(
					'miscSubMenu
					"Misc"
					miscList
					""
	)

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; Quit functions
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	qqText = "Quit iDMB"
	qqItem = hiCreateMenuItem(?name 'qqItem
			?itemText qqText
			?callback "idmbSendMessage(qqText \"q\" nil)"
	)

	quitList = list(
					qqItem
					)

	quitSubMenu = hiCreatePulldownMenu(
					'quitSubMenu
					"Quit"
					quitList
					""
	)

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	;		help menu
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	procedure(idmbHelpOk(form "r")
	let((helpString)

			idmbHelpPath = "/usr/cad/alpha/idmb/2.11.a8"

			sprintf(topicString "<<<<<[ Topic: %s ]>>>>>"
			idmbAsciiHelpForm->idmbHelpTopic->value)
			helpString = sprintf(helpString, "%s/iDMB.guide" idmbHelpPath)
			if( isFileName(helpString) then
					viWindowID=view(helpString nil nil topicString)
		hiScrollWindowToIndex(viWindowID hiStartGenTextIndex(viWindowID
					topicString))
		hiSetTextSelection(viWindowID topicString)
			else
		hiDisplayModalDBox('idmbWarning "No Help Available"
				"There is no help available on this topic"
													"" "" nil hicErrorDialog
													)
					)
			)
	)

	topics = list("General" "DMS.PATH" "File I/O" "Query" "Misc" "Configuration"  "Audittrail" "Property" "UserGuide" "Quit" )

	idmbAsciiHelpForm = hiCreateForm( 'idmbAsciiHelpForm "Help"
		list( 'idmbHelpOk )
		list( hiCreateRadioField(?name 'idmbHelpTopic
					?prompt "Topic Selection:"
					?itemsPerRow 5
					?value car(topics)
					?choices topics)) "" t)


	idmbHelpItem = hiCreateMenuItem(?name 'idmbHelpItem
					?itemText "Help"
					?callback "hiDisplayForm(idmbAsciiHelpForm)"
	)

	helpList = list(
									idmbHelpItem
							)

	helpSubMenu = hiCreatePulldownMenu(
							'helpSubMenu
							"UserGuide"
							helpList
							""
	)

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; Set Encap Window Menu
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	procedure(idmbAddMenusToEncapWindow(windowPointer)
		;hiInsertBannerMenu(windowPointer rcsSubMenu hiGetNumMenus(windowPointer))
		hiInsertBannerMenu(windowPointer pathSubMenu hiGetNumMenus(windowPointer))
		hiInsertBannerMenu(windowPointer fileSubMenu hiGetNumMenus(windowPointer))
		hiInsertBannerMenu(windowPointer qurySubMenu hiGetNumMenus(windowPointer))
		hiInsertBannerMenu(windowPointer confSubMenu hiGetNumMenus(windowPointer))
		hiInsertBannerMenu(windowPointer audtSubMenu hiGetNumMenus(windowPointer))
		hiInsertBannerMenu(windowPointer propSubMenu hiGetNumMenus(windowPointer))
		hiInsertBannerMenu(windowPointer miscSubMenu hiGetNumMenus(windowPointer))
		hiInsertBannerMenu(windowPointer quitSubMenu hiGetNumMenus(windowPointer))
		hiInsertBannerMenu(windowPointer helpSubMenu hiGetNumMenus(windowPointer))
		t
	)


	;; insert idmb menus into CIW
	_idmbMenuItem1 = (hiCreateMenuItem ?name '_idmbMenuItem1
													?itemText "Start idmb.."
													?callback "(idmbSkillStartSlave)"
													)
			
	_idmbListMenu = (hiCreateMenu '_idmbMenuItem1
													" idmb "
													(list _idmbMenuItem1)
													)
			
	(hiInsertBannerMenu (hiGetCIWindow)
												 '_idmbListMenu
													(hiGetNumMenus (hiGetCIWindow))
													)
)



;; GenesysDumpCategoryTop
;;     libName <in>: the library to generate category text files.
;; Date: 8/8/2003

;;
;; This procedure opens a Opus4.3 library and goes through the 
;; categories in this library.  It writes all category names in
;; <libname>.TopCat file, and it writes a <categoryname>.Cat file
;; for each category.  All text files are written in the <libpath>/<libName>/
;; directory.
;;  


(procedure (GenesysDumpCategoryTop libName )
  "Top level function for writing out the Top category"
  (prog (msg l_dumped)
    (setq dumped (list nil))
    (setq libId (dmOpenLib libName))
    ;; return if cannot open library. 
    (cond
        ((null libId)
            (printf "Failed to open library")
            (return nil)))

    (sprintf libpath "%s/%s" libId~>path libName)
    (sprintf fileName "%s/%s.TopCat" libpath libName)
    catPort = (outfile fileName)
    (fprintf catPort "TDMCHECKPOINT=\"1.0\"\n")
    (printf "Writing top category file: %s\n" fileName)
    
    (foreach cat libId~>category
      (fprintf catPort "%s/%s.Cat type=\"category\"\n" libName cat~>name)
      (sprintf catFile "%s/%s.Cat" libpath cat~>name)
      (GenesysDumpCategoryCat cat catFile libName libpath catPort ""))
    ;; no need to do non-categorized because CellView will process.
    (close catPort)
    ))


(procedure (GenesysDumpCategoryCat catId fileName libName libpath fptr prefix)
  "helper function for writing out the Cat category"
  (prog
    (catcount s_cat)
    (setq s_cat (concat (catId~>name)))
    (setq catcount (get dumped s_cat))
    (putprop dumped (if catcount
                        then (catcount + 1)
                        else 0)
             s_cat)
    (catP = (outfile fileName))
    (fprintf catP "TDMCHECKPOINT=\"1.0\"\n%s/%s.Cat type=\"cell\"\n" libName catId~>name)
    (unless catcount
      (foreach item (GenesysAndNotList catId~>members l_dumped)
	(if (member item (item~>lib~>groups)) then
	  ;;a nested category
	  (sprintf subFile "%s/%s.Cat" libpath item~>name)
	  (close catP)
	  (sprintf prefix "%s/%s" prefix item~>name)
	  (fprintf fptr "%s/%s.Cat type=\"category\"\n" libName prefix )
           
	  (GenesysDumpCategoryCat item subFile libName libpath fptr prefix)
   	  (catP = (outfile fileName "a"))
	 else ;;regular cell
          (fprintf catP "%s/%s type=\"cell\"\n" libName item~>name))))
    (close catP)
    ))



;; relative complement of lists.  Constructs a new list of all \
;; elements in l_list1 but not in l_list2.  Any element that appears \
;; multiple times in l_list1 will appear multiple times in the result."
(procedure (GenesysAndNotList list1 list2)
  "relative complement of two lists"
  (setof x list1 (null (member x list2))))

