#include "OneGraph.h"

COneGraph::COneGraph(void)
{
	m_dDecayFactor = 0.5;
	m_dTerminationThreshold = 0.00001;
	m_nNumberKForTopK = 10;
	m_bStopFlag = false;
}

COneGraph::~COneGraph(void)
{
}

string COneGraph::ConvertIntToString(int nNumber, int nWidth)
{
	stringstream sNumber;//create a stringstream
	sNumber << std::setfill('0') << std::setw(nWidth) << nNumber;//add number to the stream
	return sNumber.str();//return a string with the contents of the stream
}

void COneGraph::ReadNodesFile(string sNodesFileName)
{	// Read the file containing the nodes
	ifstream aOneFileInputStrm;
	aOneFileInputStrm.open(sNodesFileName.c_str(), ifstream::in);
	string sOneLineContent;
	string sOneNodeName;
	while (std::getline(aOneFileInputStrm, sOneLineContent))
	{
		if (sOneLineContent.compare("") == 0){continue;} // if it is an empty line, continue;
		std::istringstream(sOneLineContent) >> sOneNodeName;
		COneNode aOneNode;
		aOneNode.SetNodeName(sOneNodeName);
		m_vaAllNodes.push_back(aOneNode);
	}
	aOneFileInputStrm.close();
}

void COneGraph::ReadEdgesFile(string sEdgesFileName)
{	// Read the file containing the edges
	ifstream aOneFileInputStrm;
	aOneFileInputStrm.open(sEdgesFileName.c_str(), ifstream::in);
	string sOneLineContent;
	int nNode1Index = 0;
	int nNode2Index = 0;
	double dEdgeWeight = 0.0;
	while (std::getline(aOneFileInputStrm, sOneLineContent))
	{
		if (sOneLineContent.compare("") == 0){continue;} // if it is an empty line, continue;
		std::istringstream(sOneLineContent) >> nNode1Index >> nNode2Index >> dEdgeWeight;
		COneEdge aOneEdge;
		aOneEdge.SetIndexOfNode1(nNode1Index);
		aOneEdge.SetIndexOfNode2(nNode2Index);
		if (dEdgeWeight < 0.000001){dEdgeWeight = 1.0;}; // if the file does not have edge weight, then we set it to 1.0.
		aOneEdge.SetEdgeWeight(dEdgeWeight);
		m_vaAllEdges.push_back(aOneEdge);
	}
	aOneFileInputStrm.close();
}

void COneGraph::ReadSeedNodesFile(string sFilenameSeedNodes)
{	// Read the file containing the seed nodes
	ifstream aOneFileInputStrm;
	aOneFileInputStrm.open(sFilenameSeedNodes.c_str(), ifstream::in);
	string sOneLineContent;
	string sOneSeedNodeName;
	int    nOneSeedNodeIndex = 0;
	while (std::getline(aOneFileInputStrm, sOneLineContent))
	{
		if (sOneLineContent.compare("") == 0){continue;} // if it is an empty line, continue;
		std::istringstream(sOneLineContent) >> sOneSeedNodeName;
		nOneSeedNodeIndex = FindNodeIndexByNodeName(sOneSeedNodeName);
		m_vnIndexOfSeedNodes.push_back(nOneSeedNodeIndex);
	}
	aOneFileInputStrm.close();
}

int COneGraph::FindNodeIndexByNodeName(string sOneNodeName)
{	// Find the index of one node (in vector m_vaAllNodes) with the name sOneNodeName.
	int nOneNodeIndex = -1;
	int nNumOfNodes = m_vaAllNodes.size();
	for (int nIdx = 0; nIdx < nNumOfNodes; nIdx++)
	{
		COneNode& aOneNodeRefer = m_vaAllNodes[nIdx];
		string sOneExistingNodeName = aOneNodeRefer.GetNodeName();
		if (sOneExistingNodeName.compare(sOneNodeName) == 0)
		{	// If we find the node, break.
			nOneNodeIndex = nIdx;
			break;
		}
	}
	return nOneNodeIndex;
};

void COneGraph::SetUpAdjacentNodesAndInducedEdgeForEachNode()
{	// Setup the adjacency list for each node.
	int nNumOfEdges = m_vaAllEdges.size();
	for (int nIdx = 0; nIdx < nNumOfEdges; nIdx++)
	{
		COneEdge& aOneEdgeRefer = m_vaAllEdges[nIdx];
		int nIndexOfNode1 = aOneEdgeRefer.GetIndexOfNode1();
		COneNode& aOneNode1Refer = m_vaAllNodes[nIndexOfNode1];
		int nIndexOfNode2 = aOneEdgeRefer.GetIndexOfNode2();
		COneNode& aOneNode2Refer = m_vaAllNodes[nIndexOfNode2];
		double dOneEdgeWeight = aOneEdgeRefer.GetEdgeWeight();
		aOneNode1Refer.AddOneAdjacentNodeIndex(nIndexOfNode2);
		aOneNode2Refer.AddOneAdjacentNodeIndex(nIndexOfNode1);
		aOneNode1Refer.AddOneAdjacentNodeRawEdgeWeight(dOneEdgeWeight);
		aOneNode2Refer.AddOneAdjacentNodeRawEdgeWeight(dOneEdgeWeight);
	}
};

void COneGraph::CalculateTransitionProbMatrix(){
	// Calculate the trans prob out.
	int nNumOfNodes = m_vaAllNodes.size();
	for (int nIdx = 0; nIdx < nNumOfNodes; nIdx++)
	{
		COneNode & aOneNodeRefer = m_vaAllNodes[nIdx];
		aOneNodeRefer.CalculateTransProbOut();
	}
	// Set the trans prob in.
	for (int nIdx = 0; nIdx < nNumOfNodes; nIdx++)
	{
		COneNode & aOneNodeRefer = m_vaAllNodes[nIdx];
		int nNumOfAdjNodes = aOneNodeRefer.GetNumberOfAdjacentNodes();
		for (int nIdx2 = 0; nIdx2 < nNumOfAdjNodes; nIdx2++)
		{
			int nOneAdjNodeIndex = aOneNodeRefer.GetOneAdjacentNodeIndex(nIdx2);
			COneNode & aOneAdjNodeRefer = m_vaAllNodes[nOneAdjNodeIndex];
			double dTransProbIn = aOneAdjNodeRefer.GetTransProbToThisNode(nIdx);
			aOneNodeRefer.AddOneAdjacentNodeTransProbIn(dTransProbIn);
		}
	}
}

void COneGraph::FLoS_PHP_ForEachSeedNode(){
	// For each seed node, do the top-k query.
	std::vector<std::vector<stNodeWithUpperLowerBounds>> vvstTopKIndexOfNodesForAllQueries;
	// For each seed node, query the top-K.
	int nNumOfSeedNodes = m_vnIndexOfSeedNodes.size();
	for (int nIdx = 0; nIdx < nNumOfSeedNodes; nIdx++)
	{
		// Pick one seed node
		int nIdxOfSeedNode = m_vnIndexOfSeedNodes[nIdx];
		std::vector<stNodeWithUpperLowerBounds> vstTopKIndexOfNodes;
		// Query its top-k nodes
		FLoS_PHP(nIdxOfSeedNode, vstTopKIndexOfNodes);
		// Store the query results.
		vvstTopKIndexOfNodesForAllQueries.push_back(vstTopKIndexOfNodes);
	}
	// Output the Top K nodes
	string sFileNameTopK = m_sOutputFolder + "01Top" + ConvertIntToString(m_nNumberKForTopK, 3) + "_FLoS_PHP.txt";
	ofstream aOutputFileStrm;
	aOutputFileStrm.open(sFileNameTopK, ofstream::out);
	for (int nIdx = 0; nIdx < nNumOfSeedNodes; nIdx++)
	{
		std::vector<stNodeWithUpperLowerBounds> & aOneQueryResult = vvstTopKIndexOfNodesForAllQueries[nIdx];
		for (int nIdx2 = 0; nIdx2 < m_nNumberKForTopK; nIdx2++)
		{	// Also include the query node. so, "m_nNumberKForTopK + 1".
			stNodeWithUpperLowerBounds & aOneTopKNodeRefer = aOneQueryResult[nIdx2];
			int	   nIdxOfNode		  = aOneTopKNodeRefer.m_nIndexOfNode;
			COneNode& aOneNodeRefer = m_vaAllNodes[nIdxOfNode];
			string    sOneNodeName  = aOneNodeRefer.GetNodeName();
			aOutputFileStrm << sOneNodeName << " ";
		}
		aOutputFileStrm << endl;
	}
	aOutputFileStrm.close();
}

void COneGraph::FLoS_PHP(int nIdxOfSeedNode, std::vector<stNodeWithUpperLowerBounds> & vstTopKIndexOfNodes)
{	// the FLoS_PHP algorithm.
	CSubGraph aLocalSearchSubGraph;
	aLocalSearchSubGraph.SetDecayFactor(m_dDecayFactor);
	aLocalSearchSubGraph.SetTerminationThreshold(m_dTerminationThreshold);
	aLocalSearchSubGraph.SetNumOfTopKNodes(m_nNumberKForTopK);
	aLocalSearchSubGraph.SetDummyNodeConstantPHPValue(1.0);
	// 1. Add seed node to "m_vstNodesSetS"
	COneNode& aSeedNodeInWholeGraph = m_vaAllNodes[nIdxOfSeedNode];
	COneNode aOneNode;
	aOneNode.SetNodeIndexInWholeGraph(nIdxOfSeedNode);
	aOneNode.SetRegionFlag(2); // 2: it is in \delta S, since there is only one node being visited.
	aOneNode.SetPHPLowerBoundOld(1.0);
	aOneNode.SetPHPUpperBoundOld(1.0);
	aOneNode.SetPHPLowerBoundNew(1.0);
	aOneNode.SetPHPUpperBoundNew(1.0);
	aOneNode.SetNodeName(aSeedNodeInWholeGraph.GetNodeName());
	aLocalSearchSubGraph.AddOneNodeAndUpdateAdjacentEdges(aOneNode, m_vaAllNodes);
	aLocalSearchSubGraph.SetIndexOfSeedNode(0);
	aLocalSearchSubGraph.SetMaxInDeltaS(0, 1.0); // set the query node as the maximum in the boundary (\delta S) of S.
	// Begin the iterations.
	int nIdxOfIterations = 0;
	m_bStopFlag = false;
	while (m_bStopFlag == false)
	{
		nIdxOfIterations++;
		// 1. otherwise, keep extract max and expand.
		aLocalSearchSubGraph.LocalExpansion(m_vaAllNodes);
		// 2 Update the lower bound \underline{h}.
		aLocalSearchSubGraph.UpdateLowerBound();
		// 3. Update the upper bound \overline{h}.
		aLocalSearchSubGraph.UpdateUpperBound();
		// 4. Check whether we can stop.
		m_bStopFlag = aLocalSearchSubGraph.CheckTerminationCriteria();
	}
	aLocalSearchSubGraph.GetTheTopKNodesIndexResults(vstTopKIndexOfNodes);
}
