#!/usr/bin/perl

open IN, '<', $ARGV[0];
binmode IN;

for ($i = 0; $i < 1024; $i++) {
    my $buf;
    read IN, $buf, 4;
    $dec = unpack "I", $buf;
    $hex = sprintf("0x%x\n", dec);
    print $hex;
}

close OUT;
close IN;

