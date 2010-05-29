#############################################################################
##
#W  thread.g                    GAP library                  Reimer Behrends
##
#H  @(#)$Id: thread.g,v 4.50 2010/04/10 14:20:00 gap Exp $
##
#Y  Copyright (C)  1997,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
#Y  (C) 1998 School Math and Comp. Sci., University of St.  Andrews, Scotland
#Y  Copyright (C) 2002 The GAP Group
##
##  This is a minimal file to provide type information for the new types
##  provided by thread primitives.
##

Revision.thread_g :=
  "@(#)$Id: thread.g,v 4.50 2010/04/10 14:20:00 gap Exp $";


SynchronizationFamily := NewFamily("SynchronizationFamily", IsObject);

DeclareFilter("IsChannel", IsObject);
DeclareFilter("IsBarrier", IsObject);
DeclareFilter("IsSyncVar", IsObject);

TYPE_CHANNEL := NewType(SynchronizationFamily, IsChannel);
TYPE_BARRIER := NewType(SynchronizationFamily, IsBarrier);
TYPE_SYNCVAR := NewType(SynchronizationFamily, IsSyncVar);
