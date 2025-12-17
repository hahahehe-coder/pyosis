#include "stdafx.h"
#include "PyInterface.h"

bool AppendNode(int nNO, vector<double>& vktValue, std::string& errorCode)
{
	auto pNM = GET_PREP_NODE();

	PREP::NodeInfo* pNodeInfo = new PREP::NodeInfo();
	if (!pNodeInfo) 
	{
		return false;
	}
	pNodeInfo->setNO(nNO);
	pNodeInfo->x = vktValue[0];
	pNodeInfo->y = vktValue[1];
	pNodeInfo->z = vktValue[2];

	if (!pNM->append(pNodeInfo)) {

		/*m_ErrCode.Format(_T("小于节点合并精度: %lf m; 已忽略对%d号节点的操作！"), pNodeInfo->getPrecision(), _index);
		g_pWarningList->AddToTemperary(4, m_ErrCode, 1);*/
		errorCode = "小于节点合并精度: " + std::to_string(pNodeInfo->getPrecision()) + " m; 已忽略对" + std::to_string(nNO) + "号节点的操作！";
		delete pNodeInfo; pNodeInfo = nullptr;
		return false;
	}

	////压入影子命令
	//PUSH_SHADOW_CMD_CUSTOM(THIS_IS_DEF, "NodeDel,%d", _index);

	return true;
}

bool ResetNode(int nNO, vector<double>& vktValue)
{
	PREP::NodeInfo* pNodeInfo = GET_PREP_NODE()->getInfo(nNO);
	if (!pNodeInfo) return false;

	std::size_t tHashVal = 0;
	if (!pNodeInfo->check(nNO, tHashVal, vktValue[0], vktValue[1], vktValue[2])) return false;

	////压入影子命令
	////修改前压入，如果失败再弹出（经过数检后的覆盖性操作几乎不会失败）
	//PUSH_SHADOW_CMD_DEFAULT(THIS_IS_MOD, pNodeInfo);

	pNodeInfo->x = vktValue[0];
	pNodeInfo->y = vktValue[1];
	pNodeInfo->z = vktValue[2];
	pNodeInfo->setHashValue(tHashVal);

	return true;
}

/// <summary>
/// 定义节点
/// </summary>
/// <param name="nNO">节点编号，为-1代表缺省</param>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
/// <returns>是否成功，失败原因</returns>
std::pair<bool, std::string> PyInterface::OSIS_Node(int nNO, double x, double y, double z)
{
	std::string errorCode;
	if (nNO == -1)
	{
		nNO = GET_PREP_NODE()->getMaxNO() + 1;
	}
	else if (nNO > OSIS_MAX_INT_INDEX)
	{
		//m_ErrCode.Format(_T("节点索引号不应超过1,000,000"));
		//g_pWarningList->AddToTemperary(4, m_ErrCode);
		errorCode = "节点索引号不应超过" + std::to_string(OSIS_MAX_INT_INDEX) + "！";
		return { false, errorCode };
	}

	vector<double> vktValue{x, y, z};
	yilUnitScale* pUnit = GetProject()->GetUnit();
	vktValue[0] = pUnit->LocalLengthToSI(vktValue[0]);
	vktValue[1] = pUnit->LocalLengthToSI(vktValue[1]);
	vktValue[2] = pUnit->LocalLengthToSI(vktValue[2]);

	if (!GET_PREP_NODE()->getInfo(nNO))
	{

		if (!AppendNode(nNO, vktValue, errorCode))
		{
			GetProject()->GetNodeVec()->DeleteByNO(nNO);
			//m_ErrCode.Format(_T("节点创建失败！"), (LPCSTR)_Command);
			errorCode += "节点创建失败！";
			return { false, errorCode };
		}
	}
	else 
	{
		if (!ResetNode(nNO, vktValue)) 
		{
			return { false, errorCode };
		}

		GetProject()->GetPlotControl()->ElementDataChangedOn();
		GetProject()->GetPlotControl()->ElemPropBeam3DsDataChangedOn();
		GetProject()->GetPlotControl()->ElemPropTrussDataChangedOn();
		GetProject()->GetPlotControl()->ElemPropMlelDataChangedOn();
	}

	GetProject()->GetPlotControl()->InputChangedOn();
	GetProject()->GetPlotControl()->StructTreeChangedOn();
	GetProject()->GetPlotControl()->NodeDataChangedOn();

	return { true, errorCode };
	//return { true, "成功创建节点，编号:" + std::to_string(nNO) +  "！" };
}

/// <summary>
/// 删除节点
/// </summary>
/// <param name="nNO">节点编号</param>
/// <returns></returns>
std::pair<bool, std::string> PyInterface::OSIS_NodeDel(const int nNO)
{
	std::string errorCode;
	//ASSERT_FEM_STATUS();

	PREP::NodeInfo* pNF = GET_PREP_NODE()->getInfo(nNO);
	if (!pNF) 
	{
		return { true, errorCode };
	}

	//ForwardCommand(pNF); //命令流转发


	//压入影子命令
	//提前压入，如果失败再弹出
	//PUSH_SHADOW_CMD_DEFAULT(THIS_IS_DEL, pNF);

	if (!GET_PREP_NODE()->deleteByNO(nNO)) {

		errorCode = "删除节点" + std::to_string(nNO) + "失败";
		return { false, errorCode };
	}

	GetProject()->GetPlotControl()->InputChangedOn();
	GetProject()->GetPlotControl()->StructTreeChangedOn();
	GetProject()->GetPlotControl()->NodeDataChangedOn();

	return { true, errorCode };
}

/// <summary>
/// 修改节点编号，节点编号存在时，交换
/// </summary>
/// <param name="nOld">旧编号</param>
/// <param name="nNew">新编号</param>
/// <returns></returns>
std::pair<bool, std::string> PyInterface::OSIS_NodeMod(const int nOld, const int nNew)
{
	std::string errorCode;
	//ASSERT_FEM_STATUS();

	auto* pInfo = GET_PREP_NODE()->getInfo(nOld);
	if (!pInfo)
	{
		errorCode = "未找到编号为" + std::to_string(nOld) + "的节点！";
		return { false, errorCode };
	}

	auto swapIndex = [this](int nOld, int nNew) {

		auto* pInfo = GET_PREP_NODE()->getInfo(nOld);
		if (!pInfo)
			return false;

		GET_PREP_NODE()->modifyIndex(nOld, nNew); //虽然这句话应该在最后执行，但是限制于下面关联参数的数据检查，先放这里


		auto* pEM = GET_PREP_ELEMENT();
		const set<int>& setRelatedElement = pInfo->getRelatedElement();
		if (!setRelatedElement.empty()) {

			for (const int it : setRelatedElement) {
				auto* pRelatedInfo = pEM->getInfo(it);
				if (!pRelatedInfo) continue;
				pRelatedInfo->substitueNode(nOld, nNew);
			}

			GetProject()->GetPlotControl()->ElementDataChangedOn();
			GetProject()->GetPlotControl()->ElemPropBeam3DsDataChangedOn();
		}
		pEM = nullptr;



		auto* pBM = GET_PREP_BD();
		const set<int>& setRelatedBoundary = pInfo->getRelatedBoundary();
		if (!setRelatedBoundary.empty()) {

			for (const int it : setRelatedBoundary) {
				auto* pRelatedInfo = pBM->getInfo(it);
				if (!pRelatedInfo) continue;

				if (PrepEnum::Boundary::MstSlv == pRelatedInfo->getType()) {
					auto* pCastPtr = dynamic_cast<PREP::MstSlvBoundaryInfo*>(pRelatedInfo);
					if (!pCastPtr) continue;

					if (pCastPtr->getMasterNO() == nOld) {
						pCastPtr->substituteMasterNO(nOld, nNew);
					}
					else {
						pRelatedInfo->substituteEntityIndex(vector<int>{nOld}, vector<int>{nNew});

					}
				}
				else {
					pRelatedInfo->substituteEntityIndex(vector<int>{nOld}, vector<int>{nNew});
				}
			}

			GetProject()->GetPlotControl()->BoundaryChangedOn();
		}
		pBM = nullptr;

		auto* pLCM = GET_PREP_LC();
		const auto& setRelatedLoad = pInfo->getRelatedLoad();
		if (!setRelatedLoad.empty()) {

			for (const auto& it : setRelatedLoad) {
				auto* pRelatedInfo = pLCM->getInfo(it.first);
				if (!pRelatedInfo) continue;
				pRelatedInfo->substitueNode(nOld, nNew);
			}

			GetProject()->GetPlotControl()->LoadGroupChangedOn();
			GetProject()->GetPlotControl()->LoadGroupTreeChangedOn();
		}
		pLCM = nullptr;

		auto* pAM = GET_PREP_ANA();
		const set<string>& setRelatedSetlGrp = pInfo->getRelatedSetlGrp();
		if (!setRelatedSetlGrp.empty()) {

			for (const string& it : setRelatedSetlGrp) {
				auto* pRelatedInfo = pAM->getInfo<PREP::SetlGrpInfo>(it);
				if (!pRelatedInfo) continue;
				pRelatedInfo->substitueSetlNode(nOld, nNew);
			}

			GetProject()->GetPlotControl()->LoadGroupChangedOn();
			GetProject()->GetPlotControl()->LoadGroupTreeChangedOn();
		}
		pAM = nullptr;

		return true;
	};

	if (GET_PREP_NODE()->getInfo(nNew)) {
		/*m_ErrCode.Format(_T("节点[%d]已存在"), nNew);
		g_pWarningList->AddToTemperary(4, m_ErrCode);
		return false;*/

		const int nTemp = OSIS_MAX_INT_INDEX * 2;
		swapIndex(nNew, nTemp);
		swapIndex(nOld, nNew);
		swapIndex(nTemp, nOld);
	}
	else {
		swapIndex(nOld, nNew);
	}

	//压入影子命令
	//PUSH_SHADOW_CMD_CUSTOM(THIS_IS_MOD, "NodeMod,%d,%d", nNew, nOld);

	GetProject()->GetPlotControl()->InputChangedOn();
	GetProject()->GetPlotControl()->StructTreeChangedOn();
	GetProject()->GetPlotControl()->NodeDataChangedOn();
	SEL_MGR_PTR()->NodeModCmd(nOld, nNew);

	return { true, errorCode };
}
