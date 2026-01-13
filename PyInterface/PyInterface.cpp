#include "stdafx.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/embed.h>


#include "PyInterface.h"
#include "YILAlgorithm/yilWarning.h"
#include "XCPCalculateTool/CalBsUtils.h"

//// 将 UTF-8 string 转换为 wstring 用于显示
//std::wstring utf8_to_wstring(const std::string& utf8_str) {
//    if (utf8_str.empty()) return L"";
//
//    int wide_size = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, nullptr, 0);
//    if (wide_size == 0) return L"";
//
//    std::wstring wide_str(wide_size, 0);
//    MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wide_str[0], wide_size);
//
//    // 移除末尾的 null 字符
//    if (!wide_str.empty() && wide_str.back() == L'\0') {
//        wide_str.pop_back();
//    }
//
//    return wide_str;
//}
//
////wstring=>string
//std::string wstring_to_string(const std::wstring& ws) {
//    std::string strLocale = setlocale(LC_ALL, "");
//    const wchar_t* wchSrc = ws.c_str();
//    size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
//    char* chDest = new char[nDestSize];
//    memset(chDest, 0, nDestSize);
//    wcstombs(chDest, wchSrc, nDestSize);
//    std::string strResult = chDest;
//    delete[]chDest;
//    setlocale(LC_ALL, strLocale.c_str());
//    return strResult;
//}

//std::string utf8_to_wide(const std::string& utf8_str)
//{
//    return wstring_to_string(utf8_to_wstring(utf8_str));
//}

// 直接运行OSIS命令流
std::pair<bool, std::string> PyInterface::OSIS_Run(std::string strCmd, std::string mode)
{
    std::string errorCode = "";

    if (strCmd != "")
    {
        if (mode == "execute" || mode == "exec")
        {
            m_vCommand.Add(&(yilCString)strCmd);
            // 这里需要split一下
            CalBsUtils::ExecuteCommands(m_vCommand);
            m_vCommand.Clear();
        }
        else if (mode == "stash")
        {
            m_vCommand.Add(&(yilCString)strCmd);
        }
        else
        {

        }
    }
    else
    {
        if (mode == "execute" || mode == "exec")
        {
            CalBsUtils::ExecuteCommands(m_vCommand);
            m_vCommand.Clear();
        }
    }

    auto warning = g_pWarningList->GetLastWaringContent();
    errorCode = warning.c_str();
    return { true, errorCode };
}

// 下面的函数将返回<bool, string>
// 添加或修改导出函数后，请在PyInterface.pyi中也将函数的声明加上。作用为便于用户在IDE中得到osis函数的高亮信息
PYBIND11_MODULE(PyInterface, m)
{
	m.doc() = "OSIS python interface";

    py::class_<PyInterface>(m, "PyInterface")
        .def_static("GetInstance", &PyInterface::GetInstance,
            py::return_value_policy::reference)
        .def("OSIS_Run", &PyInterface::OSIS_Run,
            py::arg("strCmd"), py::arg("mode"))
        /*.def("OSIS_Replot", &PyInterface::OSIS_Replot)
        .def("OSIS_Clear", &PyInterface::OSIS_Clear)
        .def("OSIS_Solve", &PyInterface::OSIS_Solve)

        .def("OSIS_Acel", &PyInterface::OSIS_Acel,
            py::arg("dG"))
        .def("OSIS_CalcTendon", &PyInterface::OSIS_CalcTendon,
            py::arg("bFlag"))
        .def("OSIS_CalcConForce", &PyInterface::OSIS_CalcConForce,
            py::arg("bFlag"))
        .def("OSIS_CalcShrink", &PyInterface::OSIS_CalcShrink,
            py::arg("bFlag"))
        .def("OSIS_CalcCreep", &PyInterface::OSIS_CalcCreep,
            py::arg("bFlag"))
        .def("OSIS_CalcShear", &PyInterface::OSIS_CalcShear,
            py::arg("bFlag"))
        .def("OSIS_CalcRlx", &PyInterface::OSIS_CalcRlx,
            py::arg("bFlag"))
        .def("OSIS_ModLocCoor", &PyInterface::OSIS_ModLocCoor,
            py::arg("bFlag"))
        .def("OSIS_IncTendon", &PyInterface::OSIS_IncTendon,
            py::arg("bFlag"))
        .def("OSIS_NL", &PyInterface::OSIS_NL,
            py::arg("bGeom"), py::arg("bLink"))
        .def("OSIS_LnSrch", &PyInterface::OSIS_LnSrch,
            py::arg("bFlag"))
        .def("OSIS_AutoTs", &PyInterface::OSIS_AutoTs,
            py::arg("bFlag"))
        .def("OSIS_ModOpt", &PyInterface::OSIS_ModOpt,
            py::arg("nNO"))

        .def("OSIS_CrpShrk", &PyInterface::OSIS_CrpShrk,
            py::arg("nNO"), py::arg("strName"), py::arg("dAvgHumidity"), py::arg("nBirthTime"), py::arg("dTypeCoeff"), py::arg("nBirthByShrinking"))
        .def("OSIS_Material", &PyInterface::OSIS_Material,
            py::arg("nMat"), py::arg("strName"), py::arg("eMaterial"), py::arg("eCode"), py::arg("eGrade"), py::arg("nCrepShrk"), py::arg("dDmp"))
        .def("OSIS_Material", &PyInterface::OSIS_MaterialDel,
            py::arg("nMat"))
        .def("OSIS_Material", &PyInterface::OSIS_MaterialMod,
            py::arg("nOld"), py::arg("nNew"))

        .def("OSIS_Section", &PyInterface::OSIS_Section,
            py::arg("nSec"), py::arg("strName"), py::arg("eSectionType"), py::arg("kwargs"))
        .def("OSIS_SectionOffset", &PyInterface::OSIS_SectionOffset,
            py::arg("nSec"), py::arg("offsetTypeY"), py::arg("offsetValueY"), py::arg("offsetTypeZ"), py::arg("offsetValueZ"))
        .def("OSIS_SectionMesh", &PyInterface::OSIS_SectionMesh,
            py::arg("nSec"), py::arg("bMeshMethod"), py::arg("dMeshSize"))
        .def("OSIS_SectionDel", &PyInterface::OSIS_SectionDel, 
            py::arg("nSec"))
        .def("OSIS_SectionMod", &PyInterface::OSIS_SectionMod,
            py::arg("nOld"), py::arg("nNew"))

        .def("OSIS_Node", &PyInterface::OSIS_Node,
            py::arg("nNO"), py::arg("x"), py::arg("y"), py::arg("z"))
        .def("OSIS_NodeDel", &PyInterface::OSIS_NodeDel,
            py::arg("nNO"))
        .def("OSIS_NodeMod", &PyInterface::OSIS_NodeMod,
            py::arg("nOld"), py::arg("nNew"))

        .def("OSIS_Element", &PyInterface::OSIS_Element,
            py::arg("nEle"), py::arg("eElementType"), py::arg("kwargs"))
        .def("OSIS_ElementDel", &PyInterface::OSIS_ElementDel,
            py::arg("nEle"))
        .def("OSIS_ElementMod", &PyInterface::OSIS_ElementMod,
            py::arg("nOld"), py::arg("nNew"))
        
        .def("OSIS_Boundary", &PyInterface::OSIS_Boundary,
            py::arg("nBd"), py::arg("eBoundaryType"), py::arg("kwargs"))
        .def("OSIS_AsgnBd", &PyInterface::OSIS_AsgnBd,
            py::arg("nBd"), py::arg("eOP"), py::arg("nodeNOs"))

        .def("OSIS_LoadCase", &PyInterface::OSIS_LoadCase,
            py::arg("strName"), py::arg("eLoadCaseType"), py::arg("dScalar"), py::arg("strPrompt"))
        .def("OSIS_Load", &PyInterface::OSIS_Load,
            py::arg("eLoadType"), py::arg("strLCName"), py::arg("kwargs"))

        .def("OSIS_LiveGrade", &PyInterface::OSIS_LiveGrade,
            py::arg("strName"), py::arg("eCode"), py::arg("eLiveLoadType"), py::arg("kwargs"))*/

        .def("OSIS_ElemForce", &PyInterface::OSIS_ElemForce,
            py::arg("strLCName"), py::arg("eDataItem"), py::arg("eElementType"))

        .def("OSIS_QBOverall", &PyInterface::OSIS_QBOverall,
            py::arg("eBridgeType"), py::arg("spans"), py::arg("bIsElasticConnection"), py::arg("dKxOfAbutment1"), py::arg("dKyOfAbutment1"), py::arg("dKzOfAbutment1"),
            py::arg("dKxOfAbutment2"), py::arg("dKyOfAbutment2"), py::arg("dKzOfAbutment2"), py::arg("dElasticLength"))
        .def("OSIS_QBPortrait", &PyInterface::OSIS_QBPortrait,
            py::arg("eBridgeType"), py::arg("dEleLengthMin"), py::arg("dEleLengthMax"), py::arg("S1"), py::arg("L1"), py::arg("F1"), py::arg("Tb"), py::arg("Tw"), py::arg("D1"))
        .def("OSIS_QBLoad", &PyInterface::OSIS_QBLoad,
            py::arg("eBridgeType"), 
            py::arg("bHaveDeadLoad"), py::arg("bHavePavement"), py::arg("bHaveRail"), py::arg("bHaveSidewalk"), py::arg("bHaveSideBeam"), py::arg("bHaveMiddleBeam"),
            py::arg("bHaveMovingLoad"), py::arg("bHaveTemperEff"), py::arg("bHaveTemperGradient"), py::arg("bHaveSupSettle"),
            py::arg("dDeadLoadFactor"), py::arg("dPavementIntensity"), py::arg("dRailIntensity"), py::arg("dSidewalkIntensity"), py::arg("dCrowdLoad"), 
            py::arg("dSideBeamPointLoad"), py::arg("dMiddleBeamPointLoad"),
            py::arg("dTransVehDistribution"), py::arg("bIsSelfDefine"), py::arg("dFundFreq"), py::arg("dWarming"), py::arg("dCooling"), 
            py::arg("T1"), py::arg("T2"), py::arg("dSupSettle"))
        .def("OSIS_QBTendon", &PyInterface::OSIS_QBTendon,
            py::arg("eBridgeType"), py::arg("tendonInfo"))
        .def("OSIS_QBStage", &PyInterface::OSIS_QBStage,
            py::arg("eBridgeType"), py::arg("stageInfo"))
        ;

	//m.def("add", &, "A function that adds two numbers");
}