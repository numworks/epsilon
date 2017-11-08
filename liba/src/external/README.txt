"softfloat": Unaltered files from SoftFloat v3c.
http://www.jhauser.us/arithmetic/SoftFloat.html

"openbsd": Unaltered files from OpenBSD 4.9. Original path is "lib/libm/src"
"openbsd/include": Compatibility headers needed to build files from OpenBSD.
http://www.openbsd.org

Why use OpenBSD's libm? First and foremost, nearly everyone use fdlibm in a way
or another, and OpenBSD is no exception. That being said, OpenBSD comes with a
single-precision variant (which fdlibm itself lacks). And more interestingly,
this variant does all its computation in single-precision (it never upgrades to
double-precision). In our case, this is very interesting because we're doing
single-precision computation when we better performance at the cost of accuracy,
and in our case our hardware has a single-precision FPU so switching to double
can yield much lower performances.

Last but not least, we're using OpenBSD 4.9 and not the latest one because this
version still has all the double precision code (some of it got removed later on
in favor of a "long double" version).
