#!/usr/intel/bin/perl -w

use strict;

use Cwd 'realpath';

#==============================================================
#********************************************
# WARNING: THIS HAS TO REMAIN A perl SCRIPT *
#********************************************
# (not necessarily because we love perl but because c-shell will
# promptly close the read/write pipes and make dmserver not 
# be able to "talk" with the invoking application)
#
# The old wrappers are now simplified and they are all linked 
# to this one (sync_wrapper.pl). The name of the links is 
# important because it is used to see what to run.
#
# Thus <exe>_sync_wrapper -> sync_wrapper.pl
#  
# will invoke:
#
# $DMSEVER_BIN_DIR/<exe>
#
# Current wrappers: 
#   dmserver_sync_wrapper
#   idmb_sync_wrapper
#   idmSlave_sync_wrapper
#   itk_sync_wrapper
#
# First of all, why do we run the executables linking in 
# synchronicity API through a wrapper and not directly?
#
# Reason 1 (obsolete): needed to set LD_LIBRARY_PATH. This is
# -------- now ignored anyway because we are running with the 
#          setuid bit set which means all libraries have "burned
#          in" paths and all LD_ variables are ignored (OS
#          security reasons).
#          In fact, clearing the LD_LIBRARY_PATH here will prevent
#          a non -rpath linked executable to start and to
#          create problems later!!!
#          We do not care about TCLLIBPATH as none of our current
#          executables link in any tcl stuff.
#
# Reason 2 : need to source $SYNC_DIR/.syncinc to be able to
# -------- emulate exactly the behavior of dssc and stcl which
#          do this before they start.
#          As you will see below there's a weird snipet of code
#          that does this since perl does not much care if you
#          simply source a .csh file inside a perl script.
#
# Reason 3 : (debatable). Check settings at runtime. This could/
# -------- should be done at setup time since dmserver might hide
#          the error anyway. Things to check:
#          - the exes are linked in with "burned in" libraries
#          - $SYNC_DIR is set to same thing that was "burned in"
#            to avoid "interesting" behavior
#          - The exes have the setuid bit set so they can write
#            in cash
#          - The exes belong to the syncadm group (so they can
#            write in cash)
# 
#==============================================================

#----------------
# Initial checks
#----------------

my $SCRIPTDIR  = ($0 =~ /(.*)\/([^\/]+)/) ? $1 : ".";
my $SCRIPTNAME = $2 || $0;

if (! defined $ENV{"SYNC_DIR"}){
   print STDERR "-E- $SCRIPTNAME - SYNC_DIR not defined - cannot proceed\n";
   exit 1;
}

if (! defined $ENV{"DMSERVER_BIN_DIR"}){
   print STDERR "-E- $SCRIPTNAME - DMSERVER_BIN_DIR not defined - cannot proceed\n";
   exit 1;
}

my $SYNC_DIR = $ENV{"SYNC_DIR"};
my $DMSERVER_BIN_DIR = $ENV{"DMSERVER_BIN_DIR"};

my $EXE_NAME = $SCRIPTNAME;

$EXE_NAME =~ s/_sync_wrapper//;

my $EXE_FULL_PATH = "$DMSERVER_BIN_DIR/$EXE_NAME";

if (! -x "$EXE_FULL_PATH") {
   print STDERR "-E- $SCRIPTNAME - cannot find executable $EXE_FULL_PATH. Exiting\n";
   exit 1;
}

#-------------------------------
# "Sourcing" $SYNC_DIR/.syncinc
#-------------------------------

# This should become the perl "source" function
# 
# We are using csh -x to "capture" all the setenv lines and then issue $ENV{"var"} = value
# 
# We *do not* handle continuation chars and multiline vars settings. This is not at this moment
# intended to be a universal "source" replacement but just to deal with the "average" .syncinc
# Could enhace this in the near future.
#
# Handling things like:
#    setenv SYNC_HOSTNAME `hostname`
#    setenv SYNC_ARCH `"$SYNC_DIR/bin/sync_arch"`
#
# All detected variables are treated in the order received, i.e. if something is set multiple
# times, the last setting "wins".
#

my $SYNC_INC_FULLPATH = "$SYNC_DIR/.syncinc";

#
# To keep with the iDMS spirit we are using DEBUGDMS to spew more messages out
# in this case, just at STDOUT which might not be that useful for dmserver
#
# If I would be even nicer, I would also add them to the iDMS log file :-)
# Actually, that would be challenging since I do not know at this momment the process #
# for the executable which is not yet invoked and that is keyed in the file name
# Could add that if somebody deems it necessary.
# Otherwise, just run idmb and look at stdout.
#                                                       Sorin 6/18/2008
#

my $debug_mode = 0;

if (defined $ENV{'DEBUGDMS'}) {
   $debug_mode = 1;
}

if (! -x $SYNC_INC_FULLPATH) {
   goto FINAL_CHECKS;
}

# use another wrapper csh file to get env var from .syncinc
my $wrapper = $ENV{'DMSERVER_BIN_DIR'}."/../src/bundle/wrapper.sh";
open my $source,'-|',"csh $wrapper" or die "$!";
my $dump= do { local $/; <$source>};
my $VAR1;
eval $dump;

open (HANDLE,"/bin/csh -x $SYNC_INC_FULLPATH 2>&1 |");
while(<HANDLE>) {
   my $line = $_;
   if ($line =~ /^\s*setenv\s*(\S*)\s*(.*$)/){
      my $var = $1;
      $ENV{$var} = $VAR1->{$var};

      if ($debug_mode) {
         print("-I- $SCRIPTNAME - ENV{'$var'} = $ENV{$var}\n" );
      }
   }
}
close (HANDLE);

#----------------------
# Reset some variables
#----------------------

#
# Following servers two goals:
#
# 1. clears LD_LIBRARY_PATH so ldd can catch if libscapi.so was linked in properly (hardcoded path)
#    and not in the "classic" shared library way with "=> not found" which LD_LIBRARY_PATH
#
# 2. since LD_LIBRARY_PATH is cleared, we will prevent partly linked exes (any of libraries with "not found")
#    to try to execute. Not that this is even possible in the setuid bit is set but "only the paranoid survive".
#


$ENV{'LD_LIBRARY_PATH'} = "";

# clearing TCLLIBPATH too since none of our apps use tcl anyway

$ENV{'TCLLIBPATH'} = "";


#---------------
# Final checks
#---------------

# Will just check if SYNC_DIR/executable are matching. The other checks should be/are done in dmserver.setup
# Just implementing the first 2 bullets of Reason 3

FINAL_CHECKS:

my $scapi_string = `/usr/bin/ldd $EXE_FULL_PATH | /bin/grep scapi`;
chomp($scapi_string);
# should return something like:
#     libscapi.so => /p/dpg/unified/proliferable/ds/4.2sp3_nhm/syncinc/lib.Linux/libscapi.so (0x55625000)
#
# -OR- (this is bad and will err out):
#     libscapi.so => not found
#
#     This codition should only happen if the executable was linked without rpath or if it was "burned in" with
#     a site specific path, say looking for /nfs/pdx/... in SC
# 
# If we are not finding any scapi we are silently ignoring it. Either something is totally wrong or we
# linked in static. Probably the year is 2010 or higher because util mid 2008 did not see any synchronicity
# static libs

$scapi_string =~ s/^.*=>\s*//;
## changed for for V6R2013x, 
#$scapi_string =~ s/syncinc\/.*$/syncinc/;
#      print STDERR "\n-E- $SCRIPTNAME - scapi_string=$scapi_string\n";  

# Assumes that from now to Eternity or until you make the script smarter, synchronicity is installed in
# some directory like <dir>/<version>/syncinc/...

# Hopefully following error messages "make an impact" :-) Would definitelly annoy me.
# I've checked in platocbd and they show up in the invocation xterm window...

if ("$scapi_string" eq "") {
   goto RUN_EXE;
}

if ("$scapi_string" eq "not found"){
   my $hard_coded_string = `/usr/bin/strings -a $EXE_FULL_PATH | /usr/bin/tr ":" "\n" | /bin/grep lib.Linux`;
   chomp($hard_coded_string);
   if ("$hard_coded_string" eq "") {
      print STDERR "\n-E- $SCRIPTNAME - *******************************************************************************************\n";  
      print STDERR   "-E- $SCRIPTNAME - executable $EXE_FULL_PATH was not linked with -rpath\n";
      print STDERR   "-E- $SCRIPTNAME - please run relink.csh first and then rerun (*need syncadm priviledges*)\n";
      print STDERR   "-E- $SCRIPTNAME - *******************************************************************************************\n\n";
   }
   else {
      print STDERR "\n-E- $SCRIPTNAME - *******************************************************************************************\n";
      print STDERR "-E- $SCRIPTNAME - executable $EXE_FULL_PATH was linked with a path not avaiable on this system:\n\n";
      print STDERR "-E- $SCRIPTNAME - $hard_coded_string\n\n";
      print STDERR "-E- $SCRIPTNAME - Please make sure it was not linked with a site specific path.\n";
      print STDERR "-E- $SCRIPTNAME - You can rerun relink.csh first and then rerun (*need syncadm priviledges*)\n";
      print STDERR "-E- $SCRIPTNAME - *******************************************************************************************\n\n";

   }
   exit 1; 
}

#
# before comparing paths, make sure we solve the links
#

my $rp_SYNC_DIR = realpath($SYNC_DIR);
my $rp_scapi_string = realpath($scapi_string);

#if ("$rp_SYNC_DIR" ne "$rp_scapi_string") {
if ("$rp_SYNC_DIR" =~ "$rp_scapi_string") {
   
   print STDERR "\n-E- $SCRIPTNAME - *******************************************************************************************\n"; 
   print STDERR   "-E- $SCRIPTNAME - SYNC_DIR ($SYNC_DIR) is set to a different value than\n";
   print STDERR   "-E- $SCRIPTNAME - the one linked in $EXE_NAME ($scapi_string)\n";
   print STDERR   "-E- $SCRIPTNAME - this causes unpredictable behavior so you need to either:\n";
   print STDERR   "-E- $SCRIPTNAME - a) setenv \$SYNC_DIR $scapi_string and rerun -OR-\n";
   print STDERR   "-E- $SCRIPTNAME - b) \$DMSERVER_BIN_DIR/relink.csh -bin_dir \$DMSERVER_BIN_DIR -sync_dir \$SYNC_DIR\n";
   print STDERR   "-E- $SCRIPTNAME      (option b) needs syncadm priviledges)\n";
   print STDERR   "-E- $SCRIPTNAME - *******************************************************************************************\n\n";
   exit 1;
}

#------------------
# Run executable
#------------------

RUN_EXE:

if ($debug_mode) {
         print("-I- $SCRIPTNAME - running $EXE_FULL_PATH @ARGV\n");
}

if (defined $ENV{DMSERVER_VALGRIND}) {
  exec "/p/dt/sde/bin/valgrind.latest --tool=memcheck --num-callers=15 --error-limit=no --trace-children=yes --log-file=log.%p $EXE_FULL_PATH @ARGV";
  exit 1;
} 


if (defined $ENV{DMSERVER_DEBUGGER}) {
   exec ("$ENV{DMSERVER_DEBUGGER} -nx --args $EXE_FULL_PATH  @ARGV");
}
else {
   exec("$EXE_FULL_PATH", @ARGV);
}



