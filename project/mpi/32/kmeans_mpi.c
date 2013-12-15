#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

#define N 1000
#define K 10
#define THRESHOLD 0.0
#define RANGE 1000
#define tag_1 1
#define tag_2 2
#define tag_3 3
#define tag_4 4
#define tag_5 5

//--- array of data objects-----------
double objects_x[N]; 
double objects_y[N];
//---- array of cluster centers-------
double clusters_x[K];
double clusters_y[K];
//---- array of object memberships ---
int membership[N];

//--- output results ----------------
void printcluster()
{
	int i,j;
	for(i = 0; i < K; i++)
	{
		printf("Cluster %d\n", i);
		printf("x: %f, y: %f\n", clusters_x[i], clusters_y[i]);
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
		objects_x[i] = rand() % RANGE;
		objects_y[i] = rand() % RANGE;
	}
}

double get_dist(double p1_x, double p1_y, double p2_x, double p2_y)
{
	double dist;
	double tmp_1 = pow(p1_x - p2_x, 2);
	double tmp_2 = pow(p1_y - p2_y, 2);
	dist = fabs(sqrt(tmp_1 + tmp_2));
	return dist;
}

void initialize_cluster()
{
	int i;
	for(i = 0; i < K; i++)
	{
		clusters_x[i] = objects_x[i];
		clusters_y[i] = objects_y[i];
	}
}

void initialize_membership()
{
	int i;
	for(i = 0; i < N; i++)
	{
		if(i < K)
			membership[i] = i;
		else
		{

			double dist_min = get_dist(objects_x[i], objects_y[i], clusters_x[0], clusters_y[0]);
			int n = 0;
			int j;
			for(j = 0; j < K; j++)
			{
				double dist = get_dist(objects_x[i], objects_y[i], clusters_x[j], clusters_y[j]);
				if(dist < dist_min)
				{
					dist_min = dist;
					n = j;
				}             
			}
			membership[i] = n;
		}
	}
}

k_means_clustering()
{
	//--- for parallel --------------
	int PID, p_size, chunk;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &p_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &PID);

	//--- for k_means ---------------
	double delta;
	int i,j;   

	if(PID == 0)
	{
		initialize_object();
		//------- initialize cluster centers -----
		initialize_cluster();
		//---- intialize membership ------------- 
		initialize_membership();
	}

	if(PID == 0)
	{
		printf("1st: \n");
		printcluster();
	}

	delta = N;
	while(delta > THRESHOLD)
	{
		//--- update to get new centers for each cluster---
		if(PID == 0)
		{
			for(i = 0; i < K; i++)
			{
				double new_center_x = 0;
				double new_center_y = 0; 
				double cluster_num = 0;
				for(j = 0; j < N; j++)
				{
					if(membership[j] == i)
					{
						new_center_x += objects_x[j];
						new_center_y += objects_y[j];
						cluster_num ++;
					}
				}
				clusters_x[i] = new_center_x/cluster_num;
				clusters_y[i] = new_center_y/cluster_num;
			}
		
		}       

		delta = 0.0; 

		int offset;
		chunk = N / p_size;

		//--- Broadcast clusters --------
		MPI_Bcast(&clusters_x[0], K, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(&clusters_y[0], K, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		//--- Broadcast membership --------
		MPI_Bcast(&membership[0], N, MPI_INT, 0, MPI_COMM_WORLD);
		//--send the remaining splitted objects to slaves--- 
		if(PID == 0)
		{
			offset = N - (p_size - 1) * chunk;
			for(i = 1; i < p_size; i++)
			{
				MPI_Send(&objects_x[offset], chunk, MPI_DOUBLE, i, tag_1, MPI_COMM_WORLD);
				MPI_Send(&objects_y[offset], chunk, MPI_DOUBLE, i, tag_2, MPI_COMM_WORLD);
				offset += chunk;
			}
		}
		else
		{
			offset = N - (p_size -1) * chunk;
			MPI_Recv(&objects_x[offset + (PID - 1) * chunk], chunk, MPI_DOUBLE, 0, tag_1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&objects_y[offset + (PID - 1) * chunk], chunk, MPI_DOUBLE, 0, tag_2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);	
		}

		//--- calculate to update each obj's membership ----
		//--- divide the obj into groups for different nodes-
		//---- for calculation ------------------------------
		int upper_bound;
		if(PID == 0)
		{
			offset = 0;
			upper_bound = N - (p_size - 1) * chunk;
		}
		else
		{
			offset = N - (p_size - 1) * chunk + (PID - 1) * chunk;
			upper_bound = offset + chunk; 
		}
		for(i = offset; i < upper_bound; i++)
		{
			double dist_min = get_dist(objects_x[i], objects_y[i], clusters_x[0], clusters_y[0]);
			int n = 0;
			for(j = 0; j < K; j++)
			{
				double dist = get_dist(objects_x[i], objects_y[i], clusters_x[j], clusters_y[j]);
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

		//---- master get membership result from slaves -----------
		//---- master add up the delta value ----------------------
		if(PID == 0)
		{
			offset = N - (p_size - 1) * chunk;
			for(i = 1; i < p_size; i++)
			{
				MPI_Recv(&membership[offset], chunk, MPI_INT, i, tag_3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				double tmp_delta = 0;
				MPI_Recv(&tmp_delta, 1, MPI_DOUBLE, i, tag_4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				delta += tmp_delta;
				offset += chunk;
			}
		}
		else
		{
			offset = N - (p_size - 1) * chunk + (PID - 1) * chunk;
			MPI_Send(&membership[offset], chunk, MPI_INT, 0, tag_3, MPI_COMM_WORLD);
			MPI_Send(&delta, 1, MPI_DOUBLE, 0, tag_4, MPI_COMM_WORLD);
		}

		//---- root node return the true delta value to slaves for deciding --
		//--- whether to stop or not -----------------------------------------
		if(PID == 0)
		{
			for(i = 1; i < p_size; i++)
				MPI_Send(&delta, 1, MPI_DOUBLE, i, tag_5, MPI_COMM_WORLD);
		}
		else
		{
			MPI_Recv(&delta, 1, MPI_DOUBLE, 0, tag_5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}
	//-- printf the resulting cluster ------
	if(PID == 0)
	{	
		printcluster();
	}
	MPI_Finalize();
}


int main()
{
	k_means_clustering();

	return EXIT_SUCCESS;    
}
