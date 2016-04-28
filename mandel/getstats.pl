#!perl

@commands = ('python mandel.py', 'mandel');

for my $i (1..5) {
	for my $cmd (@commands) {
		my $ii = 10**$i;
		my $c = qq(WinTime "$cmd $ii");
		print $c,"\n";
		print `$c`,"\n";
	}
}