#include "MetaDataContainer.h"
#include <iostream>

MetaDataContainer::MetaDataContainer(Cursor const& cursor)
{
    auto children = cursor.getChildren();
    for(auto& child : children)
    {
        if(child.getKind() == CXCursor_AnnotateAttr)
        {
            auto displayname = child.getDisplayName();
            std::cout << "attribute str: " << displayname << std::endl;
        }
    }
}