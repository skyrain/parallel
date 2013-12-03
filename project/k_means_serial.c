#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define N 100
#define K 5
#define THRESHOLD 0.1
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
   dist = abs(pow(tmp_1 + tmp_2, 0.5));
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

    while(delta / (double)N > THRESHOLD)
    {
        delta = 0.0;

        for(i = 0; i < N; i++)
        {
            dist_min = get_dist(objects[i], clusters[0]);

            for(j = 0; j < K; j++)
            {
                double dist = get_dist(objects[i], clusters[j]);
                 
            }
        }
    }

}


int main()
{

    return 0;    
}
