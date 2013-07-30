#!/usr/bin/perl
########################################################################################################################
# Copyright 2009 Alexander Poluektov
# All rights reserved
########################################################################################################################

# $Id: //depot/main/Test/Test_cases/trivial.pl#3 $
# $DateTime: 2009/07/30 12:22:26 $

# Test case for main_app.

########################################################################################################################

# Grabs output and compare what main (and only main!) has sent.
# Timestamps are ignored during comparison.

$exe_name = shift @ARGV;

$etalon =
    "(sender:001) Major:   ... Application shut down normally\n";

$actual = `$exe_name 2>&1 | perl Test/Test_cases/process_log.pl`;

exit !($etalon eq $actual);
