/*
VertexDeDuplicator can remove duplicate vertices from a S3D file.
*/

#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<tuple>

struct Vertex
{
	float attributes[8];  //x y z nx ny nz u v
	bool operator==(const Vertex& rhs) const
	{
		return (attributes[0] == rhs.attributes[0] &&
			attributes[1] == rhs.attributes[1] && 
			attributes[2] == rhs.attributes[2] && 
			attributes[3] == rhs.attributes[3] && 
			attributes[4] == rhs.attributes[4] && 
			attributes[5] == rhs.attributes[5] && 
			attributes[6] == rhs.attributes[6] && 
			attributes[7] == rhs.attributes[7]);
	}
};

int main(int argc, char**argv)
{
	if (argc < 2 || argc > 3)
	{
		std::cout << "USAGE: " << argv[0] << " <input S3D file> <optional - output S3D file>\n";
		return -1;
	}

	std::string inputFileName = argv[1];
	std::string outputFilename;
	if (argc == 3) outputFilename = argv[2];
	else outputFilename = inputFileName;

	std::vector<Vertex> vertexList1;
	std::vector<unsigned int> indicesList;
	int vcount = 0; //vertex count
	int icount = 0; //index count
	std::string textureFilename;

	//open S3D file for input
	std::ifstream inFile(inputFileName);
	if (inFile.is_open())
	{
		//read vertex count
		inFile >> vcount;
		std::cout << "\n\tVertex count before processessing: " << vcount << "\n";

		vertexList1.resize(vcount); //allocate our vertex list in one go

		//read vertex data
		for (int i = 0; i < vcount; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				inFile >> vertexList1[i].attributes[j];
			}
		}

		//read index count
		inFile >> icount;
		indicesList.resize(icount); //allocate our indices list in one go

		//read index data
		for (int i = 0; i < icount; ++i)
		{
			inFile >> indicesList[i];
		}

		//read texture filename (no spaces version)
		inFile >> textureFilename;

		inFile.close();
	}

	//place to keep track of re-ordered vertex list
	std::vector<Vertex *> vertexList2;
	//to track whether we have de-duplicated this vertex before
	std::vector<bool> processedList(vcount,false);

	//try to de-duplicate each verstex
	for (int i = 0; i < vcount; ++i)
	{
		//have we already de-duped one of these exact vertices? If so, move on.
		if (processedList[i]) continue;
		processedList[i] = true;

		//add this vertex to our new list
		vertexList2.push_back(&vertexList1[i]);
		int newIndex = vertexList2.size() - 1; //keep track of this index

		//start one past the i'th vertex
		for (int j = i; j < vcount; ++j)
		{
			if (vertexList1[i] == vertexList1[j])
			{
				//duplicate!
				//replace any indices that point to the second vertex with the new first vertex index
				for (int index = 0; index < icount; ++index)
				{
					if (indicesList[index] == j)
					{
						indicesList[index] = newIndex;
						processedList[j] = true;
					}
				}
			}
		}
	}//end of de-duplicating loop

	vcount = vertexList2.size();
	std::cout << "\n\tVertex count after processessing: " << vcount << "\n";

	//now output the new file
	std::ofstream outFile(outputFilename);

	if (!outFile.is_open())
	{
		std::cout << "ERROR! Can't open " << outputFilename << " for output.\n";
		return -2;
	}

	
	//vertex data
	outFile << vcount << "\n";
	for (int i = 0; i < vcount; ++i)
	{
		for (int j = 0; j < 8; ++j)
			outFile << vertexList2[i]->attributes[j] << " ";
		
		outFile << "\n";
	}

	//index data
	outFile << icount << "\n";
	int counter = 0;
	for (counter = 0; counter < icount; ++counter)
	{
		outFile << indicesList[counter] << " ";
		
		if(!counter %20 && counter != 0) outFile << "\n";
	}
	if (counter % 20) outFile << "\n";
	//texture file name
	outFile << textureFilename;

	outFile.close();

	return 0;
}