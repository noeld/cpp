#!perl

use Math::Complex;

@chars = (' ', '.', ':', ';', '+', '=', 'o', 'O', '8', 'M', '#', '_');

($xmin, $ymin, $xmax, $ymax) = (-0.7, 0.6, -0.5, 0.7);
$maxit = 1000;
if (@ARGV>=1) {
	$maxit = $ARGV[0];
}

$cols = 260;
$rows = 80;

sub mandel {
	my $cc = cplx($_[0], $_[1]);
	my $c = cplx($_[0], $_[1]);
	for(my $i = 0; $i < $maxit; ++$i) {
		$cc = $cc ** 2.0 + $c;
		if (abs($cc) > 4.0) {
			return $i;
		}
	}
	return $maxit - 1;
}

for(my $y = $ymax; $y > $ymin; $y -= ($ymax-$ymin)/$rows) {
	for(my $x = $xmin; $x < $xmax; $x += ($xmax-$xmin)/$cols) {
		my $i = mandel($x, $y);
		print $chars[int($i/$maxit * @chars)];
	}
	print "\n";
}
