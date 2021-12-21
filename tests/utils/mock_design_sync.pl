#!/usr/intel/pkgs/perl/5.14.1-threads/bin/perl

use File::Copy;
use File::Path qw(make_path remove_tree);
use Cwd;

sub mock_dc{
  my ($ori_pwd, $build_temp) = @_;
  my $dms_path =  $ENV{'DMSPATH'};
  my $new_dms_path = "/tmp/dmserver_ut/dmspath.txt"; 
  #print "ori: $dms_path \n";
  # copy to modify
  if(!(-e "/tmp/dmserver_ut" && -d "/tmp/dmserver_ut")){
    make_path("/tmp/dmserver_ut");
  }
  #copy($dms_path,$new_dms_path) or die "Copy failed: $!";
  # backup
  #copy($dms_path, "/tmp/dmserver_ut");
  #change env var DMSPATH temporary
  #$ENV{'DMSPATH'} = $new_dms_path;
  #print "New DMSPATH env var: $ENV{'DMSPATH'}\n";

  #modify new DMSPATH
  open(FILE, "<$dms_path") || die "File not found";
  my @lines = <FILE>;
  close(FILE);

  my @newlines;
  foreach(@lines) {
    if(/rwpaths\s+\%genesys\.lnf / && !(/RW\(utlib\)/)) {
      push(@newlines,"rwpaths %genesys.lnf                 RW(utlib):\\\n");
      push(@newlines,"                                     \$WORK_AREA_ROOT_DIR/genesys/lnf:\\\n");
    } elsif (/libpath\s+\%genesys\.\*/ && !(/\/tmp\/dmserver_ut/)) {
      push(@newlines,"libpath %genesys.*                   /tmp/dmserver_ut/designsync/client/utlib: \\\n");
      push(@newlines,"                                     \$ADE2LYNX_ROOT/tools/dfII/src/artist/lynxLib/analogLib:\\\n"); 
    } else {
      push(@newlines,$_);
    }
  }

  open(FILE, ">$dms_path") || die "File not found";
  print FILE @newlines;
  close(FILE);

  # setup local design sign
  my $local_design_sync = "/tmp/dmserver_ut/designsync";
  my $local_server = $local_design_sync."/server";
  my $local_client = $local_design_sync."/client";
  my $local_cache  = $local_design_sync."/cache";
  my $local_lib    = $local_client."/utlib";
  my $real_lib     = $ENV{'WORK'}."/opuslib/".getpwuid($<)."_lib";

  if (-e $local_design_sync && -d $local_design_sync){
    print "local design sync alr exist\n";
    if (!(-e $local_server)){
      make_path($local_server);
    }
    if(!(-e $local_client)) {
      make_path($local_client);
    }
    if(!(-e $local_cache)) {
      make_path($local_cache);
    }
    if(!(-e $local_lib)) {
      make_path($local_lib);
    }
    
  } else {
    print ("Creating new local design sync dir $local_design_sync\n");
    make_path($local_server);
    make_path($local_client);
    make_path($local_cache);
    make_path($local_lib);
  }

system("chmod","-R","777","$local_design_sync");
 # set env var for design sync mock up
  $ENV{'SYNC_CDS_SETUP'} = 1;
  $ENV{'SYNC_MODIFY_LM'} = 1;
  $ENV{'SYNC_CACHE'}     = $local_cache;
  $ENV{'CK_DEFAULT_TIMEOUT'} = 0; #disable timeout check

  system("rm","-rf","$real_lib");
  print("linking $local_lib to $real_lib\n");
  system("ln","-s","$local_lib","$real_lib");

  my $lib_file = $ori_pwd."/tests/utils/.type.lib";
#print("pwd=$ori_pwd-- lib=$libFile is....\n");
 system("dssc","setvault","-rec","$local_server","$local_client");  

 if (!(-e $local_lib.".type.lib")) {
   system("cp","$lib_file", "$local_lib");
   chdir $local_lib;
   system("dssc", "ci", "-noc","-new", ".type.lib");
   # change back to ori dir not to mess up with make check
   chdir $build_temp;   
 }  
  # Update DMSPATH to original one - at relink
  #$ENV{'DMSPATH'} = $dms_path;
  #print("Setting back DMSPATH env: $ENV{'DMSPATH'}\n");
}

1;
