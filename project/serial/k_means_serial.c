#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define N 1000
#define K 10
#define THRESHOLD 0.0
#define RANGE 1000

typedef struct Point
{
	double x;
	double y;
}Point;

//--- array of data objects-----------
Point objects[N]; 
//---- array of cluster centers-------
Point clusters[K];
//---- array of object memberships ---
int membership[N];

//--- output results ----------------
void printcluster()
{
	int i;
	for(i = 0; i < K; i++)
	{
		printf("Cluster %d\n", i);
		printf("x: %f, y: %f\n", clusters[i].x, clusters[i].y);
	}
	printf("\n\n");
}


//---- generate random objects ------
void initialize_object()
{
	int i;
	srand(5);
	for(i = 0; i < N; i++)
	{
		objects[i].x = rand() % RANGE;
		objects[i].y = rand() % RANGE;
	}
}

double get_dist(Point p1, Point p2)
{
	double dist;
	double tmp_1 = pow(p1.x - p2.x, 2);
	double tmp_2 = pow(p1.y - p2.y, 2);
	dist = fabs(sqrt(tmp_1 + tmp_2));
	return dist;
}

k_means_clustering()
{
	double delta;
	int i,j;   

	initialize_object();

	//------- initialize cluster centers -----
	for(i = 0; i < K; i++)
	{
		clusters[i].x = objects[i].x;
		clusters[i].y = objects[i].y;
	} 
	//---- intialize membership ------------- 
	for(i = 0; i < N; i++)
	{
		if(i < K)
			membership[i] = i;
		else
		{

			double dist_min = get_dist(objects[i], clusters[0]);
			int n = 0;
			for(j = 0; j < K; j++)
			{
				double dist = get_dist(objects[i], clusters[j]);
				if(dist < dist_min)
				{
					dist_min = dist;
					n = j;
				}             
			}
			membership[i] = n;
		}
	}
	
	printf("1st :\n");
	printcluster();

	delta = N;
	while(delta > THRESHOLD)
	{
		//--- update to get new centers for each cluster---
		for(i = 0; i < K; i++)
		{
			double new_center_x = 0;
			double new_center_y = 0; 
			int cluster_num = 0;
			for(j = 0; j < N; j++)
			{
				if(membership[j] == i)
				{
					new_center_x += objects[j].x;
					new_center_y += objects[j].y;
					cluster_num ++;
				}
			}
			clusters[i].x = new_center_x/cluster_num;
			clusters[i].y = new_center_y/cluster_num;
		}       

		delta = 0.0;
		//--- calculate to update each obj's membership ----
		for(i = 0; i < N; i++)
		{
			double dist_min = get_dist(objects[i], clusters[0]);
			int n = 0;
			for(j = 0; j < K; j++)
			{
				double dist = get_dist(objects[i], clusters[j]);
				if(dist < dist_min)
				{
					dist_min = dist;
					n = j;
				}             
			}
			if (membership[i] != n)
			{
				delta += 1;
				membership[i] = n;
			}
		}

	}
	printcluster();   

}


int main()
{
	k_means_clustering();

	return 0;    
}
