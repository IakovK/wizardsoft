#pragma once

#include <string>

#include <msxml6.h>

#include <wrl.h>
#include <wrl\implements.h>
#include <wrl\client.h>

struct LogItem
{
	std::wstring name;
	time_t timestamp;
	DWORD pid;
	std::wstring message;
};

struct timeVal
{
	time_t time;
};

struct pidVal
{
	DWORD pid;
};

struct XMLLog
{
private:
	bool m_bValid;
	std::wstring m_path;
public:
	XMLLog(LPCTSTR logFilePath);
	~XMLLog();
	Microsoft::WRL::ComPtr<IXMLDOMDocument> m_pDoc;
	Microsoft::WRL::ComPtr<IXMLDOMElement> m_pRoot;
	bool LoadExisting(LPCWSTR path);
	bool CreateAndInitDOM();
	bool CreateAndAddPINode(LPCWSTR wszTarget, LPCWSTR wszData);
	bool CreateRootElement();
	bool FindRootElement();
	bool CreateAndAddTextNode(LPCWSTR wszText, IXMLDOMNode* pParent);
	bool CreateAndAddElement(LPCWSTR wszName, LPCWSTR wszText, IXMLDOMNode* pParent);
	bool CreateAndAddElement(LPCWSTR wszName, timeVal val, IXMLDOMNode* pParent);
	bool CreateAndAddElement(LPCWSTR wszName, pidVal val, IXMLDOMNode* pParent);
	bool Write(const LogItem& item);
};

