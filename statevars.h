/*******************************************************************
*
*  DESCRIPTION: class StateVars
*
*  AUTHOR: Alejandro López
*
*  EMAIL: mailto://alopez@dc.uba.ar
*         
*
*  REVISION: $Revision$  $Date$
*
*******************************************************************/

#ifndef __STATEVARS_H
#define __STATEVARS_H

#include <string>
#include <utility>
#include <map>

# include "except.h"
# include "real.h"

class StateVarsException : public MException
{
    public:
    StateVarsException(const string &str = "Undefined state variable"): 
                MException(str)  {} ;
};

class StateVars : protected map<const string, Real>
{
    public:

    inline bool exist(const string& name) const
    { return (find(name) != end()); }
    
    bool createVariable(const string& name, Real& value = Real::tundef);

    Real &get(const string &name) const;           //throws StateVarsException
    Real &set(const string &name, Real &newValue); //throws StateVarsException
    
    StateVars& operator=(const StateVars &src);
    
    StateVars& setValues(const string &values);

    string asString(void) const;
    
    inline void print(ostream& os) const
    { os << asString(); }
    
    void clear()
    { order.clear(); this->map<const string, Real>::clear(); }
    
    
    protected:
    
    inline bool exist(int index) const
    { return (order.find(index) != end()); }
    
    const string& operator[](int index) const;

    private:
    map<int, string> order;
};



#endif // __STATEVARS_H
