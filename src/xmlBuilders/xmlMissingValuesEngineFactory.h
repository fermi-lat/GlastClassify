/**@file xmlMissingValuesEngineFactory.h

@brief declaration of class xmlMissingValuesEngineFactory
@author T. Usher

$Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/GlastClassify/xmlMissingValuesEngineFactory.h,v 1.1 2005/11/04 23:19:05 usher Exp $
*/

#ifndef GlastClassify_xmlMissingValuesEngineFactory_h
#define GlastClassify_xmlMissingValuesEngineFactory_h

#include "xmlFactoryBase.h"
#include "IxmlEngineFactory.h"
#include "../ImActivityNodes/IImActivityNode.h"

class DecisionTree;

/** @class xmlMissingValuesEngineFactory
@brief A factory for accessing decision trees

*/
class xmlMissingValuesEngineFactory : public xmlFactoryBase, public IxmlEngineFactory 
{
public:
    typedef std::vector<std::string> StringList;

    /** @brief constructotor sets up for tree production 
    @param path file path to a folder containing tree data
    @param lookup Instance of a class supplied to by user, which is called back 
    to find address of each variable
    */
    xmlMissingValuesEngineFactory(std::ostream& log=std::cout, int iVerbosity=0);

    /** @param name a folder name completing the path to the folder containing the tree data   
     @return a reference to a new tree. See also the evaluate() method.
     */
    IImActivityNode* operator()(const DOMElement* element);

    virtual ~xmlMissingValuesEngineFactory();

private:

    std::ostream&                m_log;         //! output to this stream
    int                          m_outputLevel; //! output level (verbosity)

};


#endif
