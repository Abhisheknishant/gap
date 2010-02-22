#############################################################################
#
# Prototypes of functions for shared memory programming in GAP
#
#############################################################################

#############################################################################
#
# CallFuncListThread( func, args, chin, chout )
#
# This is an analog of CallFuncList(func,args) which calls the function
# func with arguments args in a separate newly created thread with an 
# input channel chin and an output channel chout.
#
# This function allows to use the same function both in CallFuncList and
# in CallFuncListThread
#
CallFuncListThread:=function( func, args, chin, chout )
SendChannel( chin, args );
return CreateThread( 
    function( ) 
    SendChannel( chout, CallFuncList( func, ReceiveChannel( chin ) ) );
    end );
end;


#############################################################################
#
# FinaliseThread( thread, chin, chout )
#
# Performs four actions in one call for a thread with an input channel chin 
# and an output channel chout: waits (blocking) until the thread will return
# the result to its output channel and free its resources, then destroys both
# input and output channels for that thread.
#
FinaliseThread:=function( thread, chin, chout )
local res;
res := ReceiveChannel( chout );
WaitThread( thread );
DestroyChannel(chin);
DestroyChannel(chout);
return res;
end;