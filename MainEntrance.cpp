#include "MyHeader.h"
#include "OneGraph.h"

int main(int argc, char * argv[]){
	// 1. The variables to store the input and output file names or folders.
	string sInputFileNameNodes; // Input file name for nodes of the graph.
	string sInputFileNameEdges; // Input file name for edges of the graph.
	string sInputFileNameSeeds; // Input file name for seed nodes of the graph.
	string sOutputFolder; // The folder to output the results.
	double dDecayFactor = 0.0; // the decay factor in PHP.
	double dTerminationThreshold = 0.0; // the constant used for the termination of the algorithm.
	int    nNumOfTopK = 0; // the number k for top-k query

	// 2. Parsing the input parameters.
	int nIdx = 1; // index for parsing the input parameters.
	while (nIdx < argc)
	{
		string sToken1 = argv[nIdx];
		string sToken2 = argv[nIdx + 1];
		nIdx = nIdx + 2;
		if (sToken1.compare("-n") == 0){ // Input file name for nodes of the graph.
			sInputFileNameNodes = sToken2;
		} else if (sToken1.compare("-e") == 0){ // Input file name for edges of the graph.
			sInputFileNameEdges = sToken2;
		} else if (sToken1.compare("-s") == 0){ // Input file name for seed nodes of the graph.
			sInputFileNameSeeds = sToken2;
		} else if (sToken1.compare("-o") == 0){ // Output file path
			sOutputFolder = sToken2;
		} else if (sToken1.compare("-c") == 0){ // Output file path
			dDecayFactor = atof(sToken2.c_str());
		} else if (sToken1.compare("-u") == 0){ // Output file path
			dTerminationThreshold = atof(sToken2.c_str());
		} else if (sToken1.compare("-k") == 0){ // Output file path
			nNumOfTopK = atoi(sToken2.c_str());
		}
	}

	// 3. The main variable: an instance of the COneGraph class.
	COneGraph aOneGraph;

	// 4. Set the input parameters and read the input files.
	aOneGraph.SetDecayFactor(dDecayFactor);
	aOneGraph.SetTerminationThreshold(dTerminationThreshold);
	aOneGraph.SetNumberKForTopKQuery(nNumOfTopK);
	aOneGraph.SetOutputFolder(sOutputFolder);
	aOneGraph.ReadNodesFile(sInputFileNameNodes);
	aOneGraph.ReadEdgesFile(sInputFileNameEdges);
	aOneGraph.ReadSeedNodesFile(sInputFileNameSeeds);

	// 5. Pre-processing the graph.
	aOneGraph.SetUpAdjacentNodesAndInducedEdgeForEachNode(); // 1) construct the adjacency list;
	aOneGraph.CalculateTransitionProbMatrix(); // 2) calculate the transition probabilities.

	// 6. The FLoS_PHP algorithm begins.
	aOneGraph.FLoS_PHP_ForEachSeedNode();

	return 0;
}
