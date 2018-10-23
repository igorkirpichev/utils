#include "scheme.h"

#include "tinyxml/tinyxml.h"

Scheme::Scheme()
{
}


Scheme::~Scheme()
{
}

bool Scheme::Load(tstring const& filePath)
{
    TiXmlDocument doc(ToString(filePath));
    return false;

}