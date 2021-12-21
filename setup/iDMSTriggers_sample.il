# A sample SKILL iDMS Data Management event trigger procedure definition file
#
# To be used, some additional Tcl and Skill code that implements a message
# passing pathway between a DesignSync linked client and a DFII executable
# needs to be updated and deployed.  DT has a proof of concept copy of the
# needed code.  As the DMS is no longer DFII based, it really doesn't make
# sense for non DFII executables to rely on DFII SKILL encoded trigger behavior
# when they could far less trouble port their DFII SKILL encoded triggers
# to DesignSync Tcl...   See the iDMSglobalTable and iDMSTriggers.tcl files.
#
# When the dssEvalSkill Tcl function exists, the code below will be used if
# the env var IDMS_USERTRIGGERFILE points to this file
#
# the SKILL procedures defined in this file will be called if the procedure
# name shows up in the pre or post event trigger name colum of the
# iDMSUserTriggerTable_Tcl table which is defined in the file pointed at by
# the env var IDMS_GLOBALFILE  (see iDMSglobalTable)

(procedure SKILL_create_pre (nullgt cn vn lp ln rn)
	(printf "SKILL_create_pre: called with nullgt=%d cn=%s vn=%s lp=%s ln=%s rn=%s\n" nullgt cn vn lp ln rn)
)
(procedure SKILL_create_pst (nullgt cn vn lp ln rn)
	(printf "SKILL_create_pst: called with nullgt=%d cn=%s vn=%s lp=%s ln=%s rn=%s\n" nullgt cn vn lp ln rn)
)
(procedure SKILL_checkout_pre (nullgt cn vn lp ln rn)
	(printf "SKILL_checkout_pre: called with nullgt=%d cn=%s vn=%s lp=%s ln=%s rn=%s\n" nullgt cn vn lp ln rn)
)
(procedure SKILL_checkout_pst (nullgt cn vn lp ln rn)
	(printf "SKILL_checkout_pst: called with nullgt=%d cn=%s vn=%s lp=%s ln=%s rn=%s\n" nullgt cn vn lp ln rn)
)
(procedure SKILL_save_pre (nullgt cn vn lp ln rn)
	(printf "SKILL_save_pre: called with nullgt=%d cn=%s vn=%s lp=%s ln=%s rn=%s\n" nullgt cn vn lp ln rn)
)
(procedure SKILL_save_pst (nullgt cn vn lp ln rn)
	(printf "SKILL_save_pst: called with nullgt=%d cn=%s vn=%s lp=%s ln=%s rn=%s\n" nullgt cn vn lp ln rn)
)
(procedure SKILL_install_pre (nullgt cn vn lp ln rn)
	(printf "SKILL_install_pre: called with nullgt=%d cn=%s vn=%s lp=%s ln=%s rn=%s\n" nullgt cn vn lp ln rn)
)
(procedure SKILL_install_pst (nullgt cn vn lp ln rn)
	(printf "SKILL_install_pst: called with nullgt=%d cn=%s vn=%s lp=%s ln=%s rn=%s\n" nullgt cn vn lp ln rn)
)
(procedure SKILL_checkin_pre (nullgt cn vn lp ln rn)
	(printf "SKILL_checkin_pre: called with nullgt=%d cn=%s vn=%s lp=%s ln=%s rn=%s\n" nullgt cn vn lp ln rn)
)
(procedure SKILL_checkin_pst (nullgt cn vn lp ln rn)
	(printf "SKILL_checkin_pst: called with nullgt=%d cn=%s vn=%s lp=%s ln=%s rn=%s\n" nullgt cn vn lp ln rn)
)
(procedure SKILL_update_pre (nullgt cn vn lp ln rn)
	(printf "SKILL_update_pre: called with nullgt=%d cn=%s vn=%s lp=%s ln=%s rn=%s\n" nullgt cn vn lp ln rn)
)
(procedure SKILL_update_pst (nullgt cn vn lp ln rn)
	(printf "SKILL_update_pst: called with nullgt=%d cn=%s vn=%s lp=%s ln=%s rn=%s\n" nullgt cn vn lp ln rn)
)
(procedure SKILL_cancelCheckout_pre (nullgt cn vn lp ln rn)
	(printf "SKILL_cancelCheckout_pre: called with nullgt=%d cn=%s vn=%s lp=%s ln=%s rn=%s\n" nullgt cn vn lp ln rn)
)
(procedure SKILL_cancelCheckout_pst (nullgt cn vn lp ln rn)
	(printf "SKILL_cancelCheckout_pst: called with nullgt=%d cn=%s vn=%s lp=%s ln=%s rn=%s\n" nullgt cn vn lp ln rn)
)
