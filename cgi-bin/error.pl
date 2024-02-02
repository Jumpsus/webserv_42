#!/usr/bin/perl

use strict;
use warnings;

# Get input from URL

my $input = $ENV{QUERY_STRING};

# Parse input into string and number of repetitions

my ($string, $reps) = $input =\~ /^(\\w+)+(\\d+)$/;

# Generate HTML output

print "Content-Type: text/html\\n\\n";
print "\<html\>\<body\>\\n";
print "<p>The string '$string' repeated $reps times:</p>\\n";
print "<p>" . ($string x $reps) . "</p>\\n";
print "\</body\>\</html\>\\n";`
