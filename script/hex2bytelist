#!/usr/bin/perl

open IN, '<', $ARGV[0];
open OUT, '>', $ARGV[1];
binmode OUT;

while ($line = <IN>) {
    if ($line !~ /^\/\//) {
	$line =~ s/0x//;
	$dec = hex($line);
	$byte1 = sprintf("0x%02x,\n", $dec & 0x000000ff);
	$byte2 = sprintf("0x%02x,\n", ($dec & 0x0000ff00) >> 8);
	$byte3 = sprintf("0x%02x,\n", ($dec & 0x00ff0000) >> 16);
	$byte4 = sprintf("0x%02x,\n", ($dec & 0xff000000) >> 24);
	print OUT $byte1 . $byte2 . $byte3 . $byte4;
    }
}

close OUT;
close IN;
