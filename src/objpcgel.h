/****************************************************************************
**
*W  objpcgel.h                  GAP source                       Frank Celler
**
*H  @(#)$Id$
**
*Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
*/
#ifdef  INCLUDE_DECLARATION_PART
SYS_CONST char * Revision_objpcgel_h =
   "@(#)$Id$";
#endif


/****************************************************************************
**

*V  PCWP_FIRST_ENTRY  . . . . . . . . . . . . . . first entry in subrep of AW
*/
#define PCWP_FIRST_ENTRY        AWP_FIRST_FREE


/****************************************************************************
**
*V  PCWP_NAMES  . . . . . . . . . . . . . . . . . . . . . . . . list of names
*/
#define PCWP_NAMES              (PCWP_FIRST_ENTRY+1)


/****************************************************************************
**
*V  PCWP_COLLECTOR  . . . . . . . . . . . . . . . . . . . .  collector to use
*/
#define PCWP_COLLECTOR          (PCWP_FIRST_ENTRY+2)


/****************************************************************************
**
*V  PCWP_FIRST_FREE . . . . . . . . . . . . .  first free position for subrep
*/
#define PCWP_FIRST_FREE         (PCWP_FIRST_ENTRY+3)


/****************************************************************************
**
*V  COLLECTOR_PCWORD( <obj> ) . . . . . . . . . . . . . .  collector of <obj>
*/
#define COLLECTOR_PCWORD(obj) \
    ( ELM_PLIST( TYPE_DATOBJ(obj), PCWP_COLLECTOR ) )


/****************************************************************************
**

*F * * * * * * * * * * * * * initialize package * * * * * * * * * * * * * * *
*/


/****************************************************************************
**


*F  SetupPcElements() . . . . . . .  initialize the pc group elements package
*/
extern void SetupPcElements ( void );


/****************************************************************************
**
*F  InitPcElements()  . . . . . . .  initialize the pc group elements package
*/
extern void InitPcElements ( void );


/****************************************************************************
**
*F  CheckPcElements() . check initialisation of the pc group elements package
*/
extern void CheckPcElements ( void );


/****************************************************************************
**

*E  objpcgel.h  . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
*/

