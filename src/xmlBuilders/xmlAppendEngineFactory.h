/**@file xmlAppendEngineFactory.h

@brief declaration of class xmlAppendEngineFactory
@author T. Usher

$Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/src/xmlBuilders/xmlAppendEngineFactory.h,v 1.1 2005/11/07 21:50:54 usher Exp $
*/

#ifndef GlastClassify_xmlAppendEngineFactory_h
#define GlastClassify_xmlAppendEngineFactory_h

#include "xmlFactoryBase.h"
#include "IxmlEngineFactory.h"
#include "../ImActivityNodes/IImActivityNode.h"

class DecisionTree;

/** @class xmlAppendEngineFactory
@brief A factory for accessing decision trees

*/
class xmlAppendEngineFactory : public xmlFactoryBase , public IxmlEngineFactory
{
public:
    typedef std::vector<std::string> StringList;

    /** @brief constructotor sets up for tree production 
    @param path file path to a folder containing tree data
    @param lookup Instance of a class supplied to by user, which is called back 
    to find address of each variable
    */
    xmlAppendEngineFactory(XTExprsnParser& parser);

    /** @param name a folder name completing the path to the folder containing the tree data   
     @return a reference to a new tree. See also the evaluate() method.
     */
    virtual IImActivityNode* operator()(const DOMElement* element);

    virtual ~xmlAppendEngineFactory();

private:
};


#endif
