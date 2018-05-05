#!/usr/bin/env perl
use v5.10.0;
# make a tar archive of the HEAD of the git repo.
# If flag -b given then increment the version number and
# tag the repo with the new number before writing the archive.
#
# Write the archive:
# ./mkversion.pl
# Bump version and write archive:
# ./mkversion.pl -b
#

my $curdir = `basename \`git rev-parse --show-toplevel\``;
chomp($curdir);
my $distname = $curdir;

use Getopt::Long;
my $bump_version_flag = 0;
my $mstring = '';
my $dry_run_flag = 0;

GetOptions ("bump|b!" => \$bump_version_flag,
            "m=s" => \$mstring,
            "dry-run|d" => \$dry_run_flag,
    )
    or die("Error in command line arguments\n");

sub dosys { 
    my $c = shift; 
    say STDERR $c;
    system $c unless $dry_run_flag;
}

sub get_git_tag_versions {
    my @tags = `git tag --sort=v:refname`;
    chomp foreach(@tags);
    my @versions = grep(/^v?[\d\.]+$/,@tags);
}

sub get_last_version_from_list {
    my ($versions) = @_;
    return $$versions[@$versions-1];
}

sub increment_last_number {
    my ($vn) = @_;
    my @nums = split(/\./,$vn);
    my $in = @nums -1;
    my $minor = $nums[$in];
    my $width = length($minor);
    my $new_minor = $minor + 1;
    my $new_minor_string = sprintf("%0${width}d",$new_minor);
    $nums[$in] = $new_minor_string;
    my $inc_number = join('.',@nums);
    return $inc_number;
}

sub get_last_version_number {
    my @v = get_git_tag_versions();
    my $lastn =  get_last_version_from_list(\@v);
    return $lastn;
}

sub get_next_version_number {
    my $lastn =  get_last_version_number();
    return increment_last_number($lastn);
}


sub bump_git_tag_version {
    my ($new_version) = @_;
#    my $mstring0 = $new_version;
#    $mstring0 = $mstring if $mstring;
    if ($mstring eq '') {
        dosys("git tag -a $new_version");        
    }
    else {
        dosys("git tag -a $new_version -m \"$mstring\"");
    }
}

sub write_tar_dist {
    my ($version) = @_;
    my $distnamenum = "${distname}-${version}";
    dosys("git archive --prefix=${distnamenum}/ -o ../${distnamenum}.tar.gz HEAD");
}

sub bump_version_write_archive {
    my $new_version = get_next_version_number();
    bump_git_tag_version($new_version);
    write_tar_dist($new_version);
}

sub write_archive_no_bump_version {
    my $version = get_last_version_number();
    write_tar_dist($version);
}

if ( $bump_version_flag == 1 ) {
    bump_version_write_archive();
}
else {
    write_archive_no_bump_version();
}
