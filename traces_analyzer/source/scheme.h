#pragma once

#include "helpers/string.h"

#define SCHEME_FILE_EXTENSTION          TEXT("xml")
#define SCHEME_DEFAULT_FILE_NAME        TEXT("New scheme")
#define SCHEME_DEFAULT_FILE_NAME_FILTER TEXT("Traces analyzer scheme (*.xml) | *.xml|")

class Scheme
{
public:
    Scheme();
    ~Scheme();

public:
    //bool Load(tstring const& filePath);
    bool Save(tstring const& filePath = tstring());
    bool IsModified() const;
    tstring GetFileName() const;
    tstring GetDisplayFileName() const;

private:
    tstring m_fileName;
};

