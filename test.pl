#!perl

use Modern::Perl;
use v5.10;

my $p =<<SQL;
SELECT *
FROM Tabelle
WHERE x=1
;
SQL

say for ( split /\W+/, $p );

