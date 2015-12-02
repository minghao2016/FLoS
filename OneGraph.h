#pragma once
#include "MyHeader.h"
#include "OneNode.h"
#include "OneEdge.h"
#include "SubGraph.h"

class COneGraph
{
public:
	COneGraph(void);
	~COneGraph(void);
public:
	std::vector<COneNode> m_vaAllNodes; // the list of all the nodes in the graph.
	std::vector<COneEdge> m_vaAllEdges; // the list of all the edges in the graph.
	std::vector<int>      m_vnIndexOfSeedNodes; // the list of the seed nodes. Each element is the index in "m_vaAllNodes".
	double  m_dDecayFactor; // the decay factor parameter in PHP
	double  m_dTerminationThreshold; // the termination threshold parameter used.
	int		m_nNumberKForTopK; // the number K for top-k query.
	string  m_sOutputFolder; // the output folder.
	bool    m_bStopFlag; // the stop flag in the FLoS_PHP algorithm
public:
	string ConvertIntToString(int nNumber, int nWidth);
	inline void SetDecayFactor(double dDecayFactor){m_dDecayFactor = dDecayFactor;};
	inline void SetTerminationThreshold(double dTerminationThreshold){m_dTerminationThreshold = dTerminationThreshold;};
	inline void SetNumberKForTopKQuery(int nNumOfTopK){m_nNumberKForTopK = nNumOfTopK;};
	inline void SetOutputFolder(string sOutputFolder){m_sOutputFolder = sOutputFolder;};
public:
	void ReadNodesFile(string sNodesFileName);
	void ReadEdgesFile(string sEdgesFileName);
	void ReadSeedNodesFile(string sFilenameSeedNodes);
	int  FindNodeIndexByNodeName(string sOneNodeName);
public:
	void SetUpAdjacentNodesAndInducedEdgeForEachNode();
	void CalculateTransitionProbMatrix();
public:
	void FLoS_PHP_ForEachSeedNode(); // For each seed node, select top K nodes.
	void FLoS_PHP(int nIdxOfSeedNode, std::vector<stNodeWithUpperLowerBounds> & vstTopKIndexOfNodes); // the kernel algorithm.
};
