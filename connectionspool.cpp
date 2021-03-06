#include <stdio.h>
#include <algorithm>

#include "connectionspool.h"

connectionsPool::connectionsPool()
{
	maxConnNr[0] = 0;
	maxConnNr[1] = 0;

	nrAvailable[0] = 0;
	nrAvailable[1] = 0;
}

set<size_t> connectionsPool::getAllUnavailable()
{
	set<size_t> ret;

	ret.insert(reservedButNotAssigned[0].begin(), reservedButNotAssigned[0].end());
	ret.insert(reservedButNotAssigned[1].begin(), reservedButNotAssigned[1].end());
	ret.insert(assigned[0].begin(), assigned[0].end());
	ret.insert(assigned[1].begin(), assigned[1].end());


//	/*tobeavailable are also not available*/
//	ret.insert(toBeAvailable.begin(), toBeAvailable.end());

	return ret;
}

void connectionsPool::preReleaseConnection(int boxType, size_t connNr)
{
	assigned[boxType].erase(connNr);
	toBeAvailable[boxType].insert(connNr);
}

void connectionsPool::releaseConnection(int boxType, size_t connNr)
{
	if(toBeAvailable[boxType].find(connNr) == toBeAvailable[boxType].end())
	{
		printf("NOT IN TOBEAVAILABLE %d\n", connNr);
	}
	toBeAvailable[boxType].erase(connNr);


	nrAvailable[boxType]++;
	available.push_back(connNr);
}

bool connectionsPool::sufficientToConsume(int boxType)
{
	return reservedButNotAssigned[boxType].size() != 0;
}

size_t connectionsPool::consumeConnection(int boxType)
{
	size_t ret = reservedButNotAssigned[boxType].front();
	reservedButNotAssigned[boxType].erase(reservedButNotAssigned[boxType].begin());

	assigned[boxType].insert(ret);

	return ret;
}

bool connectionsPool::sufficientAvailable(int boxType)
{
	return /*available.size() != 0*/nrAvailable[boxType] > 0;
}

bool connectionsPool::increaseAvailable(int boxType, int max)
{
	if(maxConnNr[boxType] < max)
	{
//		available.push_back(maxConnNr[boxType]);
		available.push_back(maxConnNr[0] + maxConnNr[1]);

		maxConnNr[boxType]++;
		nrAvailable[boxType]++;
	}
	else
	{
		return false;
	}

	return true;
}

/**
 * Reserve a connection of a specific type
 * @param boxType type of the connection
 * @return the connection's number
 */
size_t connectionsPool::reserveConnection(int boxType)
{
	size_t ret = available.front();
	available.erase(available.begin());

	/*17.02.2017*/
	nrAvailable[boxType]--;

	reservedButNotAssigned[boxType].push_back(ret);

//	printf("reserve t%d c%d\n", boxType, ret);

	return ret;
}

size_t connectionsPool::reserveConnectionPreferred(int boxType, size_t approxConnectionNumber)
{
	size_t currentApprox = approxConnectionNumber;

	vector<size_t>::iterator foundIterator = available.end();

	bool doNotFlipDirection = false;
	int step = 0;
	int direction = 1;

	while(foundIterator == available.end())
	{
		if(currentApprox == 0)
		{
			doNotFlipDirection = true;
			direction = 1;
		}
		else
		{
			direction = direction * -1;
		}

		currentApprox = currentApprox + direction * step;
		printf("..... %lu %d\n", currentApprox, step);

		step++;


		foundIterator = std::find(available.begin(), available.end(), currentApprox);

	}

	available.erase(foundIterator);

	nrAvailable[boxType]--;

	reservedButNotAssigned[boxType].push_back(currentApprox);

	printf("-------");

	return currentApprox;
}

