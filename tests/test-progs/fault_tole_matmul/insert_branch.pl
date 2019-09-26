#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Long;
#use Data::Dumper;
use POSIX;

=head1 # Description:

  This script is used to random generate branch/NOP instructions in specified assembling code

=head1 # Usage:

  Run  : $0 <*.s file> <paras in any order>
  Help : $0 --help

=head1 # Parameter:

=head2 ## Branch:

  -b    <bool>  (default = false) Enable randomly inserting branches
  -bn   <int>   (default = 5)     The # of randomly generated branches 
  -bp   <float> (default = 0.1)   Probability of inserting branches 

=head2 ## NOP:

  -nop  <bool>  (default = false) Enable randomly inserting NOPs
  -nopn <int>   (default = 5)     The # of randomly generated NOPs 
  -nopp <float> (default = 0.1)   Probability of inserting NOPs 
  -nopc <int>   (default = 5)     The maximum continuous cycle of single NOP

=cut

# Parsing arguments
my %args = (
    "help"          => undef,

    # Branch
    "branch_enable" => undef,
    "branch_num"    => 5,
    "branch_prob"   => 0.1,

    # NOP
    "nop_enable"    => undef,
    "nop_num"       => 5,
    "nop_prob"      => 0.1,
    "nop_max_cycle" => 5
);
GetOptions(
    "help"   => \$args{"help"},

    # Branch
    "b"      => \$args{"branch_enable"},
    "bn=i"   => \$args{"branch_num"},
    "bp=f"   => \$args{"branch_prob"},

    # NOP
    "nop"    => \$args{"nop_enable"},
    "nopn=i" => \$args{"nop_num"},
    "nopp=f" => \$args{"nop_prob"},
    "nopc=i" => \$args{"nop_max_cycle"}
);
#print Dumper(\%args);

# Help doc
print("\n");
die `pod2text $0` if (!defined($ARGV[0]) or $args{"help"}); 

# Get branch option
my $branch_num  = $args{"branch_enable"} ? $args{"branch_num"}  : 0;  
my $branch_prob = $args{"branch_enable"} ? $args{"branch_prob"} : 0;  

# Get nop option
my $nop_num       = $args{"nop_enable"} ? $args{"nop_num"}       : 0;                
my $nop_prob      = $args{"nop_enable"} ? $args{"nop_prob"}      : 0;                
my $nop_max_cycle = $args{"nop_enable"} ? $args{"nop_max_cycle"} : 0;                


#------------------------
# Main Code
#------------------------

# Input assembling code file
die "Please specify the target assembling file (*.s) which needs to be modified! Exit...\n\n" if (!($ARGV[0] =~ ".+\.s"));
open(CODE_FILE, "<$ARGV[0]") or die "Cannot open $ARGV[0]! Exit...\n\n";
my @code_lines = <CODE_FILE>;

# Get file name
my $file_name = $ARGV[0];
$file_name =~ s/(.+)\.s/$1/; 

# Modify the original assembling code and print it into the output file
open(MOD_FILE, ">${file_name}_mod.s");

# Insertion
my $is_main_func = undef; # Used as a flag to show the loop is printing main function

my ($branch_inserted_num,
    $nop_inserted_num) = (0, 0);

my ($branch_insertion_finish, 
    $nop_insertion_finish) = (undef, undef);

srand();
foreach my $line (@code_lines) {

    # Find the end of code (need to mannually insert DumbFunc() at first)
    if ($line =~ "^DumbFunc:") {
        foreach (1..${branch_num}) {
	    print MOD_FILE ("JMP_${_}:\n");
	    print MOD_FILE ("\tb\tJMP_${_}_BACK\n");
	}
    }

    # Judge if insertion is done
    if ($branch_inserted_num == $branch_num) {
        $branch_insertion_finish = 1;
    }

    if ($nop_inserted_num == $nop_num) {
        $nop_insertion_finish = 1;
    }

    # Inserting the modified code line
    if ($is_main_func and not $branch_insertion_finish) {
        if (rand(1) < $branch_prob) {
	    $branch_inserted_num += 1;
	    print MOD_FILE ("\tb\tJMP_${branch_inserted_num}\n");
	    print MOD_FILE ("JMP_${branch_inserted_num}_BACK:\n");
        }

	if ($line =~ "bl.*DumbFunc") {
	    my $tmp_num = $branch_inserted_num + 1;
	    foreach (${tmp_num}..${branch_num}) {
	        print MOD_FILE ("\tb\tJMP_${_}\n");
	        print MOD_FILE ("JMP_${_}_BACK:\n");
	    }

	    # All remaining branches are inserted
	    $branch_inserted_num = $branch_num;
	}
    }

    if ($is_main_func and not $nop_insertion_finish) {
        if (rand(1) < $nop_prob) {
	    $nop_inserted_num += 1;
	    foreach (1..${nop_max_cycle}) {
	        print MOD_FILE ("\tnop\n");
	    }
        }

	if ($line =~ "bl.*DumbFunc") {
	    my $tmp_num = $nop_inserted_num + 1;
	    foreach (${tmp_num}..${nop_num}) {
	        foreach (1..${nop_max_cycle}) {
	            print MOD_FILE ("\tnop\n");
	        }
	    }

	    # All remaining NOPs are inserted
	    $nop_inserted_num = $nop_num;
	}
    }

    # The start point of insertion
    if ($line =~ "^main:") {
        $is_main_func = 1;
    }

    # Copy the original code line into output file
    print MOD_FILE ($line);
}

print("Success! Please find the modified assembling code in ${file_name}_mod.s\n\n");

close(MOD_FILE);
close(CODE_FILE);

