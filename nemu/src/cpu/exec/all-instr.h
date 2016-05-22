#include "prefix/prefix.h"

#include "data-mov/mov.h"
#include "data-mov/xchg.h"
/****************************************/
/* IMPLEMENTED INSTRUCTIONS!            */
/****************************************/
#include "data-mov/push.h"

#include "arith/dec.h"
#include "arith/inc.h"
#include "arith/neg.h"
#include "arith/imul.h"
#include "arith/mul.h"
#include "arith/idiv.h"
#include "arith/div.h"
/****************************************/
/* IMPLEMENTED INSTRUCTIONS!            */
/****************************************/
#include "arith/sub.h"
#include "arith/cmp.h"

#include "logic/and.h"
#include "logic/or.h"
#include "logic/not.h"
#include "logic/xor.h"
#include "logic/sar.h"
#include "logic/shl.h"
#include "logic/shr.h"
#include "logic/shrd.h"
/****************************************/
/* IMPLEMENTED INSTRUCTIONS!            */
/****************************************/
#include "logic/test.h"

/****************************************/
/* IMPLEMENTED INSTRUCTIONS!            */
/****************************************/
#include "ctrl-transfer/call.h"
#include "ctrl-transfer/jcc/je.h"

#include "string/rep.h"

#include "misc/misc.h"

#include "special/special.h"
