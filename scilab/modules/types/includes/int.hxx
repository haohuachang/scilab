/*
 *  Scilab ( http://www.scilab.org/ ) - This file is part of Scilab
 *  Copyright (C) 2008-2008 - DIGITEO - Antoine ELIAS
 *  Copyright (C) 2014 - Scilab Enterprises - Calixte DENIZET
 *
 *  This file must be used under the terms of the CeCILL.
 *  This source file is licensed as described in the file COPYING, which
 *  you should have received as part of this distribution.  The terms
 *  are also available at
 *  http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 */

//#ifndef __ARRAYOF_HXX__
//    #error This file must only be include by arrayof.hxx
//#endif

#ifndef __INT_HXX__
#define __INT_HXX__

#include "arrayof.hxx"
#include "dynlib_types.h"
#include "internal.hxx"

#include <sstream>
#include <string>
#include "tostring_common.hxx"
#include "formatmode.h"

namespace types
{
template<typename T>
class TYPES_IMPEXP Int : public ArrayOf<T>
{
public :

    Int(int _iRows, int _iCols)
    {
        int piDims[2]   = {_iRows, _iCols};
        T * pInt     = NULL;
        this->create(piDims, 2, &pInt, NULL);
#ifndef NDEBUG
        Inspector::addItem(this);
#endif
    }

    Int(T _val)
    {
        int piDims[2]   = {1, 1};
        T * pInt     = NULL;
        this->create(piDims, 2, &pInt, NULL);
        pInt[0] = _val;
#ifndef NDEBUG
        Inspector::addItem(this);
#endif
    }

    Int(int _iRows, int _iCols, T** _pData)
    {
        int piDims[2] = {_iRows, _iCols};
        this->create(piDims, 2, _pData, NULL);
#ifndef NDEBUG
        Inspector::addItem(this);
#endif
    }

    Int(int _iDims, int* _piDims)
    {
        T * pInt     = NULL;
        this->create(_piDims, _iDims, &pInt, NULL);
#ifndef NDEBUG
        Inspector::addItem(this);
#endif
    }

    ~Int()
    {
        if (InternalType::isDeletable() == true)
        {
            deleteAll();
        }
#ifndef NDEBUG
        Inspector::removeItem(this);
#endif
    }

    InternalType* clone()
    {
        Int<T> *pbClone =  new Int<T>(GenericType::getDims(), GenericType::getDimsArray());
        pbClone->set(ArrayOf<T>::get());
        return pbClone;
    }

    /*Config management*/
    void whoAmI();

    bool isInt()
    {
        return true;
    }

    bool operator==(const InternalType& it)
    {
        if (const_cast<InternalType &>(it).getType() != getType())
        {
            return false;
        }

        Int<T>* pb = const_cast<InternalType &>(it).getAs<typename types::Int<T> >();

        if (pb->getDims() != GenericType::getDims())
        {
            return false;
        }

        for (int i = 0 ; i < GenericType::getDims() ; i++)
        {
            if (pb->getDimsArray()[i] != GenericType::getDimsArray()[i])
            {
                return false;
            }
        }

        if (memcmp(ArrayOf<T>::get(), pb->get(), GenericType::getSize() * sizeof(int)) != 0)
        {
            return false;
        }
        return true;
    }

    bool operator!=(const InternalType& it)
    {
        return !(*this == it);
    }

    /* return type as string ( double, int, cell, list, ... )*/
    virtual std::wstring        getTypeStr();

    /* return type as short string ( s, i, ce, l, ... )*/
    virtual std::wstring        getShortTypeStr()
    {
        return L"i";
    }

protected :
    InternalType::ScilabType getType(void);

private :
    virtual bool subMatrixToString(std::wostringstream& ostr, int* _piDims, int _iDims)
    {
        int iCurrentLine = 0;
        int iLineLen = getConsoleWidth();
        int iMaxLines = getConsoleLines();

        if (ArrayOf<T>::isScalar())
        {
            //scalar
            int iWidth  = 0;
            _piDims[0]  = 0;
            _piDims[1]  = 0;
            int iPos    = ArrayOf<T>::getIndex(_piDims);

            getSignedIntFormat(ArrayOf<T>::get(iPos), &iWidth);
            addSignedIntValue(&ostr, ArrayOf<T>::get(iPos), iWidth);
            ostr << std::endl;
        }
        else if (GenericType::getCols() == 1)
        {
            //column vector
            int iWidthMax = 0;

            //Array with the max printed size of each col
            for (int i = 0 ; i < GenericType::getRows() ; i++)
            {
                _piDims[1] = 0;
                _piDims[0] = i;
                int iWidth = 0;
                int iPos = ArrayOf<T>::getIndex(_piDims);
                getSignedIntFormat(ArrayOf<T>::get(iPos), &iWidth);
                iWidthMax = Max(iWidthMax, iWidth);
            }

            for (int i = InternalType::m_iRows1PrintState ; i < GenericType::getRows() ; i++)
            {
                iCurrentLine++;
                if ((iMaxLines == 0 && iCurrentLine >= MAX_LINES) || (iMaxLines != 0 && iCurrentLine >= iMaxLines))
                {
                    InternalType::m_iRows1PrintState = i;
                    return false;
                }

                _piDims[1]  = 0;
                _piDims[0]  = i;
                int iPos    = ArrayOf<T>::getIndex(_piDims);

                addSignedIntValue(&ostr, ArrayOf<T>::get(iPos), iWidthMax);
                ostr << std::endl;
            }
        }
        else if (GenericType::getRows() == 1)
        {
            //row vector
            std::wostringstream ostemp;
            int iLastVal = InternalType::m_iCols1PrintState;

            for (int i = InternalType::m_iCols1PrintState ; i < GenericType::getCols() ; i++)
            {
                int iWidth  = 0;
                int iLen    = 0;
                _piDims[0]  = 0;
                _piDims[1]  = i;
                int iPos    = ArrayOf<T>::getIndex(_piDims);

                getSignedIntFormat(ArrayOf<T>::get(iPos), &iWidth);
                iLen = iWidth + static_cast<int>(ostemp.str().size());
                if (iLen > iLineLen)
                {
                    //Max length, new line
                    iCurrentLine += 4; //"column x to Y" + empty line + value + empty line
                    if ((iMaxLines == 0 && iCurrentLine >= MAX_LINES) || (iMaxLines != 0 && iCurrentLine >= iMaxLines))
                    {
                        InternalType::m_iCols1PrintState = iLastVal;
                        return false;
                    }

                    ostr << std::endl << L"       column " << iLastVal + 1 << L" to " << i << std::endl << std::endl;
                    ostr << ostemp.str() << std::endl;
                    ostemp.str(L"");
                    iLastVal = i;
                }

                addSignedIntValue(&ostemp, ArrayOf<T>::get(iPos), iWidth);
            }

            if (iLastVal != 0)
            {
                ostr << std::endl << L"       column " << iLastVal + 1 << L" to " << GenericType::getCols() << std::endl << std::endl;
            }
            ostemp << std::endl;
            ostr << ostemp.str();
        }
        else // matrix
        {
            std::wostringstream ostemp;
            int iLen = 0;
            int iLastCol = InternalType::m_iCols1PrintState;

            //Array with the max printed size of each col
            int *piSize = new int[GenericType::getCols()];
            memset(piSize, 0x00, GenericType::getCols() * sizeof(int));

            //compute the row size for padding for each printed bloc.
            for (int iCols1 = InternalType::m_iCols1PrintState ; iCols1 < GenericType::getCols() ; iCols1++)
            {
                for (int iRows1 = 0 ; iRows1 < GenericType::getRows() ; iRows1++)
                {
                    int iWidth  = 0;
                    _piDims[0]  = iRows1;
                    _piDims[1]  = iCols1;
                    int iPos    = ArrayOf<T>::getIndex(_piDims);

                    getSignedIntFormat(ArrayOf<T>::get(iPos), &iWidth);
                    piSize[iCols1] = Max(piSize[iCols1], iWidth);
                }

                if (iLen + piSize[iCols1] > iLineLen)
                {
                    //find the limit, print this part
                    for (int iRows2 = InternalType::m_iRows2PrintState ; iRows2 < GenericType::getRows() ; iRows2++)
                    {
                        iCurrentLine++;
                        if ((iMaxLines == 0 && iCurrentLine >= MAX_LINES) ||
                                ( (iMaxLines != 0 && iCurrentLine + 3 >= iMaxLines && iRows2 == InternalType::m_iRows2PrintState) ||
                                  (iMaxLines != 0 && iCurrentLine + 1 >= iMaxLines && iRows2 != InternalType::m_iRows2PrintState)))
                        {
                            if (InternalType::m_iRows2PrintState == 0 && iRows2 != 0)
                            {
                                //add header
                                ostr << std::endl << L"       column " << iLastCol + 1 << L" to " << iCols1 << std::endl << std::endl;
                            }
                            ostr << ostemp.str();
                            InternalType::m_iRows2PrintState = iRows2;
                            InternalType::m_iCols1PrintState = iLastCol;
                            return false;
                        }

                        for (int iCols2 = iLastCol ; iCols2 < iCols1 ; iCols2++)
                        {
                            int iWidth  = 0;
                            _piDims[0]  = iRows2;
                            _piDims[1]  = iCols2;
                            int iPos    = ArrayOf<T>::getIndex(_piDims);

                            addSignedIntValue(&ostemp, ArrayOf<T>::get(iPos), piSize[iCols2]);
                        }
                        ostemp << std::endl;
                    }
                    iLen = 0;

                    iCurrentLine++;
                    if (InternalType::m_iRows2PrintState == 0)
                    {
                        iCurrentLine += 3;
                        ostr << std::endl << L"       column " << iLastCol + 1 << L" to " << iCols1 << std::endl << std::endl;
                    }

                    ostr << ostemp.str();
                    ostemp.str(L"");
                    iLastCol = iCols1;
                    InternalType::m_iRows2PrintState = 0;
                    InternalType::m_iCols1PrintState = 0;
                }

                iLen += piSize[iCols1] + SIGN_LENGTH + SIZE_BETWEEN_TWO_VALUES;
            }

            for (int iRows2 = InternalType::m_iRows2PrintState ; iRows2 < GenericType::getRows() ; iRows2++)
            {
                iCurrentLine++;
                if ((iMaxLines == 0 && iCurrentLine >= MAX_LINES) || (iMaxLines != 0 && iCurrentLine >= iMaxLines))
                {
                    if (InternalType::m_iRows2PrintState == 0 && iLastCol != 0)
                    {
                        //add header
                        ostr << std::endl << L"       column " << iLastCol + 1 << L" to " << GenericType::getCols() << std::endl << std::endl;
                    }

                    ostr << ostemp.str();
                    InternalType::m_iRows2PrintState = iRows2;
                    InternalType::m_iCols1PrintState = iLastCol;
                    return false;
                }

                for (int iCols2 = iLastCol ; iCols2 < GenericType::getCols() ; iCols2++)
                {
                    int iWidth  = 0;
                    _piDims[0]  = iRows2;
                    _piDims[1]  = iCols2;
                    int iPos    = ArrayOf<T>::getIndex(_piDims);

                    addSignedIntValue(&ostemp, ArrayOf<T>::get(iPos), piSize[iCols2]);
                }
                ostemp << std::endl;
            }

            if (InternalType::m_iRows2PrintState == 0 && iLastCol != 0)
            {
                ostr << std::endl << L"       column " << iLastCol + 1 << L" to " << GenericType::getCols() << std::endl << std::endl;
            }
            ostr << ostemp.str();
        }

        return true;
    }

    virtual T getNullValue()
    {
        return T(0);
    }

    virtual Int<T>* createEmpty(int _iDims, int* _piDims, bool /*_bComplex*/)
    {
        return new Int<T>(_iDims, _piDims);
    }

    virtual T copyValue(T _data)
    {
        return _data;
    }

    virtual void deleteAll()
    {
        delete[] ArrayOf<T>::m_pRealData;
        ArrayOf<T>::m_pRealData = NULL;
        deleteImg();
    }

    virtual void deleteImg() { }

    virtual T* allocData(int _iSize)
    {
        return new T[_iSize];
    }

    template<typename U, typename V> struct is_same_int
    {
        const static bool value = false;
    };
    template<typename U> struct is_same_int<U, U>
    {
        const static bool value = true;
    };

    bool isInt8()
    {
        return is_same_int<T, char>::value;
    }
    bool isInt16()
    {
        return is_same_int<T, short>::value;
    }
    bool isInt32()
    {
        return is_same_int<T, int>::value;
    }
    bool isInt64()
    {
        return is_same_int<T, long long>::value;
    }
    bool isUInt8()
    {
        return is_same_int<T, unsigned char>::value;
    }
    bool isUInt16()
    {
        return is_same_int<T, unsigned short>::value;
    }
    bool isUInt32()
    {
        return is_same_int<T, unsigned int>::value;
    }
    bool isUInt64()
    {
        return is_same_int<T, unsigned long long>::value;
    }
};

typedef Int<char> Int8;
typedef Int<short> Int16;
typedef Int<int> Int32;
typedef Int<long long> Int64;

typedef Int<unsigned char> UInt8;
typedef Int<unsigned short> UInt16;
typedef Int<unsigned int> UInt32;
typedef Int<unsigned long long> UInt64;

#ifdef _MSC_VER
template TYPES_IMPEXP class Int<char>;
template TYPES_IMPEXP class Int<unsigned char>;
template TYPES_IMPEXP class Int<short>;
template TYPES_IMPEXP class Int<unsigned short>;
template TYPES_IMPEXP class Int<int>;
template TYPES_IMPEXP class Int<unsigned int>;
template TYPES_IMPEXP class Int<long long>;
template TYPES_IMPEXP class Int<unsigned long long>;
#endif
}


#endif /* !__INT_HXX__ */