#############################################################################
#
# The code in this file should run in the HPC-GAP version
#
#############################################################################

ReadGapRoot("demo/threads.g");

# Example 1. Simple operations channels with no threads
ch1:=CreateChannel("t1");   
ch2:=CreateChannel("t2");
SendChannel(ch1,6);
SendChannel(ch1,7);
SendChannel( ch2, ReceiveChannel(ch1)*ReceiveChannel(ch1));
ReceiveChannel(ch2);
DestroyChannel("t1");
DestroyChannel("t2");

# Example 2. Sending to channel list elements in a loop inside a thread
ch:=CreateChannel("test");
n:=10000;
f:=function() local i; for i in [1..n] do SendChannel(ch,i); od; end;
CreateThread(f);
l:=List([1..n],i->ReceiveChannel(ch));;
DestroyChannel(ch);
l=[1..n];

# Example 3. Two threads, each with own input and output channels
chin1:=CreateChannel("chin1");
chin2:=CreateChannel("chin2");
chout1:=CreateChannel("chout1");
chout2:=CreateChannel("chout2");
mult1:=function() SendChannel( chout1, ReceiveChannel(chin1)*ReceiveChannel(chin1)); end;
mult2:=function() SendChannel( chout2, ReceiveChannel(chin2)*ReceiveChannel(chin2)); end;
CreateThread(mult1);
CreateThread(mult2);
SendChannel(chin1,2);
SendChannel(chin1,3);
SendChannel(chin2,4);
SendChannel(chin2,5);
ReceiveChannel(chout1);
ReceiveChannel(chout2);
for ch in [ chin1, chin2, chout1, chout2 ] do
   DestroyChannel(ch); 
od;

# Example 4. Sending objects to a thread and receiving them back
ChannelTest:=function( obj )
local chin, chout, r, thread;
chin:=CreateChannel();
chout:=CreateChannel();
thread := CreateThread( function() SendChannel(chout, ReceiveChannel(chin) ); end ); 
SendChannel( chin, obj );
r:=ReceiveChannel( chout );
WaitThread( thread );
DestroyChannel(chin);
DestroyChannel(chout);
return r=obj;
end;

ChannelTest(1);
ChannelTest(E(4)); 
ChannelTest(Integers);
ChannelTest(DihedralGroup(1000));
ChannelTest(GAPInfo);            
ChannelTest(2^160000);
ChannelTest(Factorial);
ChannelTest(GlobalRandomSource);
ChannelTest("bla");  

for i in [1..10000] do r:=ChannelTest(i); od;time;

# Example 5. A function to multiply objects in a thread
MultiplyInThread := function(a,b)
local chin, chout, thread, r;
chin:=CreateChannel();
chout:=CreateChannel();
SendChannel( chin, a );
SendChannel( chin, b );
thread := CreateThread( function() SendChannel(chout, ReceiveChannel(chin)*ReceiveChannel(chin) ); end ); 
r:=ReceiveChannel( chout );
WaitThread( thread );
DestroyChannel(chin);
DestroyChannel(chout);
return r;
end;
MultiplyInThread(6,7);

# Example 6. Usage of CallFuncListThread and FinaliseThread
chin:=CreateChannel();
chout:=CreateChannel();
r:= CallFuncListThread( s -> Size(SmallGroup(s)), [[8,3]], chin, chout );
FinaliseThread( r, chin, chout );

# Example 7. Recursive computation of Fibonacci(n) with two threads on top.
fib_recursive := function(n)
if n in [1, 2] then
  return 1;
else
  return fib_recursive(n-1) + fib_recursive(n-2);
fi;
end;

fib_threads_recursive:= function(n)
local chin1, chin2, chout1, chout2, thread1, thread2, r1, r2;
if n in [1, 2] then
  return 1;
else
  chin1:=CreateChannel();
  chin2:=CreateChannel();
  chout1:=CreateChannel();
  chout2:=CreateChannel();  
  thread1:= CallFuncListThread( fib_recursive, [ n-1 ], chin1, chout1 );
  thread2:= CallFuncListThread( fib_recursive, [ n-2 ], chin2, chout2 );
  r1 := FinaliseThread( thread1, chin1, chout1 );
  r2 := FinaliseThread( thread2, chin2, chout2 );
 return r1+r2; 
fi;
end; 
fib_threads_recursive(6);

# Example 8. Recursive computation of Fibonacci(n) creating two subthreads 
# on each step (good to exhaust resources and get segfaults)
fib_threads:= function(n)
local chin1, chin2, chout1, chout2, thread1, thread2, r1, r2;
if n in [1, 2] then
  return 1;
else
  chin1:=CreateChannel();
  chin2:=CreateChannel();
  chout1:=CreateChannel();
  chout2:=CreateChannel();  
  thread1:= CallFuncListThread( fib_threads, [ n-1 ], chin1, chout1 );
  thread2:= CallFuncListThread( fib_threads, [ n-2 ], chin2, chout2 );
  r1 := FinaliseThread( thread1, chin1, chout1 );
  r2 := FinaliseThread( thread2, chin2, chout2 );
  return r1+r2; 
fi;
end; 
fib_threads(6);

# Example 9. Compare standard, Fibonacci and threaded Fibonacci multiplication
ReadGapRoot("demo/karatsuba.g");
x:=Indeterminate(Rationals,"x");
nr:=IndeterminateNumberOfLaurentPolynomial(x);
fam:=FamilyObj(1);;
l:=[-4..5];
deg:=5000;
KARATSUBA_CUTOFF:=150; # quasi-optimal value for one main and two subthreads
f:=LaurentPolynomialByCoefficients( fam, List([1..deg],i->l[(i mod 10) + 1]), 0, 1 );;
g:=LaurentPolynomialByCoefficients( fam, List([1..deg],i->l[((i+5) mod 10) + 1]), 0, 1 );;
Exec("date"); t1:=f*g;; Exec("date");
Exec("date"); t2:=KaratsubaPolynomialMultiplication(f,g);; Exec("date");
Exec("date"); t3:=KaratsubaPolynomialMultiplicationThreaded(f,g);; Exec("date");
t1=t2; t1=t3; 



