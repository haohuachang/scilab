// Copyright (C) 2008 - INRIA - Michael Baudin
// Copyright (C) 2010 - DIGITEO - Michael Baudin
//
// Copyright (C) 2012 - 2016 - Scilab Enterprises
//
// This file is hereby licensed under the terms of the GNU GPL v2.0,
// pursuant to article 5.3.4 of the CeCILL v.2.1.
// This file was originally licensed under the terms of the CeCILL v2.1,
// and continues to be available under such terms.
// For more information, see the COPYING file which you should have received
// along with this program.

// <-- CLI SHELL MODE -->
// <-- ENGLISH IMPOSED -->

function flag = MY_assert_equal ( computed , expected )
  if computed==expected then
    flag = 1;
  else
    flag = 0;
  end
  if flag <> 1 then pause,end
endfunction

function checkassert ( flag , errmsg , ctype )
  if ( ctype == "success" ) then
    MY_assert_equal ( (flag==%t) & (errmsg==""), %t )
  else
    MY_assert_equal ( (flag==%f) & (errmsg<>""), %t )
  end
endfunction

format("v",10);

// Check error message when number of arguments is false
instr = "assert_checktrue ( )";
ierr=execstr(instr,"errcatch");
MY_assert_equal ( ierr , 10000 );
//
instr = "[o1,o2,o3]=assert_checktrue ( %t )";
ierr=execstr(instr,"errcatch");
MY_assert_equal ( ierr , 999 );
//////////////////////////////////////////
// Check error message when type of arguments is false
instr = "assert_checktrue ( ""a"" )";
ierr=execstr(instr,"errcatch");
MY_assert_equal ( ierr , 10000 );
//
//
// Check that the error message is correctly handled.
instr = "assert_checktrue ( [%f %t] )";
ierr=execstr(instr,"errcatch");
MY_assert_equal ( ierr , 10000 );
errmsg = lasterror();
refmsg = msprintf( gettext( "%s: Assertion failed: found false entry in condition = %s" ) , "assert_checktrue", "[F ...]");
MY_assert_equal ( errmsg , refmsg );
//
[flag,errmsg] = assert_checktrue ( %t );
checkassert ( flag , errmsg , "success" );
//
[flag,errmsg] = assert_checktrue ( [%t %t] );
checkassert ( flag , errmsg , "success" );
//
[flag,errmsg] = assert_checktrue ( %f );
checkassert ( flag , errmsg , "failure" );
//
[flag,errmsg] = assert_checktrue ( [%t %f] );
checkassert ( flag , errmsg , "failure" );

