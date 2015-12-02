#include "SubGraph.h"

CSubGraph::CSubGraph(void)
{
	m_nIndexOfSeedNode = 0;
	m_nNumOfTopKNodes = 10;
	m_dDecayFactor = 0.5;
	m_dTerminationThreshold = 0.00001;
	m_dDummyNodeConstantPHPValue = 1.0;
	m_dMaxUpperBoundOfNodeInDeltaS = 0.0;
	m_dMaxAvgOfNodeInDeltaS = 0.0;
	m_nIndexOfNodeWithMaxAvgInDeltaS = 0;
}

CSubGraph::~CSubGraph(void)
{
}

void CSubGraph::LocalExpansion(std::vector<COneNode> & vaAllNodes){
	// Move the node in \delta S with max upper bound PHP, to S.
	COneNode & aOneNodeReferMaxInBoundary = m_vaNodesInSubGraph[m_nIndexOfNodeWithMaxAvgInDeltaS]; // m_nIndexOfNodeWithMaxAvgInDeltaS has been setup.
	aOneNodeReferMaxInBoundary.SetRegionFlag(1);
	int nIndexInWholeGraph = aOneNodeReferMaxInBoundary.GetNodeIndexInWholeGraph();
	COneNode & aOneNodeReferInWholeGraph = vaAllNodes[nIndexInWholeGraph];
	// Add its neighbor nodes to this \delta S.
	int nNumOfAdjNodes = aOneNodeReferInWholeGraph.GetNumberOfAdjacentNodes();
	for (int nIdx = 0; nIdx < nNumOfAdjNodes; nIdx++){
		int nOneAdjNodeIndex = aOneNodeReferInWholeGraph.GetOneAdjacentNodeIndex(nIdx);
		bool bFoundThisNodeInSubGraph = FindWhetherThisNodeExistInSubGraph(nOneAdjNodeIndex);
		if (bFoundThisNodeInSubGraph == false)
		{	// If we can't find this node in S and \delta S, then add this node to \delta S.
			COneNode aOneNode; // aOneNode is a node in the local subgraph
			COneNode & aOneNodeReferNeighbor = vaAllNodes[nOneAdjNodeIndex];
			aOneNode.SetNodeIndexInWholeGraph(nOneAdjNodeIndex);
			aOneNode.SetRegionFlag(2); // I forgot this line, and cause a bug.
			aOneNode.SetPHPLowerBoundOld(0.0);
			aOneNode.SetPHPUpperBoundOld(1.0);
			aOneNode.SetPHPLowerBoundNew(0.0);
			aOneNode.SetPHPUpperBoundNew(1.0);
			aOneNode.SetNodeName(aOneNodeReferNeighbor.GetNodeName());
			AddOneNodeAndUpdateAdjacentEdges(aOneNode, vaAllNodes);
		}
	}
	// After Add this node, we need to check all the nodes again, to update the region flag: "1", S; "2", \delta S.
	int nNumOfExistNodes = m_vaNodesInSubGraph.size();
	for (int nIdx = 0; nIdx < nNumOfExistNodes; nIdx++)
	{
		COneNode & aOneNodeRefer = m_vaNodesInSubGraph[nIdx];
		int nRegionFlag = aOneNodeRefer.GetRegionFlag();
		if (nRegionFlag == 2)
		{	// if we think it is in the region \delta S.
			int nIndexInWholeGraph = aOneNodeRefer.GetNodeIndexInWholeGraph();
			COneNode & aOneNodeReferInWholeGraph = vaAllNodes[nIndexInWholeGraph];
			// Now the following thing all about "aOneNodeReferInWholeGraph" in whole graph.
			int nNumOfAdjNodes = aOneNodeReferInWholeGraph.GetNumberOfAdjacentNodes();
			bool bIsAllNeighborExistInS = true;
			double dTransProbOfSelfLoop = 0.0;
			double dTransProbToDummyNode = 0.0;
			for (int nIdx2 = 0; nIdx2 < nNumOfAdjNodes; nIdx2++)
			{
				int nIndexOfOneAdjNode = aOneNodeReferInWholeGraph.GetOneAdjacentNodeIndex(nIdx2);
				bool bIsOneAdjNodeExistInS = FindWhetherThisNodeExistInSubGraph(nIndexOfOneAdjNode);
				if (bIsOneAdjNodeExistInS == false)
				{
					bIsAllNeighborExistInS = false;
					// If this neighbor node is not in Exist visited nodes, then, we should add it to self loop probability.
					double dProbOut = aOneNodeReferInWholeGraph.GetOneAdjacentNodeTransProbOut(nIdx2);
					double dProbIn  = aOneNodeReferInWholeGraph.GetOneAdjacentNodeTransProbIn(nIdx2);
					dTransProbOfSelfLoop  += dProbOut * dProbIn;
					dTransProbToDummyNode += dProbOut * (1.0 - dProbIn);
				}
			}
			if (bIsAllNeighborExistInS == true)
			{	// If all neighbor nodes exist in S, then, we need to set this node's flag to "1".
				aOneNodeRefer.SetRegionFlag(1);
				aOneNodeRefer.SetTransProbOfSelfLoop(0.0);
				aOneNodeRefer.SetTransProbToDummyNode(0.0);
			} else { // anyway, we update the self-loop probability.
				dTransProbOfSelfLoop  = dTransProbOfSelfLoop  * m_dDecayFactor;
				dTransProbToDummyNode = dTransProbToDummyNode * m_dDecayFactor;
				aOneNodeRefer.SetTransProbOfSelfLoop(dTransProbOfSelfLoop);
				aOneNodeRefer.SetTransProbToDummyNode(dTransProbToDummyNode);
			}
		}
	}
}

bool CSubGraph::FindWhetherThisNodeExistInSubGraph(int nIndexOfOneNodeInWholeGraph){
	bool bFoundThisNodeInThisSubGraph = false;
	int nNumOfNodes = m_vaNodesInSubGraph.size();
	for (int nIdx = 0; nIdx < nNumOfNodes; nIdx++)
	{
		COneNode & aOneNodeRefer = m_vaNodesInSubGraph[nIdx];
		int nIndexOfThisNodeInWholeGraph = aOneNodeRefer.GetNodeIndexInWholeGraph();
		if (nIndexOfThisNodeInWholeGraph == nIndexOfOneNodeInWholeGraph)
		{
			bFoundThisNodeInThisSubGraph = true;
			break;
		}
	}
	return bFoundThisNodeInThisSubGraph;
}

void CSubGraph::AddOneNodeAndUpdateAdjacentEdges(COneNode aOneNode, std::vector<COneNode> & vaAllNodes){
	// "aOneNode" is a node in the local subgraph.
	// "vaAllNodes" is the list of nodes in the whole graph.
	// Get the reference of the new node in the global nodes list.
	int nIndexInWholeOfNewNode = aOneNode.GetNodeIndexInWholeGraph();
	COneNode & aOneNewNodeReferInWhole = vaAllNodes[nIndexInWholeOfNewNode]; // aOneNewNodeReferInWhole: a node in the whole graph
	int nNumOfAdjNodesOfNewNode = aOneNewNodeReferInWhole.GetNumberOfAdjacentNodes(); // number of adjacent node of aOneNewNodeReferInWhole in the whole graph
	int nRegionFlag = aOneNode.GetRegionFlag();
	// Check each exist node in this subgraph, whether there is an edge linking to it.
	if (nRegionFlag == 1)
	{	// If this node is finished, it is in the region S \setminus \delta S.
		int nNumOfExistNodes = m_vaNodesInSubGraph.size();
		for (int nIdx2 = 0; nIdx2 < nNumOfAdjNodesOfNewNode; nIdx2++) // all the adjacent neighbor nodes in whole graph; they are also neighbors in this subgraph
		{
			bool bFoundInExistNodes = false;
			double dTransProbIn  = aOneNewNodeReferInWhole.GetOneAdjacentNodeTransProbIn(nIdx2);
			double dTransProbOut = aOneNewNodeReferInWhole.GetOneAdjacentNodeTransProbOut(nIdx2);
			for (int nIdx3 = 0; nIdx3 < nNumOfExistNodes; nIdx3++) // check all the nodes in this subgraph, to add the transition probability.
			{
				COneNode & aExistNodeInSubGraph = m_vaNodesInSubGraph[nIdx3];
				int nIndexInWholeOfExistNode = aExistNodeInSubGraph.GetNodeIndexInWholeGraph();
				int nIndexInWholeOfOneAdjNode = aOneNewNodeReferInWhole.GetOneAdjacentNodeIndex(nIdx2);
				if (nIndexInWholeOfOneAdjNode == nIndexInWholeOfExistNode)
				{	// If this node (in the subgraph) is that node (in the whole graph), then, we copy the transition probabilities, and setup the adjacent relationship. 
					// Setup the adjacent node in the subgraph.
					aOneNode.AddOneAdjacentNodeIndex(nIdx3); // aOneNode is a node in the local subgraph.
					aOneNode.AddOneAdjacentNodeTransProbIn(dTransProbIn);
					aOneNode.AddOneAdjacentNodeTransProbOut(dTransProbOut);
					aExistNodeInSubGraph.AddOneAdjacentNodeIndex(nNumOfExistNodes); // aExistNodeInSubGraph is also a node in the local subgraph.
					aExistNodeInSubGraph.AddOneAdjacentNodeTransProbIn(dTransProbOut);
					aExistNodeInSubGraph.AddOneAdjacentNodeTransProbOut(dTransProbIn);
					bFoundInExistNodes = true; break;
				}
			}
		}
	} 
	else if (nRegionFlag == 2)
	{	// If this node is the boundary \delta S.
		double dTransProbOfSelfLoop = 0.0;
		double dTransProbToDummyNode = 0.0;
		int nNumOfExistNodes = m_vaNodesInSubGraph.size(); // the total number of nodes in the current subgraph
		for (int nIdx2 = 0; nIdx2 < nNumOfAdjNodesOfNewNode; nIdx2++) // number of adjacent node of aOneNewNodeReferInWhole in the whole graph
		{
			bool bFoundInExistNodes = false;
			double dTransProbIn  = aOneNewNodeReferInWhole.GetOneAdjacentNodeTransProbIn(nIdx2); // aOneNewNodeReferInWhole: a node in the whole graph
			double dTransProbOut = aOneNewNodeReferInWhole.GetOneAdjacentNodeTransProbOut(nIdx2);
			for (int nIdx3 = 0; nIdx3 < nNumOfExistNodes; nIdx3++) // the total number of nodes in the current subgraph
			{	// for each adjacent node in the whole graph of node aOneNewNodeReferInWhole, find whether this adjacent node exists in the current local subgraph.
				COneNode & aExistNodeInSubGraph = m_vaNodesInSubGraph[nIdx3];
				int nIndexInWholeOfExistNode = aExistNodeInSubGraph.GetNodeIndexInWholeGraph();
				int nIndexInWholeOfOneAdjNode = aOneNewNodeReferInWhole.GetOneAdjacentNodeIndex(nIdx2);
				if (nIndexInWholeOfOneAdjNode == nIndexInWholeOfExistNode)
				{	// If this adjacent node exists in the current local subgraph, then set up the adjacent relationship, and copy the transition probabilities.
					// Setup the adjacent node in the subgraph.
					aOneNode.AddOneAdjacentNodeIndex(nIdx3);
					aOneNode.AddOneAdjacentNodeTransProbIn(dTransProbIn);
					aOneNode.AddOneAdjacentNodeTransProbOut(dTransProbOut);
					aExistNodeInSubGraph.AddOneAdjacentNodeIndex(nNumOfExistNodes);
					aExistNodeInSubGraph.AddOneAdjacentNodeTransProbIn(dTransProbOut);
					aExistNodeInSubGraph.AddOneAdjacentNodeTransProbOut(dTransProbIn);
					bFoundInExistNodes = true; break;
				}
			}
			if (bFoundInExistNodes == false)
			{	// If this adjacent node does not exist in the local subgraph, then we need to update the self-loop transitioin probability and the transition probability to the dummy node.
				dTransProbOfSelfLoop  += dTransProbOut * dTransProbIn;
				dTransProbToDummyNode += dTransProbOut * (1 - dTransProbIn);
			}
		}
		dTransProbOfSelfLoop  = dTransProbOfSelfLoop  * m_dDecayFactor;
		dTransProbToDummyNode = dTransProbToDummyNode * m_dDecayFactor;
		aOneNode.SetTransProbOfSelfLoop(dTransProbOfSelfLoop); // aOneNode is a new node in the local subgraph.
		aOneNode.SetTransProbToDummyNode(dTransProbToDummyNode);
	}
	m_vaNodesInSubGraph.push_back(aOneNode); // add this new node aOneNode to this subgraph.
}

bool MyComparator(const stNodeWithUpperLowerBounds& aFst, const stNodeWithUpperLowerBounds& aSnd){ return aFst.m_dLowerBound > aSnd.m_dLowerBound; }; // descending order
bool CSubGraph::CheckTerminationCriteria(){
	// Get the max of upper bound PHP in \delta S.
	std::vector<stNodeWithUpperLowerBounds> vstSortedNodesInSInDescendingOrder;
	vstSortedNodesInSInDescendingOrder.resize(0);
	bool bStopFlag = false;
	int nNumOfNodes = m_vaNodesInSubGraph.size();
	m_dMaxUpperBoundOfNodeInDeltaS = 0.0;
	m_dMaxAvgOfNodeInDeltaS = 0.0;
	for (int nIdx = 0; nIdx < nNumOfNodes; nIdx++) // for each node in the local subgraph
	{
		COneNode & aOneNodeRefer = m_vaNodesInSubGraph[nIdx];
		int nRegionFlag = aOneNodeRefer.GetRegionFlag();
		if (nRegionFlag == 2)
		{	// 2: this node is in the region \delta S. // then, we maintain the maximum in the boundary.
			double dUpperBoundOfNodeInDeltaS = aOneNodeRefer.GetPHPUpperBoundNew();
			double dLowerBoundOfNodeInDeltaS = aOneNodeRefer.GetPHPLowerBoundNew();
			double dAvgLBUBOfNodesInDeltaS = (dUpperBoundOfNodeInDeltaS + dLowerBoundOfNodeInDeltaS) / 2;
			if (m_dMaxUpperBoundOfNodeInDeltaS < dUpperBoundOfNodeInDeltaS){
				m_dMaxUpperBoundOfNodeInDeltaS = dUpperBoundOfNodeInDeltaS; // the maximum in the boundary is updated here. It will be used in the "LocalExpansion()" function.
			}
			if (m_dMaxAvgOfNodeInDeltaS < dAvgLBUBOfNodesInDeltaS){
				m_dMaxAvgOfNodeInDeltaS = dAvgLBUBOfNodesInDeltaS;
				m_nIndexOfNodeWithMaxAvgInDeltaS = nIdx;
			}
		} else if (nRegionFlag == 1)
		{	// 1: this node is in the region S \setminus \delta S. // then, we push it in the sorted list "vstSortedNodesInSInDescendingOrder" (we will sort it later).
			stNodeWithUpperLowerBounds aOneNodeWithUpperLowerBoundPHP;
			aOneNodeWithUpperLowerBoundPHP.m_nIndexOfNode = aOneNodeRefer.GetNodeIndexInWholeGraph();
			aOneNodeWithUpperLowerBoundPHP.m_dLowerBound = aOneNodeRefer.GetPHPLowerBoundNew();
			aOneNodeWithUpperLowerBoundPHP.m_dUpperBound = aOneNodeRefer.GetPHPUpperBoundNew();
			vstSortedNodesInSInDescendingOrder.push_back(aOneNodeWithUpperLowerBoundPHP);
		}
	}
	// Update the constant proximity value of the dummy node.
	m_dDummyNodeConstantPHPValue = m_dMaxUpperBoundOfNodeInDeltaS;
	// Check the termination criteria.
	int nNumOfNodesInS = vstSortedNodesInSInDescendingOrder.size();
	if (nNumOfNodesInS >= m_nNumOfTopKNodes + 1)
	{	// If the number of nodes in (S \setminus \delta S) is greater than the number k in top-k query. Then, we need to check whether they are top-k nodes.
		// Sort the nodes in S based on lower bound nodes in descending.
		std::sort(vstSortedNodesInSInDescendingOrder.begin(), vstSortedNodesInSInDescendingOrder.end(), MyComparator);
		// Compute the maximum upper bound in the node set (S \setminus K)
		double dMaxUpperBoundNotInK = 0.0; // it stores the maximum upper bound in the node set (S \setminus K)
		for (int nIdx = m_nNumOfTopKNodes + 1; nIdx < nNumOfNodesInS; nIdx++)
		{
			stNodeWithUpperLowerBounds& aOneNodeWithUBLB = vstSortedNodesInSInDescendingOrder[nIdx];
			double dTheUpperBoundValueOfThisNode = aOneNodeWithUBLB.m_dUpperBound;
			if (nIdx == m_nNumOfTopKNodes + 1){
				dMaxUpperBoundNotInK = dTheUpperBoundValueOfThisNode;
			} else if (dMaxUpperBoundNotInK < dTheUpperBoundValueOfThisNode){
				dMaxUpperBoundNotInK = dTheUpperBoundValueOfThisNode;
			}
		}
		if (dMaxUpperBoundNotInK < m_dMaxUpperBoundOfNodeInDeltaS){ dMaxUpperBoundNotInK = m_dMaxUpperBoundOfNodeInDeltaS; }
		// Then, we can decide whether we can stop.
		stNodeWithUpperLowerBounds & aOneNodeWithUpperLowerBoundPHPRefer = vstSortedNodesInSInDescendingOrder[m_nNumOfTopKNodes]; // the node refer to the node with the k-th largest lower bound PHP proximity value.
		double dLowerBoundOfKthInS = aOneNodeWithUpperLowerBoundPHPRefer.m_dLowerBound;
		if (dLowerBoundOfKthInS + m_dTerminationThreshold >= dMaxUpperBoundNotInK)
		{	// the the minimum lower bound of the nodes in K is greater than the maximum upper bound of the nodes not in K, then, the top-k nodes are identified.
			// Store the top-k nodes results.
			for (int nIdx = 1; nIdx < m_nNumOfTopKNodes + 1; nIdx++)
			{	// Copy the top "k + 1" nodes, including the query node "q".
				stNodeWithUpperLowerBounds aOneNodeWithUpperLowerBound = vstSortedNodesInSInDescendingOrder[nIdx];
				m_vstTopKIndexOfNodes.push_back(aOneNodeWithUpperLowerBound);
			}
			bStopFlag = true;
		}
	}
	return bStopFlag;
}

void CSubGraph::UpdateLowerBound(){
	// this is the IterativeMethod, for the lower bound.
	double dMaxErrorBetweenTwoElementsInTwoAdjacentIterations = 0.0;
	int    nIndexOfIterations = 0;
	while (true){
		dMaxErrorBetweenTwoElementsInTwoAdjacentIterations = DoOneIterationToUpdateLowerBound();
		nIndexOfIterations++;
		if (nIndexOfIterations > 10000 || dMaxErrorBetweenTwoElementsInTwoAdjacentIterations < 0.00001){ break; }
	}
}

double CSubGraph::DoOneIterationToUpdateLowerBound(){
	// this is one iteration in the IterativeMethod, for the lower bound.
	double dMaxDiff = 0.0;
	int nNumOfNodes = m_vaNodesInSubGraph.size();
	for (int nIdx = 0; nIdx < nNumOfNodes; nIdx++) // For each node in the local subgraph, update its lower bound PHP proximity value.
	{
		if (nIdx == m_nIndexOfSeedNode) // seed
		{
			continue; // Seed node does not need to be updated, always 1.0;
		}
		COneNode & aOneNodeRefer = m_vaNodesInSubGraph[nIdx];
		int nNumOfAdjacentNodes = aOneNodeRefer.GetNumberOfAdjacentNodes();
		double dThisNodeOldPHP = aOneNodeRefer.GetPHPLowerBoundOld();
		double dThisNodeNewPHP = 0.0;
		for (int nIdx2 = 0; nIdx2 < nNumOfAdjacentNodes; nIdx2++)
		{
			int nIdxOfOneAdjNode = aOneNodeRefer.GetOneAdjacentNodeIndex(nIdx2);
			double dProbOut = aOneNodeRefer.GetOneAdjacentNodeTransProbOut(nIdx2);
			COneNode & aOneAdjNodeRefer = m_vaNodesInSubGraph[nIdxOfOneAdjNode];
			double dOneAdjNodePHP = aOneAdjNodeRefer.GetPHPLowerBoundOld();
			dThisNodeNewPHP += dProbOut * dOneAdjNodePHP;
		}
		// Check whether it is a node in \delta S. If it is, add self loop probability.
		int nRegionFlag = aOneNodeRefer.GetRegionFlag();
		double dTransProbOfSelfLoop = aOneNodeRefer.GetTransProbOfSelfLoop();
		if (nRegionFlag == 2)
		{
			dThisNodeNewPHP += dTransProbOfSelfLoop * dThisNodeOldPHP; // add the self-loop transition probability
		}
		dThisNodeNewPHP = dThisNodeNewPHP * m_dDecayFactor; // multiply by the decay factor.
		aOneNodeRefer.SetPHPLowerBoundNew(dThisNodeNewPHP);
		// Update the Max Difference between the old and new lower bound PHP proximity values.
		double dDiffValue = std::abs(dThisNodeOldPHP - dThisNodeNewPHP) / dThisNodeNewPHP; // Use the percentage.
		if (dMaxDiff < dDiffValue)
		{
			dMaxDiff = dDiffValue;
		}
	}
	// Update the old lower bound with the new lower bound.
	for (int nIdx = 0; nIdx < nNumOfNodes; nIdx++)
	{
		if (nIdx == m_nIndexOfSeedNode) // seed
		{
			continue; // Seed node do not need to update, always 1.0;
		}
		COneNode & aOneNodeRefer = m_vaNodesInSubGraph[nIdx];
		double dThisNodeNewPHP = aOneNodeRefer.GetPHPLowerBoundNew();
		aOneNodeRefer.SetPHPLowerBoundOld(dThisNodeNewPHP);
	}
	return dMaxDiff;
}

void CSubGraph::UpdateUpperBound(){
	// this is the IterativeMethod, for the upper bound.
	double dMaxErrorBetweenTwoElementsInTwoAdjacentIterations = 0.0;
	int    nIndexOfIterations = 0;
	while (true){
		dMaxErrorBetweenTwoElementsInTwoAdjacentIterations = DoOneIterationToUpdateUpperBound();
		nIndexOfIterations++;
		if (nIndexOfIterations > 10000 || dMaxErrorBetweenTwoElementsInTwoAdjacentIterations < 0.00001){ break; }
	}
}

double CSubGraph::DoOneIterationToUpdateUpperBound(){
	// this is one iteration in the IterativeMethod, for the upper bound.
	double dMaxDiff = 0.0;
	int nNumOfNodes = m_vaNodesInSubGraph.size();
	for (int nIdx = 0; nIdx < nNumOfNodes; nIdx++) // For each node in the local subgraph, update its upper bound PHP proximity value.
	{
		if (nIdx == m_nIndexOfSeedNode) // seed
		{
			continue; // Seed node do not need to update, always 1.0;
		}
		COneNode & aOneNodeRefer = m_vaNodesInSubGraph[nIdx];
		int nNumOfAdjacentNodes = aOneNodeRefer.GetNumberOfAdjacentNodes();
		double dThisNodeOldPHP = aOneNodeRefer.GetPHPUpperBoundOld();
		double dThisNodeNewPHP = 0.0;
		for (int nIdx2 = 0; nIdx2 < nNumOfAdjacentNodes; nIdx2++)
		{
			int nIdxOfOneAdjNode = aOneNodeRefer.GetOneAdjacentNodeIndex(nIdx2);
			double dProbOut = aOneNodeRefer.GetOneAdjacentNodeTransProbOut(nIdx2);
			COneNode & aOneAdjNodeRefer = m_vaNodesInSubGraph[nIdxOfOneAdjNode];
			double dOneAdjNodePHP = aOneAdjNodeRefer.GetPHPUpperBoundOld();
			dThisNodeNewPHP += dProbOut * dOneAdjNodePHP;
		}
		int nRegionFlag = aOneNodeRefer.GetRegionFlag();
		double dTransProbOfSelfLoop = aOneNodeRefer.GetTransProbOfSelfLoop();
		double dTransProbToDummyNode = aOneNodeRefer.GetTransProbToDummyNode();
		if (nRegionFlag == 2)
		{
			dThisNodeNewPHP += dTransProbOfSelfLoop * dThisNodeOldPHP; // add the self-loop transition probability
			dThisNodeNewPHP += dTransProbToDummyNode * m_dDummyNodeConstantPHPValue; // add the transition probability to the dummy node.
		}
		dThisNodeNewPHP = dThisNodeNewPHP * m_dDecayFactor; // multiply by the decay factor.
		aOneNodeRefer.SetPHPUpperBoundNew(dThisNodeNewPHP);
		// Update the Max Difference between the old and new upper bound PHP proximity values.
		double dDiffValue = std::abs(dThisNodeOldPHP - dThisNodeNewPHP) / dThisNodeNewPHP; // Use the percentage.
		if (dMaxDiff < dDiffValue)
		{
			dMaxDiff = dDiffValue;
		}
	}
	// Update the old lower bound with the new lower bound.
	for (int nIdx = 0; nIdx < nNumOfNodes; nIdx++)
	{
		if (nIdx == m_nIndexOfSeedNode) // seed
		{
			continue; // Seed node do not need to update, always 1.0;
		}
		COneNode & aOneNodeRefer = m_vaNodesInSubGraph[nIdx];
		double dThisNodeNewPHP = aOneNodeRefer.GetPHPUpperBoundNew();
		aOneNodeRefer.SetPHPUpperBoundOld(dThisNodeNewPHP);
	}
	return dMaxDiff;
}

void CSubGraph::GetTheTopKNodesIndexResults(std::vector<stNodeWithUpperLowerBounds> & vstTopKIndexOfNodes){
	vstTopKIndexOfNodes.resize(0);
	for (int nIdx = 0; nIdx < m_nNumOfTopKNodes; nIdx++)
	{
		stNodeWithUpperLowerBounds aOneNodeWithUpperLowerBound = m_vstTopKIndexOfNodes[nIdx];
		vstTopKIndexOfNodes.push_back(aOneNodeWithUpperLowerBound);
	}
};
