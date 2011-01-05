#include "canycontainer.h"

namespace ceng {

std::list< const CAnyContainer* > CAnyContainer::myDeleteBanList;

std::ostream& operator << ( std::ostream& os, CAnyContainer& container )
{
        os << container.GetValue();
        return os;
}

}