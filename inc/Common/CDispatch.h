
#pragma once

enum E_OLEVarType
{
	OLEVT_Boolean = VT_BOOL,
	OLEVT_Integer = VT_I2,
	OLEVT_Long = VT_I4,
	OLEVT_Single = VT_R4,
	OLEVT_Double = VT_R8,
	OLEVT_String = VT_BSTR,
	OLEVT_Object = VT_DISPATCH
};

typedef SHORT OLEBoolean;
typedef SHORT OLEInteger;
typedef LONG OLELong;
typedef FLOAT OLESingle;
typedef DOUBLE OLEDouble;
typedef LPWSTR OLEString;
typedef LPDISPATCH OLEObject;

class __CommonExt CVariant
{
public:
	CVariant()
	{
		VariantInit(&m_varValue);
	}

	CVariant(const ST_OLEVarType &nVarType, const LPVOID pValue)
	{
		m_varValue.vt = nVarType;

		switch (nVarType)
		{
		case OLEVT_Boolean:
			m_varValue.boolVal = *(OLEBoolean*)pValue;

			break;
		case OLEVT_Integer:
			m_varValue.iVal = *(OLEInteger*)pValue;

			break;
		case OLEVT_Long:
			m_varValue.lVal = *(OLELong*)pValue;

			break;
		case OLEVT_Single:
			m_varValue.fltVal = *(OLESingle*)pValue;
			
			break;
		case OLEVT_Double:
			m_varValue.dblVal = *(OLEDouble*)pValue;
			
			break;
		case OLEVT_String:
			m_varValue.bstrVal = (OLEString)pValue;

			break;
		case OLEVT_Object:
			m_varValue.pdispVal = *(LPDISPATCH*)pValue;

			break;
		default:
			break;
		}
	}

private:
	VARIANT m_varValue;

public:
	VARIANT* GetRef()
	{
		return &m_varValue;
	}

	BOOL GetValue(const ST_OLEVarType &nVarType, const LPVOID pValue)
	{
		if (VT_EMPTY == m_varValue.vt)
		{
			return TRUE;
		}
		
		if (nVarType != m_varValue.vt)
		{
			return FALSE;
		}

		switch (m_varValue.vt)
		{
		case OLEVT_Boolean:
			*(OLEBoolean*)pValue = m_varValue.boolVal;

			break;
		case OLEVT_Integer:
			*(OLEInteger*)pValue = m_varValue.iVal;

			break;
		case OLEVT_Long:
			*(OLELong*)pValue = m_varValue.lVal;

			break;
		case OLEVT_Single:
			*(OLESingle*)pValue = m_varValue.fltVal;
			
			break;
		case OLEVT_Double:
			*(OLEDouble*)pValue = m_varValue.dblVal;
			
			break;
		case OLEVT_String:
			*(OLEString*)pValue = m_varValue.bstrVal;

			break;
		case OLEVT_Object:
			*(LPDISPATCH*)pValue = m_varValue.pdispVal;

			break;
		default:
			break;
		}

		return TRUE;
	}
};

class __CommonExt CParams
{
typedef vector<CVariant> ParamVector;

public:
	CParams()
	{
		m_pValue = NULL;
	
		memset(&m_Params, 0, sizeof m_Params);
	}

	~CParams()
	{
		Clear();
	}

	CVariant* m_pValue;

private:
	ParamVector m_vctParams;

	DISPPARAMS m_Params;

public:
	void Clear()
	{
		if (m_Params.rgvarg)
		{
			delete [] m_Params.rgvarg;
		}

		if (m_Params.rgdispidNamedArgs)
		{
			delete m_Params.rgdispidNamedArgs;
		}

		for (ParamVector::iterator itParam=m_vctParams.begin(); itParam!=m_vctParams.end(); ++itParam)
		{
			if (OLEVT_String == itParam->GetRef()->vt && itParam->GetRef()->bstrVal)
			{
				::SysFreeString(itParam->GetRef()->bstrVal);
			}
		}
		m_vctParams.clear();

		m_pValue = NULL;

		memset(&m_Params, 0, sizeof m_Params);
	}

	void AddParam(CVariant Param)
	{
		m_vctParams.push_back(Param);
	}

	DISPPARAMS* GetRef()
	{
		m_Params.cArgs = (UINT)m_vctParams.size();
		if (m_pValue)
		{
			m_Params.cArgs++;
		}

		m_Params.rgvarg = new VARIANTARG[m_Params.cArgs];
		
		if (m_pValue)
		{
			m_Params.cNamedArgs = 1;
			m_Params.rgdispidNamedArgs =new DISPID(DISPID_PROPERTYPUT);

			m_Params.rgvarg[0]=*(m_pValue->GetRef());

			for (UINT uIndex = 0; uIndex < m_vctParams.size(); ++uIndex)
			{
				m_Params.rgvarg[uIndex +1] = *(m_vctParams[m_vctParams.size()-1- uIndex].GetRef());
			}
		}
		else
		{
			for (UINT uIndex = 0; uIndex < m_vctParams.size(); ++uIndex)
			{
				m_Params.rgvarg[i] = *(m_vctParams[m_vctParams.size()-1-i].GetRef());
			}
		}

		return &m_Params;
	}
};

class __CommonExt CDispatch
{
	typedef map<string, DISPID> DispIDMap;

public:
	CDispatch(const string& strProgID)
		:m_strProgID(strProgID)
	{
		m_dwCreateFlag = CLSCTX_INPROC_SERVER;

		m_pObj = NULL;

		memset(&m_errInfo, 0, sizeof m_errInfo);
	}

	CDispatch(LPDISPATCH pObj)
		:m_pObj(pObj)
	{
		m_dwCreateFlag = CLSCTX_INPROC_SERVER;

		memset(&m_errInfo, 0, sizeof m_errInfo);
	}

	~CDispatch(void)
	{
		Destroy();
	}

private:
	string m_strProgID;

	DWORD m_dwCreateFlag;

	LPDISPATCH m_pObj;

	DispIDMap m_mapDispIDs;

	CVariant m_varRet;
	
	EXCEPINFO m_errInfo;

private:
	DISPID GetDispID(const string& strName)
	{
		DispIDMap::iterator itDispID = m_mapDispIDs.find(strName);
		if (itDispID != m_mapDispIDs.end())
		{
			return itDispID->second;
		}

		DISPID nDispID = -1;

		USES_CONVERSION;
		LPWSTR lpszName = A2W(strName.c_str());

		if (S_OK == m_pObj->GetIDsOfNames(IID_NULL, &lpszName, 1, LOCALE_USER_DEFAULT, &nDispID))
		{
			m_mapDispIDs[strName] = nDispID;
		}

		return nDispID;
	}

	BOOL Dispatch(const int &nFlag, const string& strName, CParams *pParams, CVariant* pRet=NULL)
	{
		ASSERT(m_pObj);

		DISPID nDispID = 0;
		if (!strName.empty())
		{
			nDispID = GetDispID(strName);
			if (nDispID<0)
			{
				return FALSE;
			}
		}

		memset(&m_errInfo, 0, sizeof m_errInfo);
		HRESULT nRet = m_pObj->Invoke(nDispID, IID_NULL, LOCALE_USER_DEFAULT, nFlag, pParams?pParams->GetRef():&DISPPARAMS(), pRet?pRet->GetRef():NULL, &m_errInfo, NULL);
		if (S_OK != nRet)
		{
			if (DISP_E_EXCEPTION == nRet)
			{
				(void)m_errInfo;
			}

			return FALSE;
		}

		return TRUE;
	}

public:
	CVariant* GetPropEx(const string& strName, CParams *pParams)
	{
		ASSERT(pParams);

		CVariant* pRet = GetProp(strName);
	
		if (!pRet)
		{
			return NULL;
		}

		LPDISPATCH pObj = NULL;
		if (!pRet->GetValue(OLEVT_Object, &pObj))
		{
			return NULL;
		}

		if (!pObj)
		{
			return NULL;
		}

		CDispatch Obj(pObj);
		CVariant* pSubRet = Obj.GetProp("", pParams);

		if (!pSubRet)
		{
			return NULL;
		}


		LPDISPATCH pSubObj = NULL;
		if (!pSubRet->GetValue(OLEVT_Object, &pSubObj))
		{
			return NULL;
		}

		if (!pSubObj)
		{
			return NULL;
		}

		CDispatch SubObj(pSubObj);
		return SubObj.GetProp("");
	}

	BOOL SetPropEx(const string& strName, CVariant* pValue, CParams *pParams)
	{
		ASSERT(pValue);
		ASSERT(pParams);

		CVariant* pRet = GetProp(strName);
	
		if (!pRet)
		{
			return FALSE;
		}

		LPDISPATCH pObj = NULL;
		if (!pRet->GetValue(OLEVT_Object, &pObj))
		{
			return FALSE;
		}

		if (!pObj)
		{
			return FALSE;
		}

		CDispatch Obj(pObj);
		CVariant* pSubRet = Obj.GetProp("", pParams);

		if (!pSubRet)
		{
			return FALSE;
		}


		LPDISPATCH pSubObj = NULL;
		if (!pSubRet->GetValue(OLEVT_Object, &pSubObj))
		{
			return FALSE;
		}

		if (!pSubObj)
		{
			return FALSE;
		}

		CDispatch SubObj(pSubObj);
		return SubObj.SetProp("", pValue);
	}

	CVariant* GetProp(const string& strName, CParams *pParams=NULL)
	{
		m_varRet = CVariant();

		if (!Dispatch(DISPATCH_PROPERTYGET, strName, pParams, &m_varRet))
		{
			return NULL;
		}

		return &m_varRet;
	}

	BOOL SetProp(const string& strName, CVariant* pValue, CParams *pParams=NULL)
	{
		ASSERT(pValue);

		CParams Params;
		if (!pParams)
		{
			pParams = &Params;
		}
		pParams->m_pValue = pValue;

		return Dispatch(DISPATCH_PROPERTYPUT, strName, pParams);
	}

	BOOL CallProc(const string& strName, CParams *pParams=NULL)
	{
		return Dispatch(DISPATCH_METHOD, strName, pParams);
	}

	CVariant* CallFunc(const string& strName, CParams *pParams=NULL)
	{
		m_varRet = CVariant();

		if (!Dispatch(DISPATCH_METHOD, strName, pParams, &m_varRet))
		{
			return NULL;
		}

		return &m_varRet;
	}

	BOOL CreateServer()
	{
		m_dwCreateFlag = CLSCTX_LOCAL_SERVER;

		return Create();
	}

	BOOL Create()
	{
		USES_CONVERSION;
		LPCWSTR lpszProgID = A2W(m_strProgID.c_str());

		IID iidCls;
		if (S_OK != CLSIDFromProgID(lpszProgID, &iidCls))
		{
			return FALSE;
		}
		
		if (!AfxGetThreadState()->m_bNeedTerm && !AfxOleInit())
		{
			return FALSE;
		}
		
		//if (S_OK == CoCreateInstance(iidCls, NULL, m_dwCreateFlag, IID_IDispatch, (LPVOID*)&m_pObj))
		//{
		//	return TRUE;
		//}

		IClassFactory* pClsFct = NULL;
		if (S_OK != CoGetClassObject(iidCls, m_dwCreateFlag, NULL, IID_IClassFactory, (LPVOID*)&pClsFct))
		{
			return FALSE; 
		}

		if (S_OK != pClsFct->CreateInstance(NULL, IID_IDispatch, (LPVOID*)&m_pObj))
		{
			pClsFct->Release();
		
			return FALSE;
		}

		pClsFct->Release();
		
		return TRUE;
	}

	void Destroy()
	{
		if (m_pObj)
		{
			m_pObj->Release();	//CoDisconnectObject(m_pObj, NULL);

			m_pObj = NULL;
		}
	}

	static void Test()
	{
		CDispatch Obj("adodb.connection");
		if (Obj.Create())
		{
			CParams params;
			BOOL bRet = FALSE;
			CVariant* pRet = NULL;

			bRet = Obj.CallProc("cancel");
			ASSERT(bRet);
			
			LONG nCommandTimeout=123;
			bRet = Obj.SetProp("CommandTimeout", &CVariant(OLEVT_Long, &nCommandTimeout));
			ASSERT(bRet);
			
			pRet = Obj.GetProp("CommandTimeout");
			ASSERT(pRet);

			LONG nIndex = 0; 
			params.Clear();
			params.AddParam(CVariant(OLEVT_Long, &nIndex));

			bRet = Obj.SetPropEx("Properties", &CVariant(OLEVT_String, A2BSTR("aaaaaa")), &params);
			ASSERT(bRet);

			pRet = Obj.GetPropEx("Properties", &params);
			LPWSTR s=0;
			pRet->GetValue(OLEVT_String, &s);
			//AfxMessageBox(CString(s));
			ASSERT(pRet);
		}
	}
};
