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
# $Id: fwview.h,v 1.1.14.1 1997/12/12 17:36:08 sorin Exp $
#                                                                     #
#######################################################################
#######################################################################
*/
/*
#Revision 1.1  93/01/04  09:50:50  cchiu
#Initial revision
#*/

/*
  MACRO of VIEWTYPES
*/
#define		viewTypeAct	"act"
#define		viewTypeAlf	"alf"
#define		viewTypeApl	"apl"
#define		viewTypeAuxAttr "aux_attributes"
#define		viewTypeBa	"ba"
#define		viewTypeBnc	"bnc"
#define		viewTypeBlifLog	"blif_logicfunc"
#define		viewTypeBlst	"blst"
#define		viewTypeBusPrtl	"bus_protocol"
#define		viewTypeCellLst	"cell_list"
#define		viewTypeCur	"cur"
#define		viewTypeCvsLay	"cvslayout"
#define		viewTypeDirtiv	"directive"
#define		viewTypeDirect	"directives"
#define		viewTypeDrcerr	"drcerr"
#define		viewTypeEstex	"estex"
#define		viewTypeEdifNet	"edif_netlist"
#define		viewTypeErcGdbi	"erc_GDB_input"
#define		viewTypeErcRslt	"erc_results"
#define		viewTypeErcTbl	"erc_table"
#define		viewTypeGlbSig	"glb_globalSignals"
#define		viewTypeGrpGrp	"grp_group"
#define		viewTypeIcf	"icf"
#define		viewTypeIpfPar	"ipf_parasitics"
#define		viewTypeIspNet	"ispcmcro_netlist"
#define		viewTypeLayHier	"layout_hierarchy"
#define		viewTypeLifLog	"lif_logicfunc"
#define		viewTypeMainFrz	"mainsail_freeze"
#define		viewTypeMainObj	"mainsail_obj"
#define		viewTypeMap	"map"
#define		viewTypeMaskLay	"maskLayout"
#define		viewTypeNetPar	"netcell_parasitics"
#define		viewTypeNetcvs	"netcvs"
#define		viewTypeCvsNetl	"netcvs_netlist"
#define		viewTypeCvsNetc	"netcvs_netcvs"
#define		viewTypeNetNetl	"netlist_netlist"
#define		viewTypeNets	"nets"
#define		viewTypeOlc	"olc"
#define		viewTypePtfTime	"ptf_timing"
#define		viewTypePlusTec	"plus_tech"
#define		viewTypePlusCmd	"plus_commands"
#define		viewTypeRcdPar	"rcd_parasitics"
#define		viewTypeSchNet	"sch_netlist"
#define		viewTypeSch	"schematic"
#define		viewTypeSchSymb	"schematicSymbol"
#define		viewTypeSdp	"sdp"
#define		viewTypeText	"text"
#define		viewTypePcsFile	"ProcessFile"
#define		viewTypeVerilog	"verilog"
#define		viewTypeViolate	"violations"

/*
  MACRO of VIEWS
*/
#define		viewAct		"act"
#define		viewAlflay	"alflay"
#define		viewAlfpln	"alfpln"
#define		viewApl		"apl"
#define		viewAux		"aux"
#define		viewBa		"ba"
#define		viewBlif	"blif"
#define		viewBlsf	"blsf"
#define		viewBlst	"blst"
#define		viewBlsy	"blsy"
#define		viewBnc		"bnc"
#define		viewBnk		"bnk"
#define		viewBus		"bus"
#define		viewBvr		"bvr"
#define		viewBvrlss	"bvrlss"
#define		viewCdf		"cdf"
#define		viewChk		"chk"
#define		viewChprdb	"chprdb"
#define		viewCl		"cl"
#define		viewClk		"clk"
#define		viewCmd		"cmd"
#define		viewCur		"cur"
#define		viewD2f		"d2f"
#define		viewDaprdb	"daprdb"
#define		viewDef		"def"
#define		viewDev		"dev"
#define		viewDir		"dir"
#define		viewDlsdb	"dlsdb"
#define		viewDrcerr	"drcerr"
#define		viewEdif	"edif"
#define		viewEmisp	"emisp"
#define		viewEmvio	"emvio"
#define		viewErcexl	"ercexl"
#define		viewErcflw	"ercflw"
#define		viewErcgdb	"ercgdb"
#define		viewErcint	"ercint"
#define		viewErcrep	"ercrep"
#define		viewErcset	"ercset"
#define		viewErctpl	"erctpl"
#define		viewEstex	"estex"
#define		viewEvr		"evr"
#define		viewEvrlss	"evrlss"
#define		viewExlif	"exlif"
#define		viewFrz		"frz"
#define		viewGlb		"glb"
#define		viewGraph	"graph"
#define		viewGroup	"group"
#define		viewGrp 	"grp"
#define		viewGux		"gux"
#define		viewH4		"h4"
#define		viewHir		"hir"
#define		viewHdl		"hdl"
#define		viewHux		"hux"
#define		viewIcf		"icf"
#define		viewIfc		"ifc"
#define		viewIif		"iif"
#define		viewInc		"inc"
#define		viewIpf		"ipf"
#define		viewIsp		"isp"
#define		viewIspcmcro	"ispcmcro"
#define		viewLayout	"layout"
#define		viewLif		"lif"
#define		viewLn		"ln"
#define		viewLst		"lst"
#define		viewM4		"m4"
#define		viewMap		"map"
#define		viewMin		"min"
#define		viewNetcvs	"netcvs"
#define		viewNets	"nets"
#define		viewNtcl	"ntcl"
#define		viewObj		"obj"
#define		viewOlc		"olc"
#define		viewOrglif	"orglif"
#define		viewPerfdr	"perfdr"
#define		viewPrm		"prm"
#define		viewRcd		"rcd"
#define		viewSch		"sch"
#define		viewSchematic	"schematic"
#define		viewSdp		"sdp"
#define		viewSn		"sn"
#define		viewSnet	"snet"
#define		viewSp		"sp"
#define		viewSymbol	"symbol"
#define		viewTech	"tech"
#define		viewTyp		"typ"
#define		viewUpf		"upf"
#define		viewUux		"uux"
#define		viewVerilog	"verilog"
#define		viewXne		"xne"
