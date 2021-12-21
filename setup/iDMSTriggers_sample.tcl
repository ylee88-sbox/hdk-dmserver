# A sample Tcl iDMS Data Management event trigger procedure definition file
#
# To be used, the env var IDMS_USERTRIGGERFILE_TCL must point to this file
#
# Tcl procedures defined in this file will be called if the procedure name shows up
# in the pre or post event trigger name colum of the iDMSUserTriggerTable_Tcl table
# which is defined in the file pointed at by the env var IDMS_GLOBALFILE  (see
# iDMSglobalTable)
#
# Return of the value  0 (zero) indicates failure.  Return of non-zero indicates success

proc Tcl_create_pre {nullgt cn vn lp ln rv} {
	puts stderr "Tcl_create_pre: called with $nullgt $cn $vn $lp $ln $rv"
}
proc Tcl_create_pst {nullgt cn vn lp ln rv} {
	puts stderr "Tcl_create_pst: called with $nullgt $cn $vn $lp $ln $rv"
}
proc Tcl_checkout_pre {nullgt cn vn lp ln rv} {
	puts stderr "Tcl_checkout_pre: called with $nullgt $cn $vn $lp $ln $rv"
}
proc Tcl_checkout_pst {nullgt cn vn lp ln rv} {
	puts stderr "Tcl_checkout_pst: called with $nullgt $cn $vn $lp $ln $rv"
}
proc Tcl_save_pre {nullgt cn vn lp ln rv} {
	puts stderr "Tcl_save_pre: called with $nullgt $cn $vn $lp $ln $rv"
}
proc Tcl_save_pst {nullgt cn vn lp ln rv} {
	puts stderr "Tcl_save_pst: called with $nullgt $cn $vn $lp $ln $rv"
}
proc Tcl_install_pre {nullgt cn vn lp ln rv} {
	puts stderr "Tcl_install_pre: called with $nullgt $cn $vn $lp $ln $rv"
}
proc Tcl_install_pst {nullgt cn vn lp ln rv} {
	puts stderr "Tcl_install_pst: called with $nullgt $cn $vn $lp $ln $rv"
}
proc Tcl_checkin_pre {nullgt cn vn lp ln rv} {
	puts stderr "Tcl_checkin_pre: called with $nullgt $cn $vn $lp $ln $rv"
}
proc Tcl_checkin_pst {nullgt cn vn lp ln rv} {
	puts stderr "Tcl_checkin_pst: called with $nullgt $cn $vn $lp $ln $rv"
}
proc Tcl_update_pre {nullgt cn vn lp ln rv} {
	puts stderr "Tcl_update_pre: called with $nullgt $cn $vn $lp $ln $rv"
}
proc Tcl_update_pst {nullgt cn vn lp ln rv} {
	puts stderr "Tcl_update_pst: called with $nullgt $cn $vn $lp $ln $rv"
}
proc Tcl_cancelCheckout_pre {nullgt cn vn lp ln rv} {
	puts stderr "Tcl_cancelCheckout_pre: called with $nullgt $cn $vn $lp $ln $rv"
}
proc Tcl_cancelCheckout_pst {nullgt cn vn lp ln rv} {
	puts stderr "Tcl_cancelCheckout_pst: called with $nullgt $cn $vn $lp $ln $rv"
}
