/** @file AtwoodTrees.h
    @brief  Declare class AtwoodTrees

$Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/GlastClassify/AtwoodTrees.h,v 1.8 2006/02/08 18:43:21 burnett Exp $
*/
#ifndef GlastClassify_AtwoodTrees_h
#define GlastClassify_AtwoodTrees_h

#include "GlastClassify/ITupleInterface.h"

#include <string>
#include <iostream>
#include <vector>

namespace GlastClassify { 
    
    class ITupleInterface; 
    class TreeAnalysis;

/** @class AtwoodTrees
    @brief Manage Atwood-inspired classification trees, creating new tuple variables
    based on values found in the tuple.
*/
class AtwoodTrees 
{
public:
    /** set up the trees:
    * @param tuple -- abstract interface to a tuple that sets up access to tuple items, and creates new ones
    * @param treepath -- file path to the root of the tree definitions

    Uses the tuple object to access current tuple items, and to create new ones.
    */
    AtwoodTrees( ITupleInterface& tuple, std::ostream& log=std::cout, std::string treepath  ="");

    /** run the prediction nodes on the current tuple instance
    */
    bool execute();  

    ~AtwoodTrees();

private:

    // These are variables used by the code
    const Item*   m_TkrNumTracks;
    const Item*   m_CalEnergyRaw  ;
    const Item*   m_CalCsIRLn   ;  
    const Item*   m_FilterStatus_HI;

    // These are variables to be output to the ntuple 
    // (in alphabetical order)
    float         m_acdLowerTileCount;
    float         m_acdUpperTileCount;
    float         m_bestEnergyProb;
    float         m_CORE;
    float         m_evtLogEnergyRaw;
    float         m_tkrLATEdge;

    const Item*   m_AcdActiveDist3D;
    const Item*   m_AcdRibbonActDist;
    const Item*   m_AcdCornerDoca;
    const Item*   m_Tkr1SSDVeto;

    int           m_executeTreeCnt;
    int           m_goodVals;
    int           m_caughtVals;

    // stream for output (if any)
    std::ostream& m_log;

    // Pointer to the classification tree analysis
    TreeAnalysis* m_treeAnalysis;
 };

} // namespace GlastClassify

#endif
