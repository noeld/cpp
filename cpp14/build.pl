#!perl

while(<>) {
	if (/^\/\/\#(.*)$/) {
		local $|;
		print $1,"\n";
		print `$1`,"\n";
		exit;
	}
}
die "No Command found";
