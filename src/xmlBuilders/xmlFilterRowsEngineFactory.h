/**@file xmlFilterRowsEngineFactory.h

@brief declaration of class xmlFilterRowsEngineFactory
@author T. Usher

$Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/src/xmlBuilders/xmlFilterRowsEngineFactory.h,v 1.2 2005/11/08 01:10:50 usher Exp $
*/

#ifndef GlastClassify_xmlFilterRowsEngineFactory_h
#define GlastClassify_xmlFilterRowsEngineFactory_h

#include "xmlFactoryBase.h"
#include "IxmlEngineFactory.h"
#include "../ImActivityNodes/IImActivityNode.h"

class DecisionTree;

/** @class xmlFilterRowsEngineFactory
@brief A factory for accessing decision trees

*/
class xmlFilterRowsEngineFactory : public xmlFactoryBase, public IxmlEngineFactory 
{
public:
    /** @brief constructotor sets up for tree production 
    @param path file path to a folder containing tree data
    @param lookup Instance of a class supplied to by user, which is called back 
    to find address of each variable
    */
    xmlFilterRowsEngineFactory(XTExprsnParser& parser);

    /** @param name a folder name completing the path to the folder containing the tree data   
     @return a reference to a new tree. See also the evaluate() method.
     */
    IImActivityNode* operator()(const DOMElement* element);

    virtual ~xmlFilterRowsEngineFactory();

private:
};


#endif
