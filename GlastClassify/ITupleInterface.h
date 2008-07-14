/** @file ITupleInterface.h
@brief  Declare abstract class ITupleInterface

$Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/GlastClassify/ITupleInterface.h,v 1.5 2007/07/13 18:36:49 usher Exp $
*/


#ifndef GlastClassify_ITupleInterface_h
#define GlastClassify_ITupleInterface_h

#include <string>

namespace GlastClassify{


    /** @class Item

    Nested abstract class definition
    */
    class Item {
    public:
        virtual ~Item(){};
        virtual operator double()const = 0;
        virtual void setDataValue(void* data) = 0;
    protected:
        Item(){};
    };

    /** @class ITupleInterface
    @brief nested class definition interface that must be implemented by client
    */
    class ITupleInterface 
    {
    public:
        virtual ~ITupleInterface(){}

        // set an object that evaluates at run-time to the current value of the item
        virtual const Item* getItem(const std::string& name)const=0;

// LSR 14-Jul-08 code for ntuple types

        /// create a new item (float only for now) in the tuple, which will take the given value
        virtual void addItem(const std::string& name, float & value)=0;
        virtual void addItem(const std::string& name, double & value)=0;
        virtual void addItem(const std::string& name, unsigned long long & value)=0;
    protected:
        ITupleInterface(){};
    };

}
#endif

