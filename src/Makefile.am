ACLOCAL_AMFLAGS = -I m4
bin_PROGRAMS = dmserver_unfixed dmserver_novendor idmSlave_unfixed idmSlave_novendor idmb_unfixed idmb_novendor itk_unfixed itk_novendor server_ipc patchelf
common_cppflags = -DOH_MACH_LINUX -D_GNU_SOURCE=1 -fno-strict-aliasing -pipe -O0 -DBOO_INSTANTIATE_TEMPLATE -DDMCLIENT_NO_AFS -DDMSVERSION=\"$(VERSION_ID)\"  -DYTABLE_PROTO\(x\)=x -DiDMSYY_PROTO\(x\)=x -DYY_PROTO\(x\)=x -DNEW_LIB_STRUCTURE -I$(srcdir)/include -I$(srcdir)/include/p2pitc -I$(srcdir)/include/turbo -I$(srcdir)/dmserver/src/iDMS -I$(srcdir)/iDMS/p2pitc -I$(srcdir)/iDMS -I$(srcdir)/platform/include -I$(srcdir)/dmserver/include -I$(srcdir)/iDMS_nosync $(FPIC_CPPFLAGS) $(STACK_CPPFLAGS)
lib_LTLIBRARIES =
common_ldflags = $(FPIE_LDFLAGS) $(STACK_LDFLAGS)

#fwidms_c_RENAMES = -DYY=iDMSYY -Dyy=iDMSyy
#table_c_RENAMES = -DYY=YTABLE -Dyy=ytable

#dmserver_sources = dmserver/datestamp.c dmserver/dmbrowser44.c  dmserver/hashUtils.c dmserver/interp2p.c \
#  dmserver/interprocs44.c dmserver/interutls.c dmserver/log.c dmserver/ntsock.c dmserver/opusp2p.c dmserver/p2p.c \
#  dmserver/server_ipc.c dmserver/timer.c turbo/dmsT_API.cpp turbo/dmsT_manager.cpp
#
#iDMS_sources = iDMS/fwidms.yacc iDMS/table.yacc \
#  iDMS/dmsC_internal_functions.c iDMS/TimeOutput.c iDMS/dmsC_API_1.c \
#  iDMS/fwconfig.c iDMS/dmsC_triggers.c \
#  iDMS/dbC_functions.c iDMS/dmsC_API_new.c iDMS/table_functions.c \
#  iDMS/fwusertrigger.c iDMS/os_UNIX.c iDMS/fwtrigger.c \
#  iDMS/dmsC_functions2.c iDMS/dmsC_Cadence_spec.c \
#  iDMS/idmb_internal_functions.c iDMS/fwitktrack.c iDMS/idb_functions.c \
#  iDMS/fwidmb.c iDMS/dmsC_attr_dummy.c

generated_sources = dmspath_yacc.c dmspath_lex.c dmspath.tab.h table_yacc.c table.tab.h table_lex.c dummy.cpp
CLEANFILES = $(generated_sources)

dmspath_yacc.c: iDMS/fwidms.yacc
	$(YACC) -d $< -o $@ -p iDMSyy

dmspath.tab.h: dmspath_yacc.c
	mv dmspath_yacc.h $@
	touch $@

dmspath_lex.c: iDMS/fwidms.lex dmspath.tab.h
	$(LEX) -odmspath_lex.c -PiDMSyy $(srcdir)/iDMS/fwidms.lex

table_yacc.c: iDMS/table.yacc
	$(YACC) -d $< -o $@ -p ytable

table.tab.h: table_yacc.c
	mv table_yacc.h $@
	touch $@

table_lex.c: iDMS/table.lex table.tab.h
	$(LEX) -otable_lex.c -Pytable $(srcdir)/iDMS/table.lex

# Force g++ linking instead of spcifying lstdc++ manually
dummy.cpp:
	touch $@

#common_sources = $(dmserver_sources) $(iDMS_sources)
common_sources = \
iDMS/dbC_functions.c \
iDMS/dmsC_API_1.c \
iDMS/dmsC_API_new.c \
iDMS/dmsC_dm.c \
iDMS/dmsC_functions2.c \
iDMS/dmsC_internal_functions.c \
iDMS/dmsC_reports.c \
iDMS/dmsC_status.c \
iDMS/dmsC_triggers.c \
iDMS/fwidms.yacc \
iDMS/fwidms.lex \
iDMS/fwattr.c \
iDMS/fwconfig.c \
iDMS/fwidmb.c \
iDMS/fwitktrack.c \
iDMS/fwmem.c \
iDMS/fwtrigger.c \
iDMS/fwusertrigger.c \
iDMS/idb_functions.c \
iDMS/idmb_internal_functions.c \
iDMS/os_UNIX.c \
iDMS/table_functions.c \
turbo/dmsT_API.cpp \
turbo/dmsT_manager.cpp \
dmserver/datestamp.c \
dmserver/dmbrowser44.c \
dmserver/hashUtils.c \
dmserver/interp2p.c \
dmserver/interprocs44.c \
dmserver/interutls.c \
dmserver/log.c \
dmserver/ntsock.c \
dmserver/opusp2p.c \
dmserver/p2p.c \
dmserver/server_ipc.c \
dmserver/timer.c

# Massive cross dependency, using an archive to allow linker to scan and resolve symbols recursively
noinst_LIBRARIES = libsync.a libnosync.a

libsync_a_SOURCES = $(common_sources)
nodist_libsync_a_SOURCES = $(generated_sources)
libsync_a_CPPFLAGS = $(common_cppflags) -I$(SYNCHRONICITY_CPPFLAGS) 

libnosync_a_SOURCES = $(common_sources)
nodist_libnosync_a_SOURCES = $(generated_sources)
libnosync_a_CPPFLAGS = $(common_cppflags) -DDISABLE_SYNCHRONICITY=1 -I$(SYNCHRONICITY_CPPFLAGS) 

dmserver_unfixed_SOURCES = dmserver/dmserver.c 
nodist_dmserver_unfixed_SOURCES = dummy.cpp
dmserver_unfixed_CPPFLAGS = $(common_cppflags) -I$(SYNCHRONICITY_CPPFLAGS) 
dmserver_unfixed_LDADD = -L. libsync.a $(SYNCHRONICITY_LIBS) -ldl
dmserver_unfixed_LDFLAGS = $(common_ldflags) $(SYNCHRONICITY_LDFLAGS)

dmserver_novendor_SOURCES = dmserver/dmserver.c
nodist_dmserver_novendor_SOURCES = dummy.cpp
dmserver_novendor_CPPFLAGS = $(common_cppflags) -DDISABLE_SYNCHRONICITY=1 -I$(SYNCHRONICITY_CPPFLAGS)
dmserver_novendor_LDADD = -L. libnosync.a -lfl $(libcxx) -ldl
dmserver_novendor_LDFLAGS = $(common_ldflags)

idmSlave_unfixed_SOURCES = idmSlave/idmSlave.c
nodist_idmSlave_unfixed_SOURCES = dummy.cpp
idmSlave_unfixed_CPPFLAGS = $(common_cppflags) -I$(SYNCHRONICITY_CPPFLAGS) 
idmSlave_unfixed_LDADD = -L. libsync.a $(SYNCHRONICITY_LIBS) -ldl
idmSlave_unfixed_LDFLAGS = $(common_ldflags) $(SYNCHRONICITY_LDFLAGS)

idmSlave_novendor_SOURCES = idmSlave/idmSlave.c
nodist_idmSlave_novendor_SOURCES = dummy.cpp
idmSlave_novendor_CPPFLAGS = $(common_cppflags) -DDISABLE_SYNCHRONICITY=1 -I$(SYNCHRONICITY_CPPFLAGS) 
idmSlave_novendor_LDADD = -L. libnosync.a -lfl $(libcxx) -ldl
idmSlave_novendor_LDFLAGS = $(common_ldflags)

idmb_unfixed_SOURCES = idmb/fwidmbmain.c
nodist_idmb_unfixed_SOURCES = dummy.cpp
idmb_unfixed_CPPFLAGS = $(common_cppflags) -I$(SYNCHRONICITY_CPPFLAGS) -I$(SYNCHRONICITY_CPPFLAGS) 
idmb_unfixed_LDADD = -L. libsync.a $(SYNCHRONICITY_LIBS) -ldl
idmb_unfixed_LDFLAGS = $(common_ldflags) $(SYNCHRONICITY_LDFLAGS)

idmb_novendor_SOURCES = idmb/fwidmbmain.c
nodist_idmb_novendor_SOURCES = dummy.cpp
idmb_novendor_CPPFLAGS = $(common_cppflags) -DDISABLE_SYNCHRONICITY=1 -I$(SYNCHRONICITY_CPPFLAGS) 
idmb_novendor_LDADD = -L. libnosync.a -lfl $(libcxx) -ldl
idmb_novendor_LDFLAGS = $(common_ldflags)

itk_unfixed_SOURCES = itk/fwitk.c
nodist_itk_unfixed_SOURCES = dummy.cpp
itk_unfixed_CPPFLAGS = $(common_cppflags) -I$(SYNCHRONICITY_CPPFLAGS) -ldl 
itk_unfixed_LDADD = -L. libsync.a $(SYNCHRONICITY_LIBS)
itk_unfixed_LDFLAGS = $(common_ldflags) $(SYNCHRONICITY_LDFLAGS)

itk_novendor_SOURCES = itk/fwitk.c
nodist_itk_novendor_SOURCES = dummy.cpp
itk_novendor_CPPFLAGS = $(common_cppflags) -DDISABLE_SYNCHRONICITY=1 -I$(SYNCHRONICITY_CPPFLAGS) 
itk_novendor_LDADD = -L. libnosync.a -lfl -ldl
itk_novendor_LDFLAGS = $(common_ldflags)

#server_ipc_SOURCES = server_ipc/CellViewIOMgr.cpp server_ipc/DmsMgrIdms.cpp server_ipc/DmsPathIdms.cpp dmserver/dmbrowser44.c
server_ipc_SOURCES = server_ipc_dir/server_ipc.c
#nodist_server_ipc_SOURCES = dummy.cpp
server_ipc_CPPFLAGS = $(common_cppflags) -DSERVER_IPC=1
server_ipc_LDADD = -L. libnosync.a -ldl
server_ipc_LDFLAGS = $(common_ldflags)

patchelf_SOURCES = patchelf_dir/patchelf.cc
patchelf_CPPFLAGS = -DPACKAGE_STRING='"patchelf 0.7-21828ee8cc"' $(FPIC_CPPFLAGS) $(STACK_CPPFLAGS)
patchelf_LDFLAGS = $(common_ldflags)

noinst_LTLIBRARIES = libiDMS.la
libiDMS_la_SOURCES = $(common_sources) $(generated_sources)
libiDMS_la_CPPFLAGS = $(common_cppflags) -I$(SYNCHRONICITY_CPPFLAGS)
#libiDMS_la_LDFLAGS = $(common_ldflags)  

#noinst_LTLIBRARIES = libiDMS.la
#libiDMS_la_SOURCES = $(common_sources) 
#nodist_libiDMS_la_SOURCES = $(generated_sources)
#libiDMS_la_CPPFLAGS = $(common_cppflags)  -I$(SYNCHRONICITY_CPPFLAGS)

test_sources = \
../tests/check_fwidmb.c \
../tests/check_main.c \
../tests/check_check.h \
../tests/check_dmsC_API_1.c \
../tests/check_os_UNIX.c \
../tests/check_dmsC_API_new.c \
../tests/check_table_functions.c \
../tests/check_dmsC_functions2.c \
../tests/check_dmsC_dm.c \
../tests/check_idb_functions.c \
../tests/check_dmsC_status.c 
#TESTS =  check_dmserver
#check_PROGRAMS = check_dmserver
#check_dmserver_SOURCES = $(test_sources)
#check_dmserver_CFLAGS =  $(common_cppflags) -DDISABLE_SYNCHRONICITY=1 -I$(SYNCHRONICITY_CPPFLAGS) @CHECK_CFLAGS@
#check_dmserver_LDADD =   -L. libiDMS.la -lfl $(libcxx) -ldl @CHECK_LIBS@
#check_dmserver_LDFLAGS = $(common_ldflags) $(SYNCHRONOCITY_LDFLAGS)
 
TESTS =  check_dmserver
check_PROGRAMS = check_dmserver
check_dmserver_SOURCES = $(test_sources)
check_dmserver_CPPFLAGS = $(common_cppflags) -I$(SYNCHRONICITY_CPPFLAGS)
nodist_check_dmserver_SOURCES = dummy.cpp
check_dmserver_CFLAGS = @CHECK_CFLAGS@
check_dmserver_LDADD = -L. libiDMS.la $(SYNCHRONICITY_LIBS) -ldl @CHECK_LIBS@
check_dmserver_LDFLAGS = $(common_ldflags)  $(SYNCHRONOCITY_LDFLAGS)


LCOV_INPUT      = $(test_sources:%.c=.libs/%.gcda)
LCOV_OUTPUT     = lcov.info
LCOV_HTML       = lcov_html
LCOV_LCOV       = @LCOV@
LCOV_GENHTML    = @GENHTML@

lcov: $(LCOV_HTML)
check:lcov
$(LCOV_INPUT): libiDMS.la

$(LCOV_OUTPUT): $(LCOV_INPUT)
	$(LCOV_LCOV) --capture --directory $(val)/iDMS/.libs --base-directory . --output-file $@

$(LCOV_HTML): $(LCOV_OUTPUT)
	-$(RM) -r $@
	LANG=C $(LCOV_GENHTML) --output-directory $@ --function-coverage  --title "DM Server iDMS Code Coverage" --show-details $<
	@echo "Point a web browser at $(val)/$(LCOV_HTML)/index.html to see results."

clean-local: lcov-clean

.PHONY: lcov-clean
lcov-clean:
	-$(RM) -r $(LCOV_HTML) $(LCOV_OUTPUT)

install-exec-hook:
	-/bin/chmod u+s $(bindir)/dmserver_unfixed $(bindir)/idmSlave_unfixed $(bindir)/idmb_unfixed $(bindir)/itk_unfixed $(bindir)/dmserver_novendor $(bindir)/idmSlave_novendor $(bindir)/idmb_novendor $(bindir)/itk_novendor
	-/bin/rm -f $(bindir)/dmserver_sync_wrapper $(bindir)/idmSlave_sync_wrapper $(bindir)/idmb_sync_wrapper $(bindir)/itk_sync_wrapper $(bindir)/dmsapi.tcl
	$(LN_S) dmsapi.tc $(bindir)/dmsapi.tcl
	$(LN_S) sync_wrapper.pl $(bindir)/dmserver_sync_wrapper
	$(LN_S) sync_wrapper.pl $(bindir)/idmSlave_sync_wrapper
	$(LN_S) sync_wrapper.pl $(bindir)/idmb_sync_wrapper
	$(LN_S) sync_wrapper.pl $(bindir)/itk_sync_wrapper

idmsdocdir = $(prefix)/iDMS_doc/iDMS_doc

bundledir = $(prefix)/bin
dist_bundle_SCRIPTS = \
bundle/DMSApi.pm \
bundle/tclIndex \
bundle/DMSApi.tcl \
bundle/dmsapi.tc \
bundle/sync_wrapper.pl \
bundle/dmsapi \
bundle/cwshell

EXTRA_DIST = \
include/Dms/fwierr.h \
include/Dms/DmsTemplatesSimple.cpp \
include/Dms/DmsTemplates.cpp \
include/Dms/DmsLib.h \
include/Dms/code_review_log \
include/Dms/DmsSimpleLib.h \
include/Dms/DmsMgr.h \
include/Dms/DmsPathIdms.h \
include/Dms/DmsMgrSimple.h \
include/Dms/DmsPath.h \
include/Dms/DmsMgrIdms.h \
include/Dms/DmsPathSimple.h \
include/Dms/fwidms.h \
include/CellView/CellViewPrimList.h \
include/CellView/CellViewMgr.h \
include/CellView/CellViewReader.h \
include/CellView/CellViewBasics.h \
include/CellView/CellViewIOMgr.h \
include/CellView/CellViewDmsMgr.h \
include/CellView/CellViewDmsPath.h \
include/CellView/CellViewWriter.h \
include/CellView/CellViewRWMgr.h \
include/CellView/CellViewLib.h \
include/p2pitc/log.h \
include/p2pitc/server_ipc.h \
include/p2pitc/ntsock.h \
include/p2pitc/timer.h \
include/p2pitc/code_review_log \
include/p2pitc/interp2p.h \
include/p2pitc/p2p.h \
include/p2pitc/opusp2p.h \
include/turbo/dmsT_API.h \
include/turbo/dmsT_manager.h \
iDMS/attr.h \
iDMS/coral_API.h \
iDMS/daEnumVal.h \
iDMS/damCheck.h \
iDMS/dbC.h \
iDMS/dmsC.h \
iDMS/dmsC_Cadence_spec.h \
iDMS/dmsC_defines.h \
iDMS/dmsC_dm.h \
iDMS/dmsC_functions2.h \
iDMS/dmsC_internal_functions.h \
iDMS/dmsC_new.h \
iDMS/dmsC_new_vars.h \
iDMS/dmsC_parser_proto.h \
iDMS/dmsC_reports.h \
iDMS/dmsC_status.h \
iDMS/dmsC_triggers.h \
iDMS/dmsC_types.h \
iDMS/fwidms.h \
iDMS/fwierr.h \
iDMS/fwimsg.h \
iDMS/fwitk.h \
iDMS/fwitktrack.h \
iDMS/fwtable.h \
iDMS/fwview.h \
iDMS/hi.h \
iDMS/idb_codes.h \
iDMS/idb_functions.h \
iDMS/idmb_internal_functions.h \
iDMS/idmb_variables.h \
iDMS/ifw_functions.h \
iDMS/iitkDA.h \
iDMS/iitkDApvt.h \
iDMS/il.h \
iDMS/os_functions.h \
iDMS/table_functions.h \
iDMS/voTimeCorrection.h \
dmserver/dmserver.h \
dmserver/hashUtils.h \
dmserver/interprocs.h \
dmserver/interutls.h \
iDMS/table.yacc \
iDMS/fwidms.lex \
iDMS/table.yacc \
iDMS/table.lex \
patchelf_dir/elf.h \
bundle/build2013.sh

