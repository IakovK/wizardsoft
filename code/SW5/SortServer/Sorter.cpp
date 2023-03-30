// Sorter.cpp : Implementation of CSorter

#include "pch.h"
#include "Sorter.h"

#include <iostream>
#include <comutil.h>

#include <boost/iostreams/device/mapped_file.hpp>

#include "WikiSort.cpp"

// CSorter

template <typename RandomAccessIterator, typename CancelTest>
void CopyCancelable(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator dest, CancelTest isCanceled)
{
    size_t numElems = last - first;
    size_t portionSize = 100000;
    while (numElems != 0)
    {
        if (isCanceled())
            return;
        size_t eltsToCopy = portionSize;
        if (eltsToCopy > numElems)
            eltsToCopy = numElems;
        std::copy(first, first + eltsToCopy, dest);
        first += eltsToCopy;
        dest += eltsToCopy;
        numElems -= eltsToCopy;
    }
}

// mode == 0 => ascending order, mode == 1 => descending order
STDMETHODIMP CSorter::SortFile(BSTR input, BSTR output, BYTE mode)
{
    bstr_t i(input);
    bstr_t o(output);
    std::string strInput = (const char*)i;
    std::string strOutput = (const char*)o;
    if (m_busy)
        return HRESULT_FROM_WIN32(ERROR_INVALID_OPERATION);
    m_busy = true;
    m_canceled = false;
    boost::iostreams::mapped_file_source inFile;
    boost::iostreams::mapped_file_params inParams(strInput);
    try
    {
        inFile.open(inParams);
    }
    catch (const std::exception&)
    {
        return E_FAIL;
    }
    if (!inFile.is_open())
    {
        return E_FAIL;
    }
    boost::iostreams::mapped_file_sink outFile;
    boost::iostreams::mapped_file_params outParams(strOutput);
    outParams.new_file_size = inFile.size();
    try
    {
        outFile.open(outParams);
    }
    catch (const std::exception&)
    {
        return E_FAIL;
    }
    if (!outFile.is_open())
    {
        return E_FAIL;
    }
    std::size_t numElements = inFile.size() / sizeof(std::uint32_t);
    std::uint32_t* inPtr = (std::uint32_t*)inFile.data();
    std::uint32_t* outPtr = (std::uint32_t*)outFile.data();
    auto cancelTest = [this]()
    {
        return m_canceled;
    };

    CopyCancelable(inPtr, inPtr + numElements, outPtr, cancelTest);

    std::cout << "CSorter::SortFile: sorting file" << "\n";
    if (mode == 0)
    {
        auto comp = std::less<std::uint32_t>{};
        Wiki::Sort(outPtr, outPtr + numElements, comp, cancelTest);
    }
    else
    {
        auto comp = std::greater<std::uint32_t>{};
        Wiki::Sort(outPtr, outPtr + numElements, comp, cancelTest);
    }
    inFile.close();
    outFile.close();
    m_busy = false;
    return S_OK;
}


STDMETHODIMP CSorter::Cancel()
{
    m_canceled = true;
    return S_OK;
}
