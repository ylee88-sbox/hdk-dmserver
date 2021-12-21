#! /usr/intel/pkgs/perl/5.14.1-threads/bin/perl

use strict;
use warnings;
use FindBin;
use Getopt::Long;
use File::Path qw(make_path remove_tree);
use File::Basename;
use File::Copy;
use POSIX ":sys_wait_h";
use Carp;
use DateTime;
use Cwd;

use File::Spec::Functions qw(rel2abs);
use File::Basename;

my $pwd = dirname(rel2abs($0));
$pwd = $pwd."/..";

require "$pwd/tests/utils/mock_design_sync.pl";

my $stderr_handle;
my $verbose = 0;
my $debug = 0;
my $bits32 = 0;
my $ut = 0;

#my $pwd = $1."/..";
sub abnormal_exit{
  $Carp::Verbose = 1;
  $Carp::MaxArgLen = 0;
  print $stderr_handle "*** --- ERROR NOTICE ---\n";
  print $stderr_handle Carp::shortmess();
  print $stderr_handle "*** --- ERROR NOTICE ---\n";
}

open($stderr_handle, ">&STDERR") || die "cannot open stderr";

my ($opt_prefix,$opt_sync_dir,$root);
my $build_temp = "/tmp/dmserver.$>.$$";
$root = (fileparse($FindBin::Bin))[1];

my $revision = '';
if( -f "$root/setup/revision" ) {
  open(my $rev, q{<}, "$root/setup/revision");
  chomp(my $r = <$rev>);
  close $rev;

  if($r =~ m/^[0-9a-f]{40}$/i) {
    $revision = $r;
  } else {
    goto revision_fail;
  }
} else {
  revision_fail:
  $revision = 'opus44ww_default_' . DateTime->now->iso8601();
}

GetOptions(
  'sync_dir=s' => \$opt_sync_dir,
  'bin_dir=s' => \$opt_prefix,
  'verbose' => \$verbose,
  'debug' => \$debug,
  '32bit' => \$bits32,
  'ut' => \$ut,
) || die("Error in command line arguments\n");

$opt_prefix = (fileparse($opt_prefix))[1] if defined($opt_prefix); # highest priority
$opt_prefix = $root if !defined($opt_prefix);

$opt_sync_dir = $ENV{SYNC_DIR} if !defined $opt_sync_dir;

  $ENV{PATH} = '/usr/intel/pkgs/autotools/2.69/bin:' . $ENV{PATH};
  $ENV{PKG_CONFIG_PATH} = "$pwd/check/lib/pkgconfig";
  $ENV{PATH} = '/usr/intel/pkgs/lcov/1.11/bin:' . $ENV{PATH}; 
  $ENV{CK_DEFAULT_TIMEOUT} = 0;

if($ut){
  # Update check.pc prefix path
  my $check_path = $pwd."/check/lib/pkgconfig/check.pc";
print("2-- $check_path\n");
#exit 0;
  if (-e $check_path){
    open my $in,  '<',  $check_path or die "Cant read old file: $!";
    open my $out, '>', "/tmp/check.pc.new" or die "Cant write new file: $!";

    my $count =0;
    my $msg = "prefix=".$pwd."/check\n";
    print $out $msg;

    while( <$in> )
    {
      if($count>0){
        print $out $_;
      }
       $count++;
    }
    close $out; close $in;

    # overwrite original with modified copy
    if(!copy("/tmp/check.pc.new", $check_path)){
      printf("Error updating check.pc. Fix this error or disable unit test run\n");
      exit 0;
    }
  }
  
  # Update libcheck.la libdir path
  my $libcheck_path = $pwd."/check/lib/libcheck.la";

  if (-e $libcheck_path){
    open my $libin,  '<',  $libcheck_path or die "Cant read old file: $!";
    open my $libout, '>', "/tmp/libcheck.la.new" or die "Cant write new file: $!";

    my $libmsg= "libdir=\'".$pwd."/check/lib\'\n";

    while( <$libin> )
    {
      print $libout $_ if !eof;
      # $libcount++;
      print $libout $libmsg if eof;
    }
    close $libout; close $libin;

    # overwrite original with modified copy
    if(!copy("/tmp/libcheck.la.new", $libcheck_path)){
      printf("Error updating libcheck.la. Fix this error or disable unit test run\n");
      exit 0;
    }
  }
}

my $sync_include = ( -d "$opt_sync_dir/linux_a64/scapi" ) ?
  "$opt_sync_dir/linux_a64/scapi" :
  "$opt_sync_dir/scapi";

my $sync_lib = ( -d "$opt_sync_dir/linux_a64/lib.Linux" ) ?
  "$opt_sync_dir/linux_a64/lib.Linux" :
  "$opt_sync_dir/lib.Linux";

my $sync_bin = ( -d "$opt_sync_dir/linux_a64/code/bin" ) ?
  "$opt_sync_dir/linux_a64/code/bin" :
  "$opt_sync_dir/code/bin";

# sanity check
-f "$sync_include/scapi.h" || die "Please check your --sync_dir arg or SYNC_DIR env var";
-f "$sync_lib/libscapi.so" || die "Please check your --sync_dir arg or SYNC_DIR env var";

remove_tree($build_temp) if ! -d $build_temp;
mkdir $build_temp;
chdir $build_temp;

# configure
printf "Building in %s, this will take some time\n", $build_temp;
printf "Will install into %s\n", $opt_prefix;

my ($fh,$kid);
open($fh,q{>},'build.log') || die "can't write log: $!";

my $pid = open($kid,q{-|});
defined($pid) || die "can't fork: $!";

my $cflags_debug;
my $cxxflags_debug;
my $cflags;
my $cxxflags;

if ($ut){
  $cflags_debug = $bits32 ? "CFLAGS=-g3 -m32 -O0 -fprofile-arcs -ftest-coverage" : "CFLAGS=-g3 -O0  -fprofile-arcs -ftest-coverage";
  $cxxflags_debug = $bits32 ? "CXXFLAGS=-g3 -m32 -O0  -fprofile-arcs -ftest-coverage" : "CXXFLAGS=-g3 -O0  -fprofile-arcs -ftest-coverage";
  $cflags = $bits32 ? "CFLAGS=-g3 -m32 -O2  -fprofile-arcs -ftest-coverage" : "CFLAGS=-g3 -O2  -fprofile-arcs -ftest-coverage";
  $cxxflags = $bits32 ? "CXXFLAGS=-g3 -m32 -O2  -fprofile-arcs -ftest-coverage" : "CXXFLAGS=-g3 -O2  -fprofile-arcs -ftest-coverage";
} else {
  $cflags_debug = $bits32 ? "CFLAGS=-g3 -m32 -O0" : "CFLAGS=-g3 -O0";
  $cxxflags_debug = $bits32 ? "CXXFLAGS=-g3 -m32 -O0" : "CXXFLAGS=-g3 -O0";
  $cflags = $bits32 ? "CFLAGS=-g3 -m32 -O2" : "CFLAGS=-g3 -O2";
  $cxxflags = $bits32 ? "CXXFLAGS=-g3 -m32 -O2" : "CXXFLAGS=-g3 -O2";
}

if($pid) {
  while(<$kid>){ print $fh $_; print "[Build] $_" if $verbose }
  close $fh;
  close $kid;
  print "Build exited with status $?\n";
  printf "Build completed successfully\n" if (! $?);
} else {

  # GCC 4.5.1 is known to work on SLES10/SLES11
  $ENV{PATH} = '/usr/intel/pkgs/gcc/4.7.2/bin:' . $ENV{PATH};
  my $gcc_ver = '/usr/intel/pkgs/gcc/4.7.2/bin';
  # Someone thought of occupying with /p/hdk/cad/cadence/6.16.060
  $ENV{INSTALL} = '';

  # Some retard linked "/usr/pkgs/gcc/4.5.1/bin/ar" to "gas" thinking they do the same function
  $ENV{AR} = '/usr/bin/ar';

  $SIG{__DIE__} = \&abnormal_exit;

  make_path($opt_prefix, { verbose => 1, });
  foreach(qw(setup src bin)) {
    # reference copy if anyone interested in viewing it
    system("/bin/cp", "-fpr", "$root/$_","$opt_prefix/$_") if ( ! -d "$opt_prefix/$_" );
    die "Failed to copy to $opt_prefix/$_" if $?;
  }

  open(STDERR,'>&',STDOUT); # do not alarm the fragile user
  system("$opt_prefix/src/configure", "--prefix=$opt_prefix", "--with-synchronicity_header=$sync_include",
    "--with-synchronicity_ldflags=-L${sync_lib} -L${sync_bin} -Wl,-rpath,$sync_lib -Wl,-rpath,$sync_bin",
    "--with-synchronicity_libs=-lfl -lscapi -L${sync_lib} -ldl -L${sync_bin} -Wl,-rpath,$sync_lib -Wl,-rpath,$sync_bin", "--with-version_id=$revision",
   ($debug) ? $cflags_debug : $cflags,
   ($debug) ? $cxxflags_debug : $cxxflags,);
  die "configure failed, please send config.log file in $build_temp" if $?;
  system('/usr/intel/pkgs/gmake/4.2.1/bin/gmake','-j4',"AR=$ENV{AR}"); die "build failed, please send config.log and build.log in $build_temp" if $?;
  system('/usr/intel/pkgs/gmake/4.2.1/bin/gmake', 'install'); die "install failed, please send config.log and build.log in $build_temp" if $?;
  
  if($ut) {
  # Call script to mock design sync
    mock_dc($pwd, $build_temp);

    my $temp_dir = "${build_temp}/iDMS/.libs/libiDMS_*";
    system ('/usr/intel/pkgs/gmake/4.2.1/bin/gmake', 'check', "val=${build_temp}"); die "make check fail" if $?;

    system("rm","-rf", '/tmp/dmserver_ut');
    system("rm","-rf", '/tmp/check.pc.new'); 
    system("rm","-rf", '/tmp/libcheck.la.new'); 
  }

  foreach(qw(dmserver itk idmb idmSlave)) {
    my $exec = "$opt_prefix/bin/$_";
    if (-e $exec) {
      unlink($exec) || die "can't delete: $!";
    }
    copy("${exec}_unfixed", "$exec") || die "can't copy: $!";
    print "Applying 04755 to $exec\n";
    chmod 04755, "$exec" || die "can't chmod: $!";
  }

  exit 0;
}

