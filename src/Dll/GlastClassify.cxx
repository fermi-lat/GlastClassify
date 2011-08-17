/*
* @file GlastClassify_load.cxx
* @brief This is needed for forcing the linker to load all components
* of the library.
*
*  $Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/src/Dll/GlastClassify.cxx,v 1.1 2006/10/23 21:32:19 usher Exp $
*/

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(GlastClassify) {
    DECLARE_ALGORITHM(ClassifyAlg);
    DECLARE_TOOL(ClassifyTool);
} 



