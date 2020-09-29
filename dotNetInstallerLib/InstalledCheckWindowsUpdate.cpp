#include "stdafx.h"
#include "InstallerSession.h"
#include "XmlAttribute.h"
#include "InstalledCheckWindowsUpdate.h"
#include "InstallerLog.h"


InstalledCheckWindowsUpdate::InstalledCheckWindowsUpdate()
{
}

void InstalledCheckWindowsUpdate::Load(tinyxml2::XMLElement * node)
{
	kbId = node->Attribute("kb_id");
	defaultValue = node->Attribute("default_value");

	LOG(L"Loaded 'wus' installed check '" << kbId << L"'");
}

bool InstalledCheckWindowsUpdate::IsInstalled() const
{
	HRESULT hResult;
	IWbemLocator* pLoc = NULL;
	IWbemServices* pSvc = NULL;
	IEnumWbemClassObject* pEnumerator = NULL;
	IWbemClassObject* pClassObject = NULL;
	ULONG uReturn = 0;
	bool bHaveHotfix = defaultValue.GetBoolValue(false);
	
	hResult = CoInitializeEx(0, COINIT_MULTITHREADED);

	if (FAILED(hResult))
	{
		LOG(L"Failed to initialize COM library for WUS query: 0x" << std::hex << hResult << L"");
		return bHaveHotfix;
	}

	hResult = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

	if (FAILED(hResult))
	{
		LOG(L"Failed to initialize security for WUS query: 0x" << std::hex << hResult << L"");
		return bHaveHotfix;
	}

	hResult = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc);
	
	if (FAILED(hResult))
	{
		LOG(L"Failed to initialize IWbemLocator object for WUS query: 0x" << std::hex << hResult << L"");
		CoUninitialize();
		return bHaveHotfix;
	}

	hResult = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc);

	if (FAILED(hResult))
	{
		LOG(L"Could not connect to WMI host for WUS query: 0x" << std::hex << hResult << L"");
		pLoc->Release();
		CoUninitialize();
		return bHaveHotfix;
	}

	hResult = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

	if (FAILED(hResult))
	{
		LOG(L"Could not connect to WMI host for WUS query: 0x" << std::hex << hResult << L"");
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return bHaveHotfix;
	}

	hResult = pSvc->ExecQuery(
		bstr_t("WQL"),
		((bstr_t)SysAllocString(L"SELECT * FROM Win32_QuickFixEngineering WHERE HotFixID = \"") + (bstr_t)SysAllocString(kbId.GetValue().c_str()) + (bstr_t)SysAllocString(L"\"")).GetBSTR(),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hResult))
	{
		LOG(L"Querying of WMI host for WUS informaiton failed.");
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return bHaveHotfix;
	}
	
	LOG(L"Checking for " << kbId)
	hResult = pEnumerator->Next(WBEM_INFINITE, 1, &pClassObject, &uReturn);

	if (SUCCEEDED(hResult))
	{
		bHaveHotfix = uReturn > 0;
	}
	else
	{
		LOG(L"Failed to iterate WMI result set.");
	}

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return bHaveHotfix;
}