#!/usr/bin/perl

open IN, '<', $ARGV[0];
open OUT, '>', $ARGV[1];
binmode OUT;

while ($line = <IN>) {
    if ($line !~ /^\/\//) {
	$line =~ s/0x//;
	$dec = hex($line);
	$bin = pack "I", $dec;
	print OUT $bin;
    }
}

close OUT;
close IN;

