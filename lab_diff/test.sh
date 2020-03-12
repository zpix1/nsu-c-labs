#! /bin/bash
set -v;

VG=''

# Simple test
perl -e 'print "nanananaa\n"x599; print "DETONATOR\n"; print "nanananaa\n"x400; print "BATMAN"' > afile
perl -e 'print "nanananaa\n"x1000; print "BATMAN"' > bfile
$VG ./diff afile bfile

# Different linesize test
perl -e 'print "abcd\n\n"' > afile
perl -e 'print "abcde\n"' > bfile
$VG ./diff afile bfile

perl -e 'print "abcde\n"' > afile
perl -e 'print "abcd\n\n"' > bfile
$VG ./diff afile bfile

# Size mismatch
perl -e "print '123'x100" > afile
perl -e "print '123'x101" > bfile
$VG ./diff afile bfile

# Text test (check it out after tests)
perl -e "print 'ABCD'x1000; print \"\n\"; print '1234'; print 'DCBA'x1000; print '1234'; " > afile
perl -e "print 'ABCD'x1000; print \"\n\"; print '4321'; print 'DCBA'x1000; print '4321';" > bfile
$VG ./diff afile bfile > big_str_file.test

# Same texts test
perl -e "print (\"abcd\"x100 . \"\n\" . \"dcba\"x100 . \"\n\")x1000" > afile
perl -e "print (\"abcd\"x100 . \"\n\" . \"dcba\"x100 . \"\n\")x1000" > bfile
$VG ./diff afile bfile

# Big binary test (100MB)
perl -e "print chr(0xED)x100000000; print 'eee'; print 'A'x100; print 'B'" > afile
perl -e "print chr(0xED)x100000000; print 'ddd'; print 'A'x100; print 'B'" > bfile
./diff afile bfile


rm afile bfile