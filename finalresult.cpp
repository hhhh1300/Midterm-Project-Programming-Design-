#include<iostream>
#include<math.h>
#include<climits>
#include<algorithm>

using namespace std;

#define MAX_CENTER_CNT 50
#define MAX_STORE_CNT 1000

/*continue or not*/
bool judgement(int centerCapacity[], int center, int storestatus[], int store, int centerstatus[]);
bool judgement2(int centerCapacity[], int len, int storestatus[], int storeTocenter[], int store, int centerstatus[], int demand[]);
/*store mate center*/
int clostestCenter(int centerLocation[][2], int storeLocation[][2], int len, int center, int distanceCost, int centerCapacity[], int demand[], int centerstatus[]);
/*find the highest singleprice store*/
void inverseFind(int store, int i, int storestatus[], int storeTocenter[], int singleProfit[], int centerCapacity[], int demand[], int centerSent[][100], int storeProfit[], int centerProfit[]);
/*print answer*/
void print(int center, int store, int centerstatus[], int storestatus[], int centerCNT, int storeCNT, int centerSent[][100]);

void deleteCenter(int center, int centerBuildingCost[], int centerProfit[], int centerstatus[], int store, int storeTocenter[], int storestatus[], int storeProfit[], int centerSent[][100]);
void deleteStore(int store, int storeBuildingCost[], int storeProfit[], int storestatus[], int centerCapacity[], int storeTocenter[], int centerSent[][100], int centerProfit[]);

int min(int a, int b){return a < b ? a : b;}
int distance(int *store, int *center);   
//to calculate the distance between store and center
int deliverCost(int *store, int *center, int distanceCost);  
//to calculate the delivery cost form this center to that store
double revenuePerCost(int deliverCost, int storeBuildingCost, int centerBuildingCost, int price); 
//to calculate RPC(waiting to be improved)
bool valid(int delivercost, int price);   
//to check if the delivery cost is bigger than the revenue(also waiting to be improved)
int centerTotalSent(int* centerSent, int store);   
//to calculate how many products this center has sent
int profit(int centerSent[][MAX_STORE_CNT], int storeReceived[], int centerLocation[][2], int storeLocation[][2], int centerBuildingCost[], int storeBuildingCost[],int center, int store, int distanceCost, int price[]); 
double centerProfit(int centerSent[], int storeReceived[], int centerLocation[], int storeLocation[][2], int centerBuildingCost, int storeBuildingCost[], int store, int distanceCost, int price[]);
void print(int centerSent[][MAX_STORE_CNT], int storeReceived[], int center, int store);
//to print

int main()
{
	/*input*/
	int center = 0, store = 0, distanceCost = 0, type = 0;
	int centerLocation[100][2] = { 0 };
	int storeLocation[1050][2] = { 0 };
	int demand[1050] = { 0 };
	int storeBuildingCost[1050] = { 0 };
	int price[1050] = { 0 };
	int centerBuildingCost[100] = { 0 };
	int centerCapacity[100] = { 0 };

	cin >> center >> store >> distanceCost >> type;
	for (int i = 0; i < center; i++)
		for (int j = 0; j < 2; j++)
			cin >> centerLocation[i][j];
	for (int i = 0; i < store; i++)
		for (int j = 0; j < 2; j++)
			cin >> storeLocation[i][j];
	for (int i = 0; i < store; i++)
		cin >> demand[i];
	for (int i = 0; i < store; i++)
		cin >> storeBuildingCost[i];
	for (int i = 0; i < store; i++)
		cin >> price[i];
	for (int i = 0; i < center; i++)
		cin >> centerBuildingCost[i];
	for (int i = 0; i < center; i++)
		cin >> centerCapacity[i];
	
	if(type == 2)
	{
		int centerSent[MAX_CENTER_CNT][MAX_STORE_CNT] = {0};//for every store
	    int storeReceived[MAX_STORE_CNT] = {0};
	    bool flag = true;
	    while(flag)
	    {
	        /*initialize the variable*/
	        flag = false;
	        double tempMaxRPC = -1;
	        int storeId = -1;
	        int centerId = -1;
	
	        /*to calculate which combination of center and store can bring the max profit*/
	        for(int i = 0; i < center; i++)
	        {
	            if(centerTotalSent(centerSent[i],store) >= centerCapacity[i])//to check if the center is out of capacity
	                continue;
	            for(int j = 0; j < store; j++)
	            {
	                int tempDeliverCost = deliverCost(storeLocation[j],centerLocation[i],distanceCost);
	                double RPC = revenuePerCost(tempDeliverCost, storeBuildingCost[j], centerBuildingCost[i], price[j]);
	                if(valid(tempDeliverCost, price[j]) && RPC > tempMaxRPC && storeReceived[j] < demand[j])
	                {
	                    tempMaxRPC = RPC;
	                    storeId = j;
	                    centerId = i;
	                    flag = true;
	                }        
	            }
	
	        }
	        if(storeId >= 0 && centerId >= 0)
	        {
	            int m = min(centerCapacity[centerId] - centerTotalSent(centerSent[centerId], store), demand[storeId] - storeReceived[storeId]);
	            if(m > 0)
	            {
	                storeReceived[storeId] += m;
	                centerSent[centerId][storeId] += m;
	            }
	        }
	    }
	
	    /*If the profit this store made is negative, then reset the storeReceived and centerSent*/
	    for(int i = 0; i < store; i++)
	    {
	        int storeProfit = 0;
	        for(int j = 0; j < center; j++)
	            storeProfit += centerSent[j][i] * (price[i] - deliverCost(storeLocation[i], centerLocation[j], distanceCost));
	        storeProfit -= storeBuildingCost[i];
	        if(storeProfit < 0)
	        {
	            storeReceived[i] = 0;
	            for(int j = 0; j < center; j++)
	                centerSent[j][i] = 0;
	        }
	    }
	    
	    /*If the profit this center made is negative, then reset the storeReceived and centerSent(不知道為甚麼這個算法會出問題，待修正)*/
	    for(int i = 0; i < center; i++)
	    {
	        int tempProfit = centerProfit(centerSent[i], storeReceived, centerLocation[i], storeLocation, centerBuildingCost[i], storeBuildingCost, store, distanceCost, price);
	        #ifdef DEBUG
	        cout << "centerProfit : " << centerProfit(centerSent[i], storeReceived, centerLocation[i], storeLocation, centerBuildingCost[i], storeBuildingCost, store, distanceCost, price) << endl;
	        #endif
	        
	        if(tempProfit < 0)
	        {
	            for(int j = 0; j < store; j++)
	            {
	                storeReceived[j] -= centerSent[i][j];
	                centerSent[i][j] = 0;
	            }  
	        }
	    }
	    
	    /*If the total profit we got is negative, then reset all elements of centerSent and storeReceived*/
	    if(profit(centerSent, storeReceived, centerLocation, storeLocation, centerBuildingCost, storeBuildingCost, center, store, distanceCost, price) < 0)
	    {
	        for(int i = 0; i < center; i++)
	            for(int j = 0; j < store; j++)
	                centerSent[i][j] = 0;
	        for(int i = 0; i < store; i++)
	            storeReceived[i] = 0;
	    }
	    
    
	   
	    /*output*/
	    #ifdef DEBUG
	    cout << "totalProfit : " << profit(centerSent, storeReceived, centerLocation, storeLocation, centerBuildingCost, storeBuildingCost, center, store, distanceCost, price) << endl;
	    for(int j = 0; j < store; j++)
	    {
	        int storeProfit = 0;
	        for(int k = 0; k < center; k++)
	            storeProfit += centerSent[k][j] * (price[j] - deliverCost(storeLocation[j], centerLocation[k], distanceCost));
	        storeProfit -= storeBuildingCost[j];
	       
	        cout << "storeProfit :" << storeProfit << endl;
	    }
	    #endif
	    print(centerSent, storeReceived, center, store);

	}

	else if (type == 1)
	{
		int centerSent[1050][100] = { 0 };
		int storeTocenter[1050] = { 0 };
		int centerstatus[100] = { 1 };             // 1=build 2=not build
		int storestatus[1050] = { 0 };             // 0=uncertain 2=build 3=not build
		int distance[1050] = { 0 };
		int singleProfit[1050] = { 0 };
		int centerProfit[100] = { 0 };
		int storeProfit[1050] = { 0 };
		int storeCNT = 0, centerCNT = 0;
		fill_n(centerstatus, 50, 1);
		
		while (judgement(centerCapacity, center, storestatus, store, centerstatus))
		{
			int tem3 = 0;
			/*mate store and center*/
			for (int i = 0; i < store; i++)
			{
				if (storestatus[i] == 0)
				{
					int tem;
					tem = clostestCenter(centerLocation, storeLocation, i, center, distanceCost, centerCapacity, demand, centerstatus);
					if (tem == -1)
						storestatus[i] = 3;
					storeTocenter[i] = tem;
				}
			}
			/*calculate singleprofit*/
			for (int j = 0; j < store; j++)
			{
				if (storestatus[j] == 0)
				{
					distance[j] = abs(storeLocation[j][0] - centerLocation[storeTocenter[j]][0]) + abs(storeLocation[j][1] - centerLocation[storeTocenter[j]][1]);
					singleProfit[j] = price[j] - (distance[j] * distanceCost);
					if (storeBuildingCost[j] >= singleProfit[j] * demand[j])
						storestatus[j] = 3;
				}
			}

			for (int i = 0; i < center; i++)
			{
				/*inverse find*/
				while (judgement2(centerCapacity, i, storestatus, storeTocenter, store, centerstatus, demand))
				{
					inverseFind(store, i, storestatus, storeTocenter, singleProfit, centerCapacity, demand, centerSent, storeProfit, centerProfit);
				}
			}

			/*delete store*/
			deleteStore(store, storeBuildingCost, storeProfit, storestatus, centerCapacity, storeTocenter, centerSent, centerProfit);

			/*delete center*/
			deleteCenter(center, centerBuildingCost, centerProfit, centerstatus, store, storeTocenter, storestatus, storeProfit, centerSent);
		}
		/*print answer*/
		print(center, store, centerstatus, storestatus, centerCNT, storeCNT, centerSent);
	}

	return 0;
}

bool judgement(int centerCapacity[], int center, int storestatus[], int store, int centerstatus[])
{
	int tem = 0, tem2 = 0;
	for (int i = 0; i < center; i++)
	{
		if (centerCapacity[i] > 0 && centerstatus[i] == 1)
			tem++;
	}
	if (tem == 0)
		return false;

	for (int j = 0; j < store; j++)
	{
		if (storestatus[j] < 2)
			tem2++;
	}
	if (tem2 == 0)
		return false;

	return true;
}

int clostestCenter(int centerLocation[][2], int storeLocation[][2], int len, int center, int distanceCost, int centerCapacity[], int demand[], int centerstatus[])
{
	int lowestDistance = INT_MAX;
	int nearCenter = -1;
	for (int i = 0; i < center; i++)
	{
		if (centerCapacity[i] > 0 && centerstatus[i] == 1)
		{
			int distance;
			distance = abs(storeLocation[len][0] - centerLocation[i][0]) + abs(storeLocation[len][1] - centerLocation[i][1]);
			if (lowestDistance > distance)
			{
				lowestDistance = distance;
				nearCenter = i;
			}
		}
	}
	return nearCenter;
}

bool judgement2(int centerCapacity[], int len, int storestatus[], int storeTocenter[], int store, int centerstatus[], int demand[])
{
	int tem = 0;
	for (int i = 0; i < store; i++)
	{
		if (storestatus[i] == 0 && storeTocenter[i] == len && centerCapacity[len] > 0 && centerstatus[len] == 1)
			return true;
	}
	return false;
}

void inverseFind(int store, int i, int storestatus[], int storeTocenter[], int singleProfit[], int centerCapacity[], int demand[],
	int centerSent[][100], int storeProfit[], int centerProfit[])
{
	int tem = 0, tem2 = 0;
	for (int j = 0; j < store; j++)
	{
		if (storeTocenter[j] == i && storestatus[j] == 0)
		{
			if (singleProfit[j] > tem)
			{
				tem = singleProfit[j];
				tem2 = j;
			}
		}
	}

	if (centerCapacity[i] >= demand[tem2])
	{
		centerSent[tem2][i] = demand[tem2];
		centerProfit[i] += demand[tem2] * singleProfit[tem2];
		storeProfit[tem2] = demand[tem2] * singleProfit[tem2];
		centerCapacity[i] -= demand[tem2];
		storestatus[tem2] = 2;
	}
	else if (centerCapacity[i] < demand[tem2])
	{
		centerSent[tem2][i] = centerCapacity[i];
		centerProfit[i] += centerCapacity[i] * singleProfit[tem2];
		storeProfit[tem2] = centerCapacity[i] * singleProfit[tem2];
		centerCapacity[i] = 0;
		storestatus[tem2] = 2;
	}
}

void deleteStore(int store, int storeBuildingCost[], int storeProfit[], int storestatus[], int centerCapacity[], int storeTocenter[], int centerSent[][100], int centerProfit[])
{
	for (int i = 0; i < store; i++)
	{
		if (storeBuildingCost[i] >= storeProfit[i])
		{
			storestatus[i] = 3;
			centerCapacity[storeTocenter[i]] += centerSent[i][storeTocenter[i]];
			centerSent[i][storeTocenter[i]] = 0;
			centerProfit[storeTocenter[i]] -= storeProfit[i];
			storeProfit[i] = 0;
		}
	}
}

void deleteCenter(int center, int centerBuildingCost[], int centerProfit[], int centerstatus[], int store, int storeTocenter[], int storestatus[], int storeProfit[], int centerSent[][100])
{
	for (int i = 0; i < center; i++)
	{
		if (centerBuildingCost[i] >= centerProfit[i])
		{
			centerstatus[i] = 2;
			for (int j = 0; j < store; j++)
			{
				if (storeTocenter[j] == i)
				{
					storestatus[j] = 0;
					storeProfit[j] = 0;
				}
				centerSent[j][i] = 0;
			}
			centerProfit[i] = 0;
		}
	}
}

void print(int center, int store, int centerstatus[], int storestatus[], int centerCNT, int storeCNT, int centerSent[][100])
{
	for (int i = 0; i < center; i++)
	{
		if (centerstatus[i] == 1)
			centerCNT++;
	}
	for (int i = 0; i < store; i++)
	{
		if (storestatus[i] == 2)
		{
			storeCNT++;
		}
	}
	cout << centerCNT;
	for (int i = 0; i < center; i++)
	{
		if (centerstatus[i] == 1)
			cout << " " << i + 1;
	}
	cout << endl;
	cout << storeCNT;
	for (int i = 0; i < store; i++)
	{
		if (storestatus[i] == 2)
			cout << " " << i + 1;
	}
	cout << endl;
	for (int i = 0; i < store; i++)
	{
		for (int j = 0; j < center; j++)
		{
			if (j == 0)
				cout << centerSent[i][j];
			else
				cout << " " << centerSent[i][j];
		}
		if (i != store - 1)
			cout << endl;
	}
}

int distance(int *store, int *center)
{
    return abs(store[0] - center[0]) + abs(store[1] - center[1]);
}

int deliverCost(int *store, int *center, int distanceCost)
{
    return distance(store,center) * distanceCost;
}
double revenuePerCost(int deliverCost, int storeBuildingCost, int centerBuildingCost, int price)
{
    return static_cast<double>(price) / (deliverCost + storeBuildingCost + centerBuildingCost);
}
int centerTotalSent(int* centerSent, int store)
{
    int total = 0;
    for(int i = 0; i < store; i++)
        total += centerSent[i];
    return total;
}
bool valid(int delivercost, int price)
{
    return price > delivercost ? true : false;
}
void print(int centerSent[][MAX_STORE_CNT], int storeReceived[], int center, int store)
{
    int totalCenter = 0, centerId[MAX_CENTER_CNT] = {0};
    for(int i = 0; i < center; i++)
        if(centerTotalSent(centerSent[i], store) > 0)
        {
            centerId[i]++;
            totalCenter++;
        }
    cout << totalCenter << " ";
    for(int i = 0; i < center; i++)
    {
        if(centerId[i] != 0)
            cout << i + 1 << " ";
    }
    cout << endl;

    int totalStore = 0, storeId[MAX_STORE_CNT] = {0};
    for(int i = 0; i < store; i++)
        if(storeReceived[i] != 0)
        {
            totalStore++;
            storeId[i]++;
        }
    cout << totalStore << " ";
    for(int i = 0; i < store; i++)
    {
        if(storeId[i] != 0)
            cout << i + 1 << " ";
    }
    cout << endl;

    for(int i = 0; i < store; i++)
    {
        for(int j = 0; j < center; j++)
        {
            cout << centerSent[j][i] << " ";
        }
        cout << endl;
    }
}

int profit(int centerSent[][MAX_STORE_CNT], int storeReceived[], int centerLocation[][2], int storeLocation[][2], int centerBuildingCost[], int storeBuildingCost[], int center, int store, int distanceCost, int price[])
{
    int totalProfit = 0;

    for(int i = 0; i < center; i++)
    {
        bool centerFlag = false;
        for(int j = 0; j < store; j++)
        {
            if(centerSent[i][j] != 0)
            {
                // cout << j << " single profit :" << (price[j] - distanceCost * distance(storeLocation[j],centerLocation[i])) * centerSent[i][j] << " " << distanceCost * distance(storeLocation[j],centerLocation[i]) << endl;
                totalProfit += (price[j] - distanceCost * distance(storeLocation[j],centerLocation[i])) * centerSent[i][j];
                centerFlag = true;
            }
        }
        if(centerFlag)
            totalProfit -= centerBuildingCost[i];
    }
    for(int i = 0; i < store; i++)
    {
        if(storeReceived[i] != 0)
            totalProfit -= storeBuildingCost[i];
    }
    return totalProfit ;
}

double centerProfit(int centerSent[], int storeReceived[], int centerLocation[], int storeLocation[][2], int centerBuildingCost, int storeBuildingCost[], int store, int distanceCost, int price[])
{
    double totalProfit = 0;
    for(int i = 0; i < store; i++)
    {
        if(centerSent[i] != 0)
        {
            totalProfit += (price[i] - distanceCost * distance(storeLocation[i],centerLocation)) * centerSent[i] - static_cast<double>(storeBuildingCost[i]) * (centerSent[i] / storeReceived[i]) ;
        }
    }
    totalProfit -= centerBuildingCost;
    return totalProfit ;
}


