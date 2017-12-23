use utf8;
use v5.10;
use strict;
use warnings;
use constant INITSTR => '*'x80;
use List::Util;
use File::Find;
use File::Path;
my $pwd = File::Path::getcwd();

my $maxlength = List::Util::max(map { length } keys %ENV);
say INITSTR;
say sprintf("%-${maxlength}s: ", $_), $ENV{$_} for ( sort { uc($a) cmp uc($b) } keys %ENV );
say INITSTR;
find(sub { say $File::Find::name }, $pwd);

my $b = Bla->new();
say ref($b);

package Bla;

sub new {
    my ($proto) = @_;
    my $self = bless {}, $proto;
    return $self;
}

sub DESTROY {
    say __PACKAGE__, ;
}
sub get_width {
    return $_[0]->{width};
}
sub set_width {
    my ($self, $new_value) = @_;
    $$self{width} = $new_value;
    return $self;
}
sub get_size {
    return $_[0]->{size};
}
sub set_size {
    my ($self, $new_value) = @_;
    $$self{size} = $new_value;
    return $self;
}
sub get_name {
    return $_[0]->{name};
}
sub set_name {
    my ($self, $new_value) = @_;
    $$self{name} = $new_value;
    return $self;
}
sub get_height {
    return $_[0]->{height};
}
sub set_height {
    my ($self, $new_value) = @_;
    $$self{height} = $new_value;
    return $self;
}

1;


