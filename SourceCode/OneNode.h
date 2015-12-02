#pragma once
#include "MyHeader.h"

class COneNode
{
public:
	COneNode(void);
	~COneNode(void);
public:
	string m_sNodeName; // Gene name
	std::vector<int>    m_vnAdjacentNodesIndex; // the adjacent nodes indices list. The index in "vector<COneNode> m_vaAllNodes;" in COneGraph class.
	std::vector<double> m_vdAdjacentNodesRawEdgeWeight; // the raw edge weight on the edge linking this node and one adjacent node.
	std::vector<double> m_vdAdjacentNodesTransProbOut; // the transition probability to the node.
	std::vector<double> m_vdAdjacentNodesTransProbIn; // the transition probability to the node. These three vectors are synchronized.
	int    m_nNodeIndexInWholeGraph; // the node index in whole graph.
	int    m_nRegionFlag; // 1, S; 2, \delta S.
	double m_dPHPLowerBoundOld; // lower bound PHP of this node.
	double m_dPHPLowerBoundNew; // lower bound PHP of this node.
	double m_dPHPUpperBoundOld; // Upper bound PHP of this node.
	double m_dPHPUpperBoundNew; // Upper bound PHP of this node.
	double m_dTransProbOfSelfLoop; // the self loop probability for node in \delta S, i.e., m_nRegionFlag == 2.
	double m_dTransProbToDummyNode; // the transition probability to dummy node.
public:
	COneNode(const COneNode& aOneNode)
	{
		m_sNodeName = aOneNode.GetNodeName();
		m_nNodeIndexInWholeGraph = aOneNode.GetNodeIndexInWholeGraph();
		m_nRegionFlag = aOneNode.GetRegionFlag();
		m_dPHPLowerBoundOld = aOneNode.GetPHPLowerBoundOld();
		m_dPHPUpperBoundOld = aOneNode.GetPHPUpperBoundOld();
		m_dPHPLowerBoundNew = aOneNode.GetPHPLowerBoundNew();
		m_dPHPUpperBoundNew = aOneNode.GetPHPUpperBoundNew();
		m_dTransProbOfSelfLoop = aOneNode.GetTransProbOfSelfLoop();
		m_dTransProbToDummyNode = aOneNode.GetTransProbToDummyNode();
		int nNumOfAdjacentNodes = aOneNode.GetNumberOfAdjacentNodes();
		m_vnAdjacentNodesIndex.resize(nNumOfAdjacentNodes);
		for (int nIdx = 0; nIdx < nNumOfAdjacentNodes; nIdx++)
		{
			int nOneAdjacentNodeIndex = aOneNode.GetOneAdjacentNodeIndex(nIdx);
			m_vnAdjacentNodesIndex[nIdx] = nOneAdjacentNodeIndex;
		}
		int nNumOfAdjacentNodesRawEdgeWeight = aOneNode.GetNumberOfAdjacentNodesRawEdgeWeight();
		m_vdAdjacentNodesRawEdgeWeight.resize(nNumOfAdjacentNodesRawEdgeWeight);
		for (int nIdx = 0; nIdx < nNumOfAdjacentNodesRawEdgeWeight; nIdx++)
		{
			double dOneAdjacentNodeRawEdgeWeight = aOneNode.GetOneAdjacentNodeRawEdgeWeight(nIdx);
			m_vdAdjacentNodesRawEdgeWeight[nIdx] = dOneAdjacentNodeRawEdgeWeight;
		}
		int nNumOfAdjacentNodesTransProbOut = aOneNode.GetNumberOfAdjacentNodesTransProbOut();
		m_vdAdjacentNodesTransProbOut.resize(nNumOfAdjacentNodesTransProbOut);
		for (int nIdx = 0; nIdx < nNumOfAdjacentNodesTransProbOut; nIdx++)
		{
			double dOneAdjacentNodeTransProb = aOneNode.GetOneAdjacentNodeTransProbOut(nIdx);
			m_vdAdjacentNodesTransProbOut[nIdx] = dOneAdjacentNodeTransProb;
		}
		int nNumOfAdjacentNodesTransProbIn = aOneNode.GetNumberOfAdjacentNodesTransProbIn();
		m_vdAdjacentNodesTransProbIn.resize(nNumOfAdjacentNodesTransProbIn);
		for (int nIdx = 0; nIdx < nNumOfAdjacentNodesTransProbIn; nIdx++)
		{
			double dOneAdjacentNodeTransProb = aOneNode.GetOneAdjacentNodeTransProbIn(nIdx);
			m_vdAdjacentNodesTransProbIn[nIdx] = dOneAdjacentNodeTransProb;
		}
	};
	COneNode& operator = (const COneNode& aOneNode)
	{
		m_sNodeName = aOneNode.GetNodeName();
		m_nNodeIndexInWholeGraph = aOneNode.GetNodeIndexInWholeGraph();
		m_nRegionFlag = aOneNode.GetRegionFlag();
		m_dPHPLowerBoundOld = aOneNode.GetPHPLowerBoundOld();
		m_dPHPUpperBoundOld = aOneNode.GetPHPUpperBoundOld();
		m_dPHPLowerBoundNew = aOneNode.GetPHPLowerBoundNew();
		m_dPHPUpperBoundNew = aOneNode.GetPHPUpperBoundNew();
		int nNumOfAdjacentNodes = aOneNode.GetNumberOfAdjacentNodes();
		m_vnAdjacentNodesIndex.resize(nNumOfAdjacentNodes);
		for (int nIdx = 0; nIdx < nNumOfAdjacentNodes; nIdx++)
		{
			int nOneAdjacentNodeIndex = aOneNode.GetOneAdjacentNodeIndex(nIdx);
			m_vnAdjacentNodesIndex[nIdx] = nOneAdjacentNodeIndex;
		}
		int nNumOfAdjacentNodesTransProbOut = aOneNode.GetNumberOfAdjacentNodesTransProbOut();
		m_vdAdjacentNodesTransProbOut.resize(nNumOfAdjacentNodesTransProbOut);
		for (int nIdx = 0; nIdx < nNumOfAdjacentNodesTransProbOut; nIdx++)
		{
			double dOneAdjacentNodeTransProb = aOneNode.GetOneAdjacentNodeTransProbOut(nIdx);
			m_vdAdjacentNodesTransProbOut[nIdx] = dOneAdjacentNodeTransProb;
		}
		int nNumOfAdjacentNodesTransProbIn = aOneNode.GetNumberOfAdjacentNodesTransProbIn();
		m_vdAdjacentNodesTransProbIn.resize(nNumOfAdjacentNodesTransProbIn);
		for (int nIdx = 0; nIdx < nNumOfAdjacentNodesTransProbIn; nIdx++)
		{
			double dOneAdjacentNodeTransProb = aOneNode.GetOneAdjacentNodeTransProbIn(nIdx);
			m_vdAdjacentNodesTransProbIn[nIdx] = dOneAdjacentNodeTransProb;
		}
		return *this;
	};
public:
	inline string GetNodeName() const {return m_sNodeName;};
	inline void   SetNodeName(string sNodeName){m_sNodeName = sNodeName;};

	inline int  GetNodeIndexInWholeGraph() const {return m_nNodeIndexInWholeGraph;};
	inline void SetNodeIndexInWholeGraph(int nNodeIndexInWholeGraph) {m_nNodeIndexInWholeGraph = nNodeIndexInWholeGraph;};

	inline int  GetRegionFlag() const {return m_nRegionFlag;};
	inline void SetRegionFlag(int nRegionFlag) {m_nRegionFlag = nRegionFlag;};

	inline double GetPHPLowerBoundOld() const {return m_dPHPLowerBoundOld;};
	inline void   SetPHPLowerBoundOld(double dPHPLowerBound) {m_dPHPLowerBoundOld = dPHPLowerBound;};
	inline double GetPHPUpperBoundOld() const {return m_dPHPUpperBoundOld;};
	inline void   SetPHPUpperBoundOld(double dPHPUpperBound) {m_dPHPUpperBoundOld = dPHPUpperBound;};

	inline double GetPHPLowerBoundNew() const {return m_dPHPLowerBoundNew;};
	inline void   SetPHPLowerBoundNew(double dPHPLowerBound) {m_dPHPLowerBoundNew = dPHPLowerBound;};
	inline double GetPHPUpperBoundNew() const {return m_dPHPUpperBoundNew;};
	inline void   SetPHPUpperBoundNew(double dPHPUpperBound) {m_dPHPUpperBoundNew = dPHPUpperBound;};

	inline double GetTransProbOfSelfLoop() const {return m_dTransProbOfSelfLoop;};
	inline void   SetTransProbOfSelfLoop(double dTransProbOfSelfLoop) {m_dTransProbOfSelfLoop = dTransProbOfSelfLoop;};

	inline double GetTransProbToDummyNode() const {return m_dTransProbToDummyNode;};
	inline void   SetTransProbToDummyNode(double dTransProbToDummyNode) {m_dTransProbToDummyNode = dTransProbToDummyNode;};

	inline int  GetNumberOfAdjacentNodes() const {return m_vnAdjacentNodesIndex.size();};
	inline int  GetOneAdjacentNodeIndex(int nIdxOfAdjacentNodeIndex) const {return m_vnAdjacentNodesIndex[nIdxOfAdjacentNodeIndex];};
	inline void AddOneAdjacentNodeIndex(int nOneAdjacentNodeIndex){m_vnAdjacentNodesIndex.push_back(nOneAdjacentNodeIndex);};

	inline int  GetNumberOfAdjacentNodesRawEdgeWeight() const {return m_vdAdjacentNodesRawEdgeWeight.size();};
	inline double GetOneAdjacentNodeRawEdgeWeight(int  nAdjacentNodeIndex) const {return m_vdAdjacentNodesRawEdgeWeight[nAdjacentNodeIndex];};
	inline void AddOneAdjacentNodeRawEdgeWeight(double dAdjacentNodeRawEdgeWeight){m_vdAdjacentNodesRawEdgeWeight.push_back(dAdjacentNodeRawEdgeWeight);};

	inline int  GetNumberOfAdjacentNodesTransProbOut() const {return m_vdAdjacentNodesTransProbOut.size();};
	inline double GetOneAdjacentNodeTransProbOut(int  nAdjacentNodeIndex) const {return m_vdAdjacentNodesTransProbOut[nAdjacentNodeIndex];};
	inline void AddOneAdjacentNodeTransProbOut(double dAdjacentNodeTransProb){m_vdAdjacentNodesTransProbOut.push_back(dAdjacentNodeTransProb);};

	inline int  GetNumberOfAdjacentNodesTransProbIn() const {return m_vdAdjacentNodesTransProbIn.size();};
	inline double GetOneAdjacentNodeTransProbIn(int  nAdjacentNodeIndex) const {return m_vdAdjacentNodesTransProbIn[nAdjacentNodeIndex];};
	inline void AddOneAdjacentNodeTransProbIn(double dAdjacentNodeTransProb){m_vdAdjacentNodesTransProbIn.push_back(dAdjacentNodeTransProb);};
public:
	void   CalculateTransProbOut();
	double GetTransProbToThisNode(int nIdxOfNode);
};
