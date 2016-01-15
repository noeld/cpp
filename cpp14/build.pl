#!perl

while(<>) {
	if (/^\/\/\#(.*)$/) {
		print `$1`,"\n";
		exit;
	}
}
print "No Command found";
