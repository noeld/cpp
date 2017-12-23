#!perl

use v5.10;
use Data::Dumper;
use POSIX qw(ceil floor);
use constant {MAXX => 1000000, MAXY => 100};
# Prototype: add a 2 dimensional matrix. Each pixel is a list of indexes
# effectively making it a >= 3 dimensional array (cube)
# Targets are
# * height and width of the structure are fix (x, y)
# * 0 < x < 1000000
# * 0 < y < 100
# * the number of entries per (x, y) can be between 0 and z
# * 0 < z < 100000
# A reansonable programm will need 1000 of these strucutres
# * memory efficiency
# * the 
my $rect_orig = [0, 0, MAXX, MAXY];
my $rect_dst = [0, 0, 15, 19];
my @a;
$#a = rect_width($rect_dst) * rect_height($rect_dst);

sub min {
	$_[0] < $_[1] ? $_[0] : $_[1];
}
sub max {
	$_[0] > $_[1] ? $_[0] : $_[1];
}
sub random { 
	int(rand($_[0]));
}
sub random_rect {
	my ($maxx, $maxy) = @_;
	my @x = (random($maxx), random($maxx));
	my @y = (random($maxy), random($maxy));
	return [min(@x), min(@y), max(@x), max(@y)];
}
sub rect_to_string {
	my ($r) = @_;
	return "(($$r[0], $$r[1]), ($$r[2], $$r[3]))";
}
sub rect_width {
	return $_[0][2] - $_[0][0] + 1;
}
sub rect_height {
	return $_[0][3] - $_[0][1] + 1;
}
sub scale_rect {
	my ($rect_orig, $rect_dst, $r1) = @_;
	my ($x1, $y1) = scale_coords($rect_orig, $rect_dst, $$r1[0], $$r1[1]);
	my ($x2, $y2) = scale_coords($rect_orig, $rect_dst, $$r1[2], $$r1[3]);
	return [
		  max(floor($x1), $$rect_dst[0])
		, max(floor($y1), $$rect_dst[1])
		, min(ceil ($x2), $$rect_dst[2])
		, min(ceil ($y2), $$rect_dst[3])
	];
}
sub scale_coords {
	my ($rect_orig, $rect_dst, $x, $y) = @_;
	my $w1 = rect_width($rect_orig);
	my $h1 = rect_height($rect_orig);
	my $w2 = rect_width($rect_dst);
	my $h2 = rect_height($rect_dst);
	return (
		  ($x + $$rect_orig[0] - $$rect_dst[0]) * $w2 / $w1
		, ($y + $$rect_orig[1] - $$rect_dst[1]) * $h2 / $h1
		);
}
sub array_index_for_coords {
	my ($x, $y) = @_;
	return $y * rect_width($rect_dst) + $x ;
}
my @rects;
my %combinations; # by joined string => index
my %inverse_combinations; # index => joined string

sub combinations_for_index {
	my ($index, $emptyifnone) = @_;
	$emptyifnone = 0 unless defined($emptyifnone);
	return ($index) if ($index < @rects);
	unless(exists($inverse_combinations{$index})) {
		if ($emptyifnone) {
			return ();
		} else {
			die "Cannot find index $index";
		}
	}
	my $comb = $inverse_combinations{$index};
	return split(',', $comb);
}
sub index_for_indices {
	my ($indexold, $indexadd) = @_;
	unless(defined($indexold)) {
		return $indexadd;
	}
	my @comb;
	if (exists($inverse_combinations{$indexold})) {
		push @comb, $inverse_combinations{$indexold};
	} else {
		if ($indexold >= @rects) {
			die "Something is wrong with $indexold";
		}
	}
	push @comp, $indexadd;
	my $newkey = join(',', @comp);
	my $newindex = scalar(keys(%combinations)) + $#rects + 1;
	$combinations{$newkey} = $newindex;
	$inverse_combinations{$newindex} = $newkey;
	return $newindex;
}
sub index_for_combination {
	my ($c) = @_;
	my $r = \@_;
	if (ref($c) eq 'ARRAY') {
		$r = $c;
	}
	my $key = join(',', @$r);
	unless(defined($combinations{$key})) {
		return undef;
	}
	return $combinations{$key};
}

# Generate random_n random rects within $rect_orig
# Map rects to $rect_dst
my $random_n = 4;
for my $n (1..$random_n) {
	my $r = random_rect(MAXX, MAXY);
	push @rects, $r;
	my $r2 = scale_rect($rect_orig, $rect_dst, $r);
	print rect_to_string($r), ' - ', rect_to_string($r2), "\n";
	for (my $y = $$r2[1]; $y <= $$r2[3]; ++$y) {
		for (my $x = $$r2[0]; $x <= $$r2[2]; ++$x) {
			my $index = array_index_for_coords($x, $y);
			$a[$index] .= $n;
		}
	}
}

# Print scaled rect
for (my $y = 0; $y <= $$rect_dst[3]; ++$y ) {
	for (my $x = 0; $x <= $$rect_dst[2]; ++$x) {
		my $index = array_index_for_coords($x, $y);
		printf(".%-${random_n}s", $a[ $index ]);
	}
	printf("\n");
}

# Lookup some random points and print the orignial rects which contain then
print 'Max Index:', $#a, "\n";
for (1..10) {
	my $pt_orig = [random(MAXX), random(MAXY)];
	print "Orig ($$pt_orig[0], $$pt_orig[1]) ";
	my @pt_dst = scale_coords($rect_orig, $rect_dst, @$pt_orig);
	print "Dst ($pt_dst[0], $pt_dst[1]) ";
	my $dst_idx = array_index_for_coords(map {int} @pt_dst);
	print " Index $dst_idx";
	my $idx = $a[$dst_idx];
	print " Comb: ", join(', ', split(//, $idx));
	print "\n";
}
