#pragma once

#include "XmlAttribute.h"
#include "InstalledCheck.h"

class InstalledCheckWindowsUpdate :	public InstalledCheck
{
public:
	// Update KB ID.
	XmlAttribute kbId;
	// Default value when the Windows update store can not be queried.
	XmlAttribute defaultValue;
public:
	InstalledCheckWindowsUpdate();
	void Load(tinyxml2::XMLElement * node);
	virtual bool IsInstalled() const;
};