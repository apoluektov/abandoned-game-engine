#!/usr/bin/perl
########################################################################################################################
# Copyright 2009 Alexander Poluektov
# All rights reserved
########################################################################################################################

# $Id: //depot/main/Test/Test_cases/process_log.pl#2 $
# $DateTime: 2009/07/30 12:22:26 $

# Process logfile: skip timestamps and messages that does not belong to main.

########################################################################################################################

while (<STDIN>)
{
    if (m/\(sender:001\)/)
    {
	s/\([\d]{8}\) (.*)/\1/;
	print;
    }
}
