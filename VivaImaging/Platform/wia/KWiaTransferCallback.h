#pragma once
#include <Wia.h>

class KWiaTransferCallback : public IWiaTransferCallback
{
public:
  KWiaTransferCallback();
  ~KWiaTransferCallback();

  long RefCount;
  //IStream *mpDestination;
  TCHAR mDownloadFolder[FILENAME_MAX]; 
  TCHAR mFilename[FILENAME_MAX];
  UINT mIndex;
  TCHAR mFileExtension[6];

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void   **ppvObject);
  ULONG STDMETHODCALLTYPE AddRef();
  ULONG STDMETHODCALLTYPE Release();

  void STDMETHODCALLTYPE SetOption(LPCTSTR downloadFolder, LPCTSTR extension);
  LPCTSTR STDMETHODCALLTYPE GetFilename() { return mFilename; }

public: // IWiaTransferCallback
  virtual HRESULT STDMETHODCALLTYPE TransferCallback(LONG lFlags, WiaTransferParams *pWiaTransferParams);
  virtual HRESULT STDMETHODCALLTYPE GetNextStream(LONG lFlags, BSTR bstrItemName, BSTR bstrFullItemName, IStream **ppDestination);

};

