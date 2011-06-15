
AC_DEFUN(AC_GNU_MAKE,
[
AC_MSG_CHECKING("for gnu make")
AC_CACHE_VAL(ac_cv_gnu_make,
[
if eval 'make -v | grep -i gnu 1>&5 2>&5'; then
ac_cv_gnu_make=yes
else
ac_cv_gnu_make=no
fi
])
AC_MSG_RESULT("$ac_cv_gnu_make")
if test $ac_cv_gnu_make = no; then
AC_MSG_ERROR( The version of the program \"make\" that is currently in
your path does not appear to be gnu make.  Gnu's version of \"make\"
is a hard requirement to build warped.  All gnu software is available
for free via anonymous ftp from prep.ai.mit.edu in the directory
/pub/gnu.  If you have installed gnu make please make sure it is the
first \"make\" in your current path:
$PATH)
fi
])


AC_DEFUN(AC_CXX_HAS_BOOL,
[
AC_MSG_CHECKING("for builtin bool type in c++ compiler")
AC_CACHE_VAL(ac_cv_cxx_has_bool,
[
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE(,
bool x; return 0;,
ac_cv_prog_cxx_bool=yes,
ac_cv_prog_cxx_bool=no)
if test "$ac_cv_prog_cxx_bool" = yes; then
CPPFLAGS="$CPPFLAGS -DHAS_BOOLEAN"
fi
])
AC_MSG_RESULT($ac_cv_prog_cxx_bool)
])

AC_DEFUN(AC_CXX_OLD_FRIENDS,
[
AC_MSG_CHECKING("for old style friends of template classes")
AC_CACHE_VAL(ac_cv_cxx_old_friends,
[
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE(,
}
class ostream;
template <class Something> class SomeClass {
  friend ostream &operator<< <> ( ostream &, SomeClass & );
};
SomeClass<int> *x;
int foo(){ ,
ac_cv_prog_old_friends=no,
ac_cv_prog_old_friends=yes)
if test "$ac_cv_prog_old_friends" = yes; then
CPPFLAGS="$CPPFLAGS -DOLD_STYLE_FRIENDS"
fi
])
AC_MSG_RESULT($ac_cv_prog_old_friends)
])


AC_DEFUN(AC_SAVANT_MAKEDEPEND,[
  AC_MSG_CHECKING(for some way to build dependencies)
  AC_CACHE_VAL(ac_cv_found_gpp,[
    AC_CHECK_PROG(ac_cv_found_gpp, g++, yes, no )
  ])

  if test $ac_cv_found_gpp = yes; then
    MAKEDEPEND="-rm -f .depend;g++ -MM -MG \$(MDFLAGS) \$(CPPFLAGS) \$(MAKEDEP_SRCS) > .depend"
  fi

  if test $ac_cv_found_gpp = no; then
    AC_CACHE_VAL(ac_cv_savant_makedepend,[
      if test -f ../bin/makedepend.$host_os; then
        ac_cv_savant_makedepend=yes
        MAKEDEPEND="-rm -f .depend;../bin/makedepend.$host_os -f- -- \$(MDFLAGS) \$(CPPFLAGS) -- \$(MAKEDEP_SRCS) > .depend"
      else
        ac_cv_savant_makedepend=no
      fi
    AC_MSG_RESULT( checking for ../bin/makedepend.$host_os installed by savant... $ac_cv_savant_makedepend )
    ])
  fi


  if test $ac_cv_found_gpp = no; then
    if test $ac_cv_savant_makedepend = no; then
        AC_MSG_WARN( 
The regular \"makedepend\" that ships with Xwindows 
has hard limits on the number of files that it can handle and 
SAVANT has more files than it can deal with.  The developers of 
SAVANT provide two easy solutions to this problem.  One is to use 
g++ to generate dependencies (even if you're compiling with some
other compiler) and the other is to ship a binaries of X makedepend 
with its static limits increased.  However configure could not 
locate any version of the program \"g++\" in your path nor can it 
find a binary version of \"makedepend\" installed by SAVANT for your 
OS. Since configure cannot find either one you'll need to build the 
dependencies some other way - perhaps using a compiler flag of the 
c++ compiler you're using or making a soft link to a compatible
binary installed by SAVANT.  Please see the notes in the README 
regarding makedepend.  If g++ _is_ installed on your system please make 
sure the directory it is in appears in your path - your path right now
is:
$PATH )
      exit -1;
    fi
  fi
])
