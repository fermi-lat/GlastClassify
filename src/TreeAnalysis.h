/**@file TreeAnalysis.h

@brief declaration of class TreeAnalysis
@author T. Burnett

$Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/GlastClassify/TreeAnalysis.h,v 1.2 2005/11/09 00:04:36 usher Exp $
*/

#ifndef GlastClassify_TreeAnalysis_h
#define GlastClassify_TreeAnalysis_h

#include "GlastClassify/ITupleInterface.h"

class IImActivityNode;

#include <vector>
#include <utility>
#include <map>

#include "src/XT/XTtupleVars.h"

namespace GlastClassify {

/** @class TreeAnalysis
@brief A factory for accessing decision trees

*/
class TreeAnalysis  
{
public:

    /** @brief ctor sets up for tree production 
    @param path file path to a folder containing tree data
    @param lookup Instance of a class supplied to by user, which is called back 
    to find address of each variable
    */
    TreeAnalysis(ITupleInterface& tuple);

    ~TreeAnalysis();

    /** @brief This will execute the TreeAnalysis nodes with the current event
    */
    void execute();

    /** @brief Look up the value of a variable stored in the local tuple
               This should be the "standard" method for retrieving information
    */
    double getTupleVal(const std::string& name);

    /** @brief This gives full access to the XTtupleMap
               This is really intended to be used by the TreeAnalysis "Builder"
               not by casual users!
    */
    XTcolumnVal<double>::XTtupleMap& xtTupleMap() {return m_xtTupleMap;}

    /** @brief Used by the TreeAnalysisBuilder to add a new node to our structure
    */
    void addNewNode(IImActivityNode* node);

    /** @brief Used by the TreeAnalysisBuilder, once all nodes have been added,
               to set the "head" node (the first to be executed
    */
    void setHeadNode(IImActivityNode* head) {m_headNode = head;}

    // For output
    void print(std::ostream& out=std::cout) const;

private:

    // Mapping between the ActivityNode type and a vector of pointers to these nodes
    typedef std::map<std::string, std::vector<IImActivityNode*> > typeToINodeVecMap;

    // The "head" ActivityNode
    IImActivityNode*                   m_headNode;

    // This looks up the values in the output ntuple
    ITupleInterface&                   m_lookup;

    // Class needed to calcluate local variables used in CT's
    XTcolumnVal<double>::XTtupleMap    m_xtTupleMap;

    // Try a lookup class here...
    std::map<std::string,const GlastClassify::Item*> m_nTupleMap;

    // Vector of Activity Nodes
    std::vector<IImActivityNode*>    m_iNodeVec;

    // Map between ActivityNode type and vector of pointers to objects
    typeToINodeVecMap                m_typeToINodeVecMap;
};


} // namespace

#endif