#include "pch.h"
#include "XMLLog.h"

XMLLog::XMLLog(LPCTSTR logFilePath)
	:m_bValid(false),
	m_path(logFilePath)
{
	HRESULT hres = CoCreateInstance(__uuidof(DOMDocument60), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(m_pDoc.GetAddressOf()));
	if (FAILED(hres))
	{
		return;
	}
	if (!LoadExisting(m_path.c_str()))
	{
		if (!CreateAndInitDOM())
			return;
		if (!CreateAndAddPINode(L"xml", L"version='1.0'"))
			return;
		if (!CreateRootElement())
			return;
	}
	m_bValid = true;
}

XMLLog::~XMLLog()
{
	if (m_bValid)
	{
		_variant_t v{ m_path.c_str() };
		HRESULT hr = m_pDoc->save(v);
	}
}

bool XMLLog::LoadExisting(LPCTSTR path)
{
	_variant_t v{ path };
	VARIANT_BOOL success{ VARIANT_FALSE };
	HRESULT hr = m_pDoc->load(v, &success);

	if (FAILED(hr))
		return false;
	if (!success)
		return false;

	if (!FindRootElement())
		return false;

	return true;
}

bool XMLLog::CreateAndInitDOM()
{
	m_pDoc->put_async(VARIANT_FALSE);
	m_pDoc->put_validateOnParse(VARIANT_FALSE);
	m_pDoc->put_resolveExternals(VARIANT_FALSE);
	m_pDoc->put_preserveWhiteSpace(VARIANT_TRUE);
	return true;
}

bool XMLLog::CreateAndAddPINode(LPCTSTR wszTarget, LPCTSTR wszData)
{
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<IXMLDOMProcessingInstruction> pPI;

	_bstr_t bstrTarget(wszTarget);
	_bstr_t bstrData(wszData);

	if (FAILED(m_pDoc->createProcessingInstruction(bstrTarget, bstrData, pPI.GetAddressOf())))
		return false;
	Microsoft::WRL::ComPtr<IXMLDOMNode> pChildOut;
	if (FAILED(m_pDoc->appendChild(pPI.Get(), pChildOut.GetAddressOf())))
		return false;

	return true;
}

bool XMLLog::CreateRootElement()
{
	if (FAILED(m_pDoc->createElement(_bstr_t{ L"root" }, m_pRoot.GetAddressOf())))
		return false;
	// Add NEWLINE for identation before </root>.
	if (!CreateAndAddTextNode(L"\n", m_pRoot.Get()))
		return false;
	Microsoft::WRL::ComPtr<IXMLDOMNode> n;
	if (FAILED(m_pDoc->appendChild(m_pRoot.Get(), n.GetAddressOf())))
		return false;

	return true;
}

bool XMLLog::CreateAndAddTextNode(LPCWSTR wszText, IXMLDOMNode* pParent)
{
	Microsoft::WRL::ComPtr<IXMLDOMText> pText;

	if (FAILED(m_pDoc->createTextNode(bstr_t{ wszText }, &pText)))
		return false;
	Microsoft::WRL::ComPtr<IXMLDOMNode> n;
	if (FAILED(pParent->appendChild(pText.Get(), &n)))
		return false;

	return true;
}

bool XMLLog::FindRootElement()
{
	Microsoft::WRL::ComPtr<IXMLDOMNode> n;
	if (FAILED(m_pDoc->get_firstChild(n.GetAddressOf())))
		return false;
	while (true)
	{
		bstr_t s;
		if (FAILED(n->get_baseName(s.GetAddress())))
			return false;
		if (s == bstr_t{ L"root" })
		{
			if (SUCCEEDED(n.As(&m_pRoot)))
				return true;
		}
		if (FAILED(n->get_nextSibling(n.GetAddressOf())))
			return false;
		if (n == nullptr)
			return false;
	}
	return false;
}

bool XMLLog::CreateAndAddElement(LPCWSTR wszName, LPCWSTR wszText, IXMLDOMNode* pParent)
{
	Microsoft::WRL::ComPtr<IXMLDOMElement> n;
	Microsoft::WRL::ComPtr<IXMLDOMNode> n1;
	if (FAILED(m_pDoc->createElement(_bstr_t{ wszName }, n.GetAddressOf())))
		return false;
	if (!CreateAndAddTextNode(wszText, n.Get()))
		return false;
	if (FAILED(pParent->appendChild(n.Get(), n1.GetAddressOf())))
		return false;

	return true;
}

bool XMLLog::CreateAndAddElement(LPCWSTR wszName, timeVal val, IXMLDOMNode* pParent)
{
	WCHAR timeString[std::size("yyyy-mm-ddThh:mm:ssZ")];
	tm t;
	gmtime_s(&t, &val.time);
	wcsftime(std::data(timeString), std::size(timeString), L"%FT%TZ", &t);

	if (!CreateAndAddElement(wszName, timeString, pParent))
		return false;

	return true;
}

bool XMLLog::CreateAndAddElement(LPCWSTR wszName, pidVal val, IXMLDOMNode* pParent)
{
	WCHAR pidString[100];
	_ltow_s(val.pid, pidString, 10);

	if (!CreateAndAddElement(wszName, pidString, pParent))
		return false;

	return true;
}

bool XMLLog::Write(const LogItem& item)
{
	if (!m_bValid)
		return false;
	Microsoft::WRL::ComPtr<IXMLDOMElement> logEntry;
	Microsoft::WRL::ComPtr<IXMLDOMNode> n1;
	if (FAILED(m_pDoc->createElement(_bstr_t{ item.name.c_str() }, logEntry.GetAddressOf())))
		return false;

	if (!CreateAndAddElement(L"TimeStamp", timeVal{ item.timestamp }, logEntry.Get()))
		return false;

	if (!CreateAndAddElement(L"Pid", pidVal{ item.pid }, logEntry.Get()))
		return false;

	if (!item.message.empty())
	{
		if (!CreateAndAddElement(L"Message", item.message.c_str(), logEntry.Get()))
			return false;
	}

	if (FAILED(m_pRoot->appendChild(logEntry.Get(), n1.GetAddressOf())))
		return false;

	return true;
}
