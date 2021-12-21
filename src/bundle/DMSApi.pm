package DMSApi;

use strict;

use IPC::Open2;

require Exporter;
our @ISA = qw(Exporter);
our @EXPORT = qw(&DMSSendCmd &DMSEndSession &DMSInit);

my $dmsrd;
my $dmswr;
my $dmspid;

my $debug = $ENV{'DMSAPI_DEBUG'};

sub DMSInit {
  my ($toolname) = @_;

  if (!defined($toolname) || $toolname eq "") {
    die "DMSApi: -F- No tool name specified";
  }
  $dmspid = open2($dmsrd, $dmswr, "$ENV{DMSERVER_BIN_DIR}/dmsapi");
  if (! defined $dmsrd) {
     die "DMSApi: -F- could not succesfully launch $ENV{DMSERVER_BIN_DIR}/dmsapi";
     return;
  }
  my $gotOK = 0;
  my @errs;
  my $entered = 0;
  while (<$dmsrd>) {
     $entered = 1;
     my $line = $_;
     chomp($line);
     push (@errs, "$line"); 
     if ($debug) {
        print "line: $line\n";
     }
     if ($line =~ /^DMSAPI_READY/) {
        $gotOK = 1;
        last; 
     }
  }

  die "DMSApi: -F- $ENV{DMSERVER_BIN_DIR}/dmsapi has closed output stream to $toolname prematurely" unless ($entered);

  if (! $gotOK) {
     print STDERR "DMSApi: -E- DMSAPI not ready. See output from dmsapi:\n\n";
     foreach (@errs) {
        print STDERR "$_\n";
     } 
     die "DMSApi: -F- could not successfully launch $ENV{DMSERVER_BIN_DIR}/dmsapi";
     return;
  } 

  my $result = &DMSSendCmd("setToolName", $toolname);
  if (&DMSSendCmd("getToolName") ne $toolname) {
    die "DMSApi: -F- Failed to initialize DMS for tool $toolname";
  }
  if ($debug) {
     print("succesful DMSApi init\n");
  } 
}

sub DMSSendCmd {
  if (! $dmswr) {
     die("cannot open oput (dmswr is empty\n");
  }
  print $dmswr join(' ', @_), "\n";
  while (<$dmsrd>){
    chomp($_);
    if (m/^OK (.*)/) {
      if ($debug) {
         print("result: $_\n");
      } 
      return $1;
    } else {
      print $_;
    }
  }
}


sub DMSEndSession {
  &DMSSendCmd("quit");
  waitpid $dmspid, 0;
}

1;
