#pragma once
#include "MyHeader.h"
#include "OneNode.h"

struct stNodeWithUpperLowerBounds{
	int m_nIndexOfNode; // index in "m_vaAllNodes".
	double m_dUpperBound; // the upper bound.
	double m_dLowerBound; // the lower bound.
};

class CSubGraph
{
public:
	CSubGraph(void);
	~CSubGraph(void);
public:
	std::vector<COneNode> m_vaNodesInSubGraph;
	int    m_nIndexOfSeedNode;
	int    m_nNumOfTopKNodes;
 	double m_dDecayFactor;
	double m_dTerminationThreshold;
public:
	double m_dDummyNodeConstantPHPValue; // = dMaxUpperBoundValueInDeltaS;
	double m_dMaxUpperBoundOfNodeInDeltaS; // the maximum upper bound value of the nodes in \delta S
	double m_dMaxAvgOfNodeInDeltaS; // the maximum average LB and UB of the nodes in \delta S. the index of that node is m_nIndexOfNodeWithMaxAvgInDeltaS
	int    m_nIndexOfNodeWithMaxAvgInDeltaS; // the index of node with value m_dMaxAvgOfNodeInDeltaS
	std::vector<stNodeWithUpperLowerBounds> m_vstTopKIndexOfNodes; // the top-K node indices are stored in this variable.
public:
	inline void SetNumOfTopKNodes(int nNumOfTopKNodes) {m_nNumOfTopKNodes = nNumOfTopKNodes;};
	inline void SetIndexOfSeedNode(int nIndexOfSeedNode) {m_nIndexOfSeedNode = nIndexOfSeedNode;};
	inline void SetDecayFactor(double dDecayFactor) {m_dDecayFactor = dDecayFactor;};
	inline void SetTerminationThreshold(double dTerminationThreshold){m_dTerminationThreshold = dTerminationThreshold;};
	inline void SetDummyNodeConstantPHPValue(double dDummyNodeConstantPHPValue) {m_dDummyNodeConstantPHPValue = dDummyNodeConstantPHPValue;};
	inline void SetMaxInDeltaS(int nIndexOfNodeWithMaxAvgInDeltaS, double dMaxAvgOfNodeInDeltaS){m_nIndexOfNodeWithMaxAvgInDeltaS = nIndexOfNodeWithMaxAvgInDeltaS; m_dMaxAvgOfNodeInDeltaS = dMaxAvgOfNodeInDeltaS;};
public:
	void   LocalExpansion(std::vector<COneNode> & vaAllNodes);
	void   AddOneNodeAndUpdateAdjacentEdges(COneNode aOneNode, std::vector<COneNode> & vaAllNodes);
	bool   FindWhetherThisNodeExistInSubGraph(int nIndexOfOneNodeInWholeGraph);
public:
	void   UpdateLowerBound();
	double DoOneIterationToUpdateLowerBound();
	void   UpdateUpperBound();
	double DoOneIterationToUpdateUpperBound();
public:
	bool   CheckTerminationCriteria();
	void   GetTheTopKNodesIndexResults(std::vector<stNodeWithUpperLowerBounds> & vstTopKIndexOfNodes);
};
