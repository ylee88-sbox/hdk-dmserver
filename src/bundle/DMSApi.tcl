#!tcl

proc DMSInit { toolname } {
    global DMSH env

    set DMSH [open "| $env(DMSERVER_BIN_DIR)/dmsapi" w+]
    
    fconfigure $DMSH -buffering none
    
    set gotOK 0
    while {!$gotOK} {
	gets $DMSH line
	#puts "GOT: $line"
	
	if [regexp {^DMSAPI_READY } $line] {
	    set gotOK 1
	}
    }
    set result [DMSSendCmd setToolName $toolname]
    if {[DMSSendCmd getToolName] != $toolname} {
	error "Failed to initialize DMS for tool $toolname"
    }
}

proc DMSSendCmd { args } {
    global DMSH
    set line ""
    set result ""

    puts $DMSH $args
    gets $DMSH line
    #puts "DMSSendCmd: $line"
    if [regexp {^OK (.*)} $line junk result] {
	return $result
    } else {
	puts $line
    }
}

proc DMSEndSession { } {
    global DMSH

    DMSSendCmd quit
    close $DMSH
}
