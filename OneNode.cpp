#include "OneNode.h"

COneNode::COneNode(void)
{
	m_nNodeIndexInWholeGraph = 0;
	m_nRegionFlag = 0;
	m_dPHPLowerBoundOld = 0.0;
	m_dPHPLowerBoundNew = 0.0;
	m_dPHPUpperBoundOld = 0.0;
	m_dPHPUpperBoundNew = 0.0;
	m_dTransProbOfSelfLoop = 0.0;
	m_dTransProbToDummyNode = 0.0;
}

COneNode::~COneNode(void)
{
}

void COneNode::CalculateTransProbOut(){
	// Get the weighted degree of this node.
	double dWeightedDegree = 0.0;
	int nNumOfAdjacentNodes = m_vdAdjacentNodesRawEdgeWeight.size();
	for (int nIdx = 0; nIdx < nNumOfAdjacentNodes; nIdx++)
	{
		dWeightedDegree += m_vdAdjacentNodesRawEdgeWeight[nIdx];
	}
	// Get the trans prob out.
	m_vdAdjacentNodesTransProbOut.resize(nNumOfAdjacentNodes);
	for (int nIdx = 0; nIdx < nNumOfAdjacentNodes; nIdx++)
	{
		m_vdAdjacentNodesTransProbOut[nIdx] = m_vdAdjacentNodesRawEdgeWeight[nIdx] / dWeightedDegree;
	}
}

double COneNode::GetTransProbToThisNode(int nIdxOfNode){
	// Find the index of this node (nIdxOfNode) in the adjacency list, and get the transition probability to this node.
	int nNumOfAdjNodes = m_vnAdjacentNodesIndex.size();
	bool bDoWeFoundThisNode = false;
	int  nIndexOfThisNodeInAdjList = 0;
	double dTransProbToThisNode = 0.0;
	for (int nIdx = 0; nIdx < nNumOfAdjNodes; nIdx++)
	{	// Find this node
		int nIndexOfOneAdjNode = m_vnAdjacentNodesIndex[nIdx];
		if (nIndexOfOneAdjNode == nIdxOfNode)
		{	// If it is this node.
			bDoWeFoundThisNode = true;
			nIndexOfThisNodeInAdjList = nIdx;
			break;
		}
	}
	if (bDoWeFoundThisNode == true)
	{	// Get the transition probability
		dTransProbToThisNode = m_vdAdjacentNodesTransProbOut[nIndexOfThisNodeInAdjList];
	}
	return dTransProbToThisNode;
}
