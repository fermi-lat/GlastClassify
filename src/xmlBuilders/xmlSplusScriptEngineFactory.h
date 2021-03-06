/**@file xmlSplusScriptEngineFactory.h

@brief declaration of class xmlSplusScriptEngineFactory
@author T. Usher

$Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/src/xmlBuilders/xmlSplusScriptEngineFactory.h,v 1.1 2006/01/03 21:40:00 usher Exp $
*/

#ifndef GlastClassify_xmlSplusScriptEngineFactory_h
#define GlastClassify_xmlSplusScriptEngineFactory_h

#include "xmlFactoryBase.h"
#include "IxmlEngineFactory.h"
#include "../ImActivityNodes/IImActivityNode.h"

class DecisionTree;

/** @class xmlSplusScriptEngineFactory
@brief A factory for accessing decision trees

*/
class xmlSplusScriptEngineFactory : public xmlFactoryBase , public IxmlEngineFactory
{
public:
    typedef std::vector<std::string> StringList;

    /** @brief constructotor sets up for tree production 
    @param path file path to a folder containing tree data
    @param lookup Instance of a class supplied to by user, which is called back 
    to find address of each variable
    */
    xmlSplusScriptEngineFactory(XTExprsnParser& parser);

    /** @param name a folder name completing the path to the folder containing the tree data   
     @return a reference to a new tree. See also the evaluate() method.
     */
    virtual IImActivityNode* operator()(const DOMElement* element);

    virtual ~xmlSplusScriptEngineFactory();

private:
};


#endif
