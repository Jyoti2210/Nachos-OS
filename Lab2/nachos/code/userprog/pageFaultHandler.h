#include "addrspace.h"
#include "machine.h"
#include "thread.h"

class PageFaultHandler {
	public:

		static void pgFaultHandler(int badAdd);
	
};